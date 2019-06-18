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

#pragma once

#include <string>
#include <list>
#include "SipMutex.h"

class CQueryTime
{
public:
	std::string m_strSQL;
	int m_iSecond;
};

typedef std::list< CQueryTime > QUERY_TIME_LIST;

class CQueryTimeList
{
public:
	CQueryTimeList();
	~CQueryTimeList();

	void Insert( QUERY_TIME_LIST & clsList );
	void Select( QUERY_TIME_LIST & clsList );
	void DeleteAll();

private:
	void Insert( CQueryTime & clsQT );

	QUERY_TIME_LIST m_clsList;
	CSipMutex m_clsMutex;
};
