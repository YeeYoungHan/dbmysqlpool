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
#include "MySQLSlaveOverWatcher.h"
#include "MySQLSlaveOverWatcherSetup.h"
#include "MySQLSlaveOverWatcherVersion.h"
#include "ServerService.h"
#include "ServerUtility.h"
#include "TimeUtility.h"
#include "Log.h"
#include "DbMySQLConnection.h"
#include "MemoryDebug.h"

bool gbFork = true;

class CSlaveStatus
{
public:
	CSlaveStatus() : m_iLastErrNo(0)
	{
	}

	std::string m_strSlaveIORunning;
	std::string m_strSlaveSQLRunning;
	int					m_iLastErrNo;
	std::string m_strLastError;
};

bool FetchStatus( void * pclsData, MYSQL_ROW & sttRow )
{
	CSlaveStatus * pclsStatus = (CSlaveStatus *)pclsData;

	if( sttRow[10] ) pclsStatus->m_strSlaveIORunning = sttRow[10];
	if( sttRow[11] ) pclsStatus->m_strSlaveSQLRunning = sttRow[11];
	if( sttRow[18] ) pclsStatus->m_iLastErrNo = atoi( sttRow[18] );
	if( sttRow[19] ) pclsStatus->m_strLastError = sttRow[19];

	return true;
}

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief replication slave 가 SQL 문 오류로 정상 동작하지 않으면 오류가 발생한 SQL 문을 skip 한 후, replication slave 를 재시작한다.
 * @param clsDB MySQL DB 객체
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CheckSlave( CDbMySQLConnection & clsDB )
{
	CLog::Print( LOG_DEBUG, "%s is started", __FUNCTION__ );

	while( 1 )
	{
		CSlaveStatus clsStatus;

		clsDB.Query( "SHOW SLAVE STATUS", &clsStatus, FetchStatus );

		if( !strcmp( clsStatus.m_strSlaveIORunning.c_str(), "Yes" ) && 
				!strcmp( clsStatus.m_strSlaveSQLRunning.c_str(), "Yes" ) ) 
		{
			CLog::Print( LOG_DEBUG, " Slave_IO_Running: %s", clsStatus.m_strSlaveIORunning.c_str() );
			CLog::Print( LOG_DEBUG, "Slave_SQL_Running: %s", clsStatus.m_strSlaveSQLRunning.c_str() );
			CLog::Print( LOG_DEBUG, "       Last_Errno: %d", clsStatus.m_iLastErrNo );
			CLog::Print( LOG_DEBUG, "       Last_Error: %s", clsStatus.m_strLastError.c_str() );
			break;
		}

		CLog::Print( LOG_ERROR, " Slave_IO_Running: %s", clsStatus.m_strSlaveIORunning.c_str() );
		CLog::Print( LOG_ERROR, "Slave_SQL_Running: %s", clsStatus.m_strSlaveSQLRunning.c_str() );
		CLog::Print( LOG_ERROR, "       Last_Errno: %d", clsStatus.m_iLastErrNo );
		CLog::Print( LOG_ERROR, "       Last_Error: %s", clsStatus.m_strLastError.c_str() );

		clsDB.Execute( "STOP SLAVE" );
		clsDB.Execute( "SET GLOBAL SQL_SLAVE_SKIP_COUNTER = 1" );
		clsDB.Execute( "START SLAVE" );

		MiliSleep( gclsSetup.m_iDbReWatchPeriod );
	}

	CLog::Print( LOG_DEBUG, "%s is terminated", __FUNCTION__ );

	return true;
}

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief MySQL slave 감시자 서비스
 * @returns 정상 종료하면 0 을 리턴하고 오류가 발생하면 -1 를 리턴한다.
 */
int ServiceMain( )
{
#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	if( gclsSetup.Read( GetConfigFileName() ) == false && gclsSetup.Read( CONFIG_FILENAME ) == false )
	{
		printf( "config filename(%s) read error\n", GetConfigFileName() );
		return -1;
	}

	CLog::Print( LOG_SYSTEM, "MySQLSlaveOverWatcher is started ( version-%s %s %s )", MYSQL_SLAVE_OVERWATCHER_VERSION, __DATE__, __TIME__ );

	Fork( gbFork );
	SetCoreDumpEnable();
	ServerSignal();

	CDbMySQLConnection clsDB;
	int iSecond = 0;

	clsDB.Connect( gclsSetup.m_strDbHost.c_str(), gclsSetup.m_strDbUserId.c_str(), gclsSetup.m_strDbPassWord.c_str(), "", gclsSetup.m_iDbPort );

	CheckSlave( clsDB );

	while( gbStop == false )
	{
		sleep(1);
		++iSecond;

		if( iSecond == gclsSetup.m_iDbWatchPeriod )
		{
			CheckSlave( clsDB );
			iSecond = 0;
		}

		if( gclsSetup.IsChange() )
		{
			gclsSetup.Read();
		}
	}

	CLog::Print( LOG_SYSTEM, "MySQLSlaveOverWatcher is terminated" );
	CLog::Release();

	return 0;
}

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief MySQL slave 감시자 서비스
 * @param argc 
 * @param argv 
 * @returns 정상 종료하면 0 을 리턴하고 오류가 발생하면 -1 를 리턴한다.
 */
int main( int argc, char * argv[] )
{
	CServerService clsService;

	clsService.m_strName = SERVICE_NAME;
	clsService.m_strDisplayName = SERVICE_DISPLAY_NAME;
	clsService.m_strDescription = SERVICE_DESCRIPTION_STRING;
	clsService.m_strConfigFileName = CONFIG_FILENAME;
	clsService.m_strVersion = MYSQL_SLAVE_OVERWATCHER_VERSION;
	clsService.SetBuildDate( __DATE__, __TIME__ );

	if( argc == 3 && !strcmp( argv[2], "-n" ) )
	{
		gbFork = false;
	}

	ServerMain( argc, argv, clsService, ServiceMain );

	return 0;
}
