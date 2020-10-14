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
#include "DbMySQLThread.h"
#include "ServerUtility.h"
#include "TimeUtility.h"
#include "Log.h"
#include "MemoryDebug.h"

static bool gbStopThread = false;
static CSipMutex gclsMutex;
static int giThreadCount = 0;

/**
 * @ingroup DbMySQLPool
 * @brief MySQL 연동 쓰레드 인자 저장 클래스
 */
class CDbMySQLThreadArg
{
public:
	CDbMySQLThreadArg( CDbMySQLConnection * pclsDbConn, CDbMySQLQueue * pclsSqlQueue )
	{
		m_pclsDbConn = pclsDbConn;
		m_pclsSqlQueue = pclsSqlQueue;
	}

	CDbMySQLConnection * m_pclsDbConn;
	CDbMySQLQueue * m_pclsSqlQueue;
};

/**
 * @ingroup DbMySQLPool
 * @brief MySQL 연동 쓰레드
 * @param lpParameter CDbMySQLThreadArg 객체의 포인터
 * @returns 0 을 리턴한다.
 */
THREAD_API DbMySQLThread( LPVOID lpParameter )
{
	CDbMySQLThreadArg * pclsArg = (CDbMySQLThreadArg *)lpParameter;
	CDbMySQLConnection * pclsDbConn = pclsArg->m_pclsDbConn;
	CDbMySQLQueue * pclsSqlQueue = pclsArg->m_pclsSqlQueue;
	CDbMySQLQueueData clsData;

	delete pclsArg;

	gclsMutex.acquire();
	++giThreadCount;
	gclsMutex.release();

	while( gbStopThread == false )
	{
		// SQL Queue 에서 무한 대기할 경우, 종료할 때에 signal 을 전송해 주어야 하므로 signal 을 전송하지 않기 위해서
		// 20ms 간격으로 큐를 검사한다.
		if( pclsSqlQueue->Select( clsData, false ) )
		{
			if( clsData.m_clsArgList.empty() )
			{
				pclsDbConn->Execute( clsData.m_strSQL.c_str(), true );
			}
			else
			{
				pclsDbConn->Execute( clsData.m_strSQL.c_str(), clsData.m_clsArgList );
			}
		}
		else
		{
			MiliSleep( 20 );
		}
	}

	pclsDbConn->Close();

	delete pclsDbConn;

	gclsMutex.acquire();
	--giThreadCount;
	gclsMutex.release();

	return 0;
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL 연동 쓰레드들을 시작한다.
 * @param iThreadCount MySQL 연동 쓰레드 개수
 * @param pclsSqlQueue SQL 문자열 큐
 * @param pszHost			MySQL 호스트 이름 or IP 주소
 * @param pszUserId		MySQL 접속 아이디
 * @param pszPassWord MySQL 접속 아이디의 비밀번호
 * @param pszDbName		MySQL 접속 데이터베이스 이름
 * @param iPort				MySQL 접속 포트 번호
 * @param pszCharacterSet	character set
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool StartDbMySQLThread( int iThreadCount, CDbMySQLQueue * pclsSqlQueue, const char * pszHost, const char * pszUserId, const char * pszPassWord, const char * pszDbName, int iPort, const char * pszCharacterSet )
{
	bool bError = false;

	for( int i = 0; i < iThreadCount; ++i )
	{
		CDbMySQLConnection * pclsDbConn = new CDbMySQLConnection();
		if( pclsDbConn == NULL )
		{
			bError = true;
			break;
		}

		pclsDbConn->Connect( pszHost, pszUserId, pszPassWord, pszDbName, iPort, pszCharacterSet );

		if( StartDbMySQLThread( pclsDbConn, pclsSqlQueue ) == false )
		{
			bError = true;
			break;
		}
	}

	if( bError )
	{
		StopDbMySQLThread();
		return true;
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL 연동 쓰레드를 시작한다.
 * @param pclsDbConn		MySQL DB 연결 객체
 * @param pclsSqlQueue	SQL 문자열 큐
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool StartDbMySQLThread( CDbMySQLConnection * pclsDbConn, CDbMySQLQueue * pclsSqlQueue )
{
	CDbMySQLThreadArg * pclsArg = new CDbMySQLThreadArg( pclsDbConn, pclsSqlQueue );
	if( pclsArg == NULL )
	{
		CLog::Print( LOG_ERROR, "%s new error", __FUNCTION__ );
		return false;
	}

	gbStopThread = false;

	return StartThread( "DbMySQLThread", DbMySQLThread, pclsArg );
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL 연동 쓰레드에 중지 명령을 전송한다.
 */
void StopDbMySQLThread( )
{
	gbStopThread = true;
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL 연동 쓰레드가 실행 중인지 검사한다.
 * @returns MySQL 연동 쓰레드가 실행중이면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool IsDbMySQLThreadRun( )
{
	if( giThreadCount > 0 ) return true;

	return false;
}
