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
#include "Log.h"
#include "MemoryDebug.h"

bool gbFork = true;

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

	int iSecond = 0;

	while( gbStop == false )
	{
		sleep(1);
		++iSecond;
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
