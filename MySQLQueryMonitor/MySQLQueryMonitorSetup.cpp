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
 * @brief �������� XML ������ �Ľ��Ͽ��� ���� ������ �����Ѵ�.
 * @param pszFileName ������ XML ������ full path
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief ������ ���� ������ �д´�.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief ���� ������ ���� �߿��� �ǽð����� ���� ������ �׸��� �ٽ� �����Ѵ�.
 * @param clsXml ���� ������ ������ ������ ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief ���������� �����Ǿ����� Ȯ���Ѵ�.
 * @returns ���������� �����Ǿ����� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
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
 * @brief ���������� ���� �ð��� �����Ѵ�.
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

