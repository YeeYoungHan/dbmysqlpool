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
#include "MySQLQueryMonitorSetup.h"
#include "Log.h"
#include <sys/stat.h>
#include "MemoryDebug.h"

CMySQLQueryMonitorSetup gclsSetup;

CMySQLQueryMonitorSetup::CMySQLQueryMonitorSetup() : m_iDbPort(3306), m_iDbMonitorPeriod(1), m_iMaxQueryCount(10)
{
}

CMySQLQueryMonitorSetup::~CMySQLQueryMonitorSetup()
{
}

/**
 * @ingroup MySQLQueryMonitor
 * @brief 설정파일 XML 파일을 파싱하여서 내부 변수에 저장한다.
 * @param pszFileName 설정파 XML 파일의 full path
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CMySQLQueryMonitorSetup::Read( const char * pszFileName )
{
	CXmlElement clsXml, * pclsElement;
	std::string strTemp;

	if( clsXml.ParseFile( pszFileName ) == false )
	{
		return false;
	}

	// DB
	pclsElement = clsXml.SelectElement( "Db" );
	if( pclsElement == NULL )
	{
		CLog::Print( LOG_ERROR, "%s Db element is not found", __FUNCTION__ );
		return false;
	}

	pclsElement->SelectElementTrimData( "Host", m_strDbHost );
	pclsElement->SelectElementData( "Port", m_iDbPort );
	pclsElement->SelectElementTrimData( "UserId", m_strDbUserId );
	pclsElement->SelectElementTrimData( "PassWord", m_strDbPassWord );

	if( m_strDbHost.empty() )
	{
		return false;
	}

	if( m_strDbUserId.empty() )
	{
		return false;
	}

	bool bRes = Read( clsXml );

	if( bRes )
	{
		m_strFileName = pszFileName;
		SetFileSizeTime();
	}

	return bRes;
}

/**
 * @ingroup MySQLQueryMonitor
 * @brief 수정된 설정 파일을 읽는다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CMySQLQueryMonitorSetup::Read( )
{
	if( m_strFileName.length() == 0 ) return false;

	CXmlElement clsXml;

	if( clsXml.ParseFile( m_strFileName.c_str() ) == false ) return false;

	Read( clsXml );
	SetFileSizeTime();

	return true;
}

/**
 * @ingroup MySQLQueryMonitor
 * @brief 설정 파일의 정보 중에서 실시간으로 변경 가능한 항목을 다시 저장한다.
 * @param clsXml 설정 파일의 내용을 저장한 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CMySQLQueryMonitorSetup::Read( CXmlElement & clsXml )
{
	CXmlElement * pclsElement;

	pclsElement = clsXml.SelectElement( "Db" );
	if( pclsElement )
	{
		pclsElement->SelectElementData( "MonitorPeriod", m_iDbMonitorPeriod );
		if( m_iDbMonitorPeriod <= 0 )
		{
			m_iDbMonitorPeriod = 1;
		}

		pclsElement->SelectElementData( "MaxQueryCount", m_iMaxQueryCount );
		if( m_iMaxQueryCount <= 0 )
		{
			m_iMaxQueryCount = 10;
		}
	}

	return true;
}

/**
 * @ingroup MySQLQueryMonitor
 * @brief 설정파일이 수정되었는지 확인한다.
 * @returns 설정파일이 수정되었으면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CMySQLQueryMonitorSetup::IsChange()
{
	struct stat	clsStat;

	if( stat( m_strFileName.c_str(), &clsStat ) == 0 )
	{
		if( m_iFileSize != clsStat.st_size || m_iFileTime != clsStat.st_mtime ) return true;
	}

	return false;
}

/**
 * @ingroup MySQLQueryMonitor
 * @brief 설정파일의 저장 시간을 저장한다.
 */
void CMySQLQueryMonitorSetup::SetFileSizeTime( )
{
	struct stat	clsStat;

	if( stat( m_strFileName.c_str(), &clsStat ) == 0 )
	{
		m_iFileSize = clsStat.st_size;
		m_iFileTime = clsStat.st_mtime;
	}
}

