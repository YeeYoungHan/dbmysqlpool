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

#include "StdAfx.h"
#include "QueryTimeList.h"
#include "MySQLQueryMonitorSetup.h"

CQueryTimeList::CQueryTimeList()
{
}

CQueryTimeList::~CQueryTimeList()
{
}

void CQueryTimeList::Insert( QUERY_TIME_LIST & clsList )
{
	QUERY_TIME_LIST::iterator itQT;

	m_clsMutex.acquire();
	for( itQT = clsList.begin(); itQT != clsList.end(); ++itQT )
	{
		Insert( *itQT );
	}

	// QQQ: 쿼리 시간이 많이 소요된 순서로 다시 정렬한다.
	m_clsMutex.release();
}

void CQueryTimeList::Select( QUERY_TIME_LIST & clsList )
{
	m_clsMutex.acquire();
	clsList = m_clsList;
	m_clsMutex.release();
}

void CQueryTimeList::DeleteAll()
{
	m_clsMutex.acquire();
	m_clsList.clear();
	m_clsMutex.release();
}

void CQueryTimeList::Insert( CQueryTime & clsQT )
{
	QUERY_TIME_LIST::iterator itQT;
	bool bFound = false;

	// 입력된 쿼리가 리스트에 존재하면 쿼리 시간을 수정한다.
	for( itQT = m_clsList.begin(); itQT != m_clsList.end(); ++itQT )
	{
		if( !strcmp( clsQT.m_strSQL.c_str(), itQT->m_strSQL.c_str() ) )
		{
			if( clsQT.m_iSecond > itQT->m_iSecond )
			{
				itQT->m_iSecond = clsQT.m_iSecond;
			}

			bFound = true;
			break;
		}
	}

	if( bFound == false )
	{
		// 쿼리 시간이 많이 소요된 순서로 저장한다.
		for( itQT = m_clsList.begin(); itQT != m_clsList.end(); ++itQT )
		{
			if( clsQT.m_iSecond >= itQT->m_iSecond )
			{
				m_clsList.insert( itQT, clsQT );
				bFound = true;
				break;
			}
		}

		if( bFound == false )
		{
			if( gclsSetup.m_iMaxQueryCount > (int)m_clsList.size() )
			{
				m_clsList.push_back( clsQT );
			}
		}
	}
}
