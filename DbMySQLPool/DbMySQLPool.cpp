/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "SipPlatformDefine.h"
#include "DbMySQLPool.h"
#include "TimeUtility.h"
#include "Log.h"
#include "MemoryDebug.h"

CDbMySQLPool::CDbMySQLPool() : m_bDestroy(false)
{
}

CDbMySQLPool::~CDbMySQLPool()
{
	Destroy();
}

/**
 * @ingroup DbMySQLPool
 * @brief DB 에 연결한다.
 * @param iPoolCount	DB pool 에 포함될 DB 연결 개수
 * @param pszHost			MySQL 호스트 이름 or IP 주소
 * @param pszUserId		MySQL 접속 아이디
 * @param pszPassWord MySQL 접속 아이디의 비밀번호
 * @param pszDbName		MySQL 접속 데이터베이스 이름
 * @param iPort				MySQL 접속 포트 번호
 * @param pszCharacterSet	character set
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::Create( int iPoolCount, const char * pszHost, const char * pszUserId, const char * pszPassWord, const char * pszDbName, int iPort, const char * pszCharacterSet )
{
	if( iPoolCount <= 0 ) return false;

	bool bError = false;

	m_clsMutex.acquire();
	for( int i = 0; i < iPoolCount; ++i )
	{
		CDbMySQLConnection * pclsDB = new CDbMySQLConnection( );
		if( pclsDB == NULL )
		{
			CLog::Print( LOG_ERROR, "new CDbMySQLConnection error - index(%d)", i );
			bError = true;
			break;
		}

		if( pclsDB->Connect( pszHost, pszUserId, pszPassWord, pszDbName, iPort, pszCharacterSet ) == false )
		{
			CLog::Print( LOG_ERROR, "pclsDB->Connect error - index(%d)", i );
			bError = true;
			break;
		}

		m_clsStandByList.push_back( pclsDB );
	}
	m_clsMutex.release();

	if( bError )
	{
		Destroy();
		return false;
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief DB pool 에 포함된 DB 연결을 해제하고 리소스를 정리한다.
 */
void CDbMySQLPool::Destroy( )
{
	DB_CONNECTION_LIST::iterator	itList;
	bool	bUse = true;

	// DB connection 을 Select 할 수 없도록 설정한다.
	m_bDestroy = true;

	// 사용되고 있는 DB connection 이 존재하지 않을 때까지 최대 10초간 대기한다.
	for( int i = 0; i < 500; ++i )
	{
		m_clsMutex.acquire();
		if( m_clsUseMap.size() == 0 )
		{
			bUse = false;
		}
		m_clsMutex.release();

		if( bUse == false ) break;

		MiliSleep( 20 );
	}

	m_clsMutex.acquire();
	for( itList = m_clsStandByList.begin(); itList != m_clsStandByList.end(); ++itList )
	{
		(*itList)->Close();
		delete (*itList);
	}
	m_clsStandByList.clear();
	m_clsMutex.release();

	m_bDestroy = false;
}

/**
 * @ingroup DbMySQLPool
 * @brief DB pool 에서 DB 연결 객체를 가져온다.
 * @param ppclsDbConn DB 연결 객체의 포인터
 * @param bStandByUntilAvailable 사용할 수 있는 DB 연결 객체가 존재할 때까지 대기하면 true 이고 그렇지 않으면 false.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::Select( CDbMySQLConnection ** ppclsDbConn, bool bStandByUntilAvailable )
{
	if( ppclsDbConn == NULL ) return false;
	if( m_bDestroy ) return false;

	bool bRes = false;

	while( m_bDestroy == false )
	{
		m_clsMutex.acquire();
		if( m_clsStandByList.size() > 0 )
		{
			*ppclsDbConn = m_clsStandByList.front();
			m_clsStandByList.pop_front();

			m_clsUseMap.insert( DB_CONNECTION_MAP::value_type( *ppclsDbConn, *ppclsDbConn ) );
			bRes = true;
		}
		m_clsMutex.release();

		if( bRes ) break;
		if( bStandByUntilAvailable == false ) break;

		MiliSleep( 20 );
	}

	return bRes;
}

/**
 * @ingroup DbMySQLPool
 * @brief DB 연결 객체를 DB pool 에 반환한다.
 * @param pclsDbConn DB 연결 객체
 */
void CDbMySQLPool::Release( CDbMySQLConnection * pclsDbConn )
{
	if( pclsDbConn == NULL ) return;

	DB_CONNECTION_MAP::iterator itMap;

	m_clsMutex.acquire();
	itMap = m_clsUseMap.find( pclsDbConn );
	if( itMap != m_clsUseMap.end() )
	{
		m_clsUseMap.erase( itMap );
		m_clsStandByList.push_back( pclsDbConn );
	}
	m_clsMutex.release();
}

/**
 * @ingroup DbMySQLPool
 * @brief INSERT / UPDATE / DELETE SQL 문을 실행한다.
 * @param pszSQL SQL 문
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::Execute( const char * pszSQL )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->Execute( pszSQL );
}

/**
 * @ingroup DbMySQLPool
 * @brief INSERT 문의 실행한다.
 * @param pszSQL	SQL 문
 * @param piId		AUTO_INCREMENT 로 생성된 정수 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::Insert( const char * pszSQL, uint64_t * piId )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->Insert( pszSQL, piId );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT 문을 실행한다.
 * @param pszSQL		SQL 문
 * @param pclsData	응용 프로그램 변수
 * @param FetchRow	1개의 Row 마다 실행되는 함수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, MYSQL_ROW & sttRow ) )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->Query( pszSQL, pclsData, FetchRow );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) 와 같은 1개의 row, column 인 SQL 문을 실행한다.
 * @param pszSQL	SQL 문
 * @param iData		검색 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, int & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) 와 같은 1개의 row, column 인 SQL 문을 실행한다.
 * @param pszSQL	SQL 문
 * @param iData		검색 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, uint32_t & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) 와 같은 1개의 row, column 인 SQL 문을 실행한다.
 * @param pszSQL	SQL 문
 * @param iData		검색 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, uint64_t & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) 와 같은 1개의 row, column 인 SQL 문을 실행한다.
 * @param pszSQL	SQL 문
 * @param iData		검색 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, int64_t & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT string 와 같은 1개의 row, column 인 SQL 문을 실행한다.
 * @param pszSQL	SQL 문
 * @param strData 검색 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, std::string & strData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, strData );
}
