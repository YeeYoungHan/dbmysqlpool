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

#include "stdafx.h"
#include "SipPlatformDefine.h"
#include "ServerUtility.h"
#include "Directory.h"
#include "DbMySQLConnection.h"
#include "MySQLQueryMonitorSetup.h"
#include "QueryTimeList.h"

static bool gbStop = false;
static bool gbRun = false;
static HWND ghWnd;

bool FetchQuery( void * pclsData, MYSQL_ROW & sttRow )
{
	if( sttRow[0] && sttRow[1] )
	{
		QUERY_TIME_LIST * pclsList = (QUERY_TIME_LIST *)pclsData;

		CQueryTime clsTime;

		clsTime.m_iSecond = atoi(sttRow[0]);
		clsTime.m_strSQL = sttRow[1];

		pclsList->push_back( clsTime );
	}

	return true;
}

THREAD_API MySQLQueryThread( LPVOID lpParameter )
{
	gbRun = true;

	CDbMySQLConnection clsDB;

	std::string strFileName = CDirectory::GetProgramDirectory();
	CDirectory::AppendName( strFileName, "MySQLQueryMonitor.xml" );

	if( gclsSetup.Read( strFileName.c_str() ) == false )
	{
		::SendMessage( ghWnd, WM_MYSQL_QUERY_THREAD, PARAM_SETUP_ERROR, 0 );
	}
	else if( clsDB.Connect( gclsSetup.m_strDbHost.c_str(), gclsSetup.m_strDbUserId.c_str(), gclsSetup.m_strDbPassWord.c_str(), "", gclsSetup.m_iDbPort ) == false )
	{
		::SendMessage( ghWnd, WM_MYSQL_QUERY_THREAD, PARAM_CONNECT_ERROR, 0 );
	}
	else
	{
		DWORD dwStart, dwEnd;
		int iSleepSecond;

		while( gbStop == false )
		{
			QUERY_TIME_LIST clsList;

			dwStart = GetTickCount();
			clsDB.Query( "SELECT TIME, INFO FROM information_schema.processlist WHERE COMMAND = 'Query' AND TIME > 0", &clsList, FetchQuery );
			gclsQueryTimeList.Insert( clsList );

			::SendMessage( ghWnd, WM_MYSQL_QUERY_THREAD, PARAM_REFRESH, 0 );

			dwEnd = GetTickCount();

			iSleepSecond = gclsSetup.m_iDbMonitorPeriod - (( dwEnd - dwStart ) / 1000 );

			for( int i = 0; i < iSleepSecond; ++i )
			{
				Sleep(1000);
				if( gbStop ) break;
			}
		}
	}

	gbRun = false;

	return 0;
}

bool StartMySQLQueryThread( HWND hWnd )
{
	if( gbRun ) return true;

	ghWnd = hWnd;

	if( StartThread( "MySQLQueryThread", MySQLQueryThread, NULL ) == false ) return false;

	return true;
}

bool StopMySQLQueryThread()
{
	if( gbRun )
	{
		gbStop = true;
		
		while( gbRun == false )
		{
			Sleep(100);
		}

		gbStop = false;
	}

	return true;
}

bool IsMySQLQueryThreadRun()
{
	return gbRun;
}
