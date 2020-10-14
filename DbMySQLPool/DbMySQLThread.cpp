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
 * @brief MySQL ���� ������ ���� ���� Ŭ����
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
 * @brief MySQL ���� ������
 * @param lpParameter CDbMySQLThreadArg ��ü�� ������
 * @returns 0 �� �����Ѵ�.
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
		// SQL Queue ���� ���� ����� ���, ������ ���� signal �� ������ �־�� �ϹǷ� signal �� �������� �ʱ� ���ؼ�
		// 20ms �������� ť�� �˻��Ѵ�.
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
 * @brief MySQL ���� ��������� �����Ѵ�.
 * @param iThreadCount MySQL ���� ������ ����
 * @param pclsSqlQueue SQL ���ڿ� ť
 * @param pszHost			MySQL ȣ��Ʈ �̸� or IP �ּ�
 * @param pszUserId		MySQL ���� ���̵�
 * @param pszPassWord MySQL ���� ���̵��� ��й�ȣ
 * @param pszDbName		MySQL ���� �����ͺ��̽� �̸�
 * @param iPort				MySQL ���� ��Ʈ ��ȣ
 * @param pszCharacterSet	character set
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief MySQL ���� �����带 �����Ѵ�.
 * @param pclsDbConn		MySQL DB ���� ��ü
 * @param pclsSqlQueue	SQL ���ڿ� ť
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief MySQL ���� �����忡 ���� ����� �����Ѵ�.
 */
void StopDbMySQLThread( )
{
	gbStopThread = true;
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL ���� �����尡 ���� ������ �˻��Ѵ�.
 * @returns MySQL ���� �����尡 �������̸� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool IsDbMySQLThreadRun( )
{
	if( giThreadCount > 0 ) return true;

	return false;
}
