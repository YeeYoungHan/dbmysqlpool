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

CDbMySQLPool::CDbMySQLPool() : m_bDestroy(false), m_iReadTimeout(0), m_iWriteTimeout(0)
{
}

CDbMySQLPool::~CDbMySQLPool()
{
	Destroy();
}

/**
 * @ingroup DbMySQLPool
 * @brief DB �� �����Ѵ�.
 * @param iPoolCount	DB pool �� ���Ե� DB ���� ����
 * @param pszHost			MySQL ȣ��Ʈ �̸� or IP �ּ�
 * @param pszUserId		MySQL ���� ���̵�
 * @param pszPassWord MySQL ���� ���̵��� ��й�ȣ
 * @param pszDbName		MySQL ���� �����ͺ��̽� �̸�
 * @param iPort				MySQL ���� ��Ʈ ��ȣ
 * @param pszCharacterSet	character set
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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

		pclsDB->SetReadTimeout( m_iReadTimeout );
		pclsDB->SetWriteTimeout( m_iWriteTimeout );

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
 * @brief DB pool �� ���Ե� DB ������ �����ϰ� ���ҽ��� �����Ѵ�.
 */
void CDbMySQLPool::Destroy( )
{
	DB_CONNECTION_LIST::iterator	itList;
	bool	bUse = true;

	// DB connection �� Select �� �� ������ �����Ѵ�.
	m_bDestroy = true;

	// ���ǰ� �ִ� DB connection �� �������� ���� ������ �ִ� 10�ʰ� ����Ѵ�.
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
 * @brief DB pool ���� DB ���� ��ü�� �����´�.
 * @param ppclsDbConn DB ���� ��ü�� ������
 * @param bStandByUntilAvailable ����� �� �ִ� DB ���� ��ü�� ������ ������ ����ϸ� true �̰� �׷��� ������ false.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief DB ���� ��ü�� DB pool �� ��ȯ�Ѵ�.
 * @param pclsDbConn DB ���� ��ü
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
 * @brief INSERT / UPDATE / DELETE SQL ���� �����Ѵ�.
 * @param pszSQL SQL ��
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::Execute( const char * pszSQL )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->Execute( pszSQL );
}

/**
 * @ingroup DbMySQLPool
 * @brief INSERT ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param piId		AUTO_INCREMENT �� ������ ���� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::Insert( const char * pszSQL, uint64_t * piId )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->Insert( pszSQL, piId );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT ���� �����Ѵ�.
 * @param pszSQL		SQL ��
 * @param pclsData	���� ���α׷� ����
 * @param FetchRow	1���� Row ���� ����Ǵ� �Լ�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, MYSQL_ROW & sttRow ) )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->Query( pszSQL, pclsData, FetchRow );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, int & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, uint32_t & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, uint64_t & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, int64_t & iData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, iData );
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT string �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param strData �˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLPool::QueryOne( const char * pszSQL, std::string & strData )
{
	CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsData( *this );

	if( Select( &clsData.m_pclsData ) == false ) return false;

	return clsData.m_pclsData->QueryOne( pszSQL, strData );
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL read timeout �ð��� �����Ѵ�. �� �޼ҵ�� Connect() �޼ҵ带 ȣ���ϱ� ���� ȣ���ؾ� ��ȿ�ϴ�.
 * @param iSecond MySQL read timeout �ð� (�ʴ���)
 */
void CDbMySQLPool::SetReadTimeout( int iSecond )
{
	if( iSecond > 0 )
	{
		m_iReadTimeout = iSecond;
	}
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL write timeout �ð��� �����Ѵ�. �� �޼ҵ�� Connect() �޼ҵ带 ȣ���ϱ� ���� ȣ���ؾ� ��ȿ�ϴ�.
 * @param iSecond MySQL write timeout �ð� (�ʴ���)
 */
void CDbMySQLPool::SetWriteTimeout( int iSecond )
{
	if( iSecond > 0 )
	{
		m_iWriteTimeout = iSecond;
	}
}
