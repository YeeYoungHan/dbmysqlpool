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
#include "DbMySQLQueue.h"
#include "Log.h"
#include "MemoryDebug.h"

CDbMySQLQueue::CDbMySQLQueue() : m_iMaxSize(0)
{
}

CDbMySQLQueue::~CDbMySQLQueue()
{
}

/**
 * @ingroup DbPool
 * @brief 큐에 SQL 문자열을 입력한다.
 * @param pszSQL SQL 문자열
 * @returns 성공하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CDbMySQLQueue::Insert( const char * pszSQL )
{
	bool bRes = true;

	m_clsMutex.acquire();
	if( m_iMaxSize != 0 )
	{
		if( (int)m_clsList.size() >= m_iMaxSize )
		{
			CLog::Print( LOG_ERROR, "%s queue size(%d) >= max size(%d) sql(%s) is dropped", __FUNCTION__, m_clsList.size(), m_iMaxSize, pszSQL );
			bRes = false;
		}
	}

	if( bRes )
	{
		m_clsList.push_back( pszSQL );
		if( m_clsList.size() == 1 )
		{
			m_clsMutex.signal();
		}
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup DbPool
 * @brief 큐에서 SQL 문자열을 가져온다.
 * @param strSQL	SQL 문자열 저장 변수
 * @param bWait		SQL 문자열이 존재할 때까지 대기하는가?
 * @returns SQL 문자열이 존재하면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CDbMySQLQueue::Select( std::string & strSQL, bool bWait )
{
	bool bRes = false;

	m_clsMutex.acquire();
	if( bWait )
	{
		if( m_clsList.size() == 0 )
		{
			m_clsMutex.wait();
		}
	}

	if( m_clsList.size() > 0 )
	{
		strSQL = m_clsList.front();
		m_clsList.pop_front();
		bRes = true;
	}
	m_clsMutex.release();

	return bRes;
}

/**
 * @ingroup DbPool
 * @brief 큐에 저장된 SQL 문자열의 개수를 리턴한다.
 * @returns 큐에 저장된 SQL 문자열의 개수를 리턴한다.
 */
int CDbMySQLQueue::GetSize( )
{
	int iCount;

	m_clsMutex.acquire();
	iCount = (int)m_clsList.size();
	m_clsMutex.release();

	return iCount;
}

/**
 * @ingroup DbPool
 * @brief 큐에 SQL 문자열이 존재할 때까지 대기하는 쓰레드에게 이벤트를 전송한다.
 */
void CDbMySQLQueue::Signal( )
{
	m_clsMutex.broadcast();
}

/**
 * @ingroup DbPool
 * @brief 큐에 저장할 수 있는 SQL 문자열의 최대 개수를 설정한다.
 * @param iMaxSize 큐에 저장할 수 있는 SQL 문자열의 최대 개수
 */
void CDbMySQLQueue::SetMaxSize( int iMaxSize )
{
	m_iMaxSize = iMaxSize;
}
