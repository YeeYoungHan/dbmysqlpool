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

#include "MySQLSlaveOverWatcherSetup.h"
#include "Log.h"
#include <sys/stat.h>
#include "MemoryDebug.h"

CMySQLSlaveOverWatcherSetup gclsSetup;

CMySQLSlaveOverWatcherSetup::CMySQLSlaveOverWatcherSetup() : m_iDbPort(3306), m_iDbWatchPeriod(60), m_iDbReWatchPeriod(20)
{
}

CMySQLSlaveOverWatcherSetup::~CMySQLSlaveOverWatcherSetup()
{
}

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief 설정파일 XML 파일을 파싱하여서 내부 변수에 저장한다.
 * @param pszFileName 설정파 XML 파일의 full path
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CMySQLSlaveOverWatcherSetup::Read( const char * pszFileName )
{
	CXmlElement clsXml, * pclsElement;
	std::string strTemp;

	if( clsXml.ParseFile( pszFileName ) == false )
	{
		CLog::Print( LOG_ERROR, "%s clsXml.ParseFile(%s) error", __FUNCTION__, pszFileName );
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
		CLog::Print( LOG_ERROR, "%s Db -> Host is not defined", __FUNCTION__ );
		return false;
	}

	if( m_strDbUserId.empty() )
	{
		CLog::Print( LOG_ERROR, "%s Db -> UserId is not defined", __FUNCTION__ );
		return false;
	}

	// 로그
	pclsElement = clsXml.SelectElement( "Log" );
	if( pclsElement )
	{
		if( pclsElement->SelectElementTrimData( "Folder", strTemp ) )
		{
			if( CLog::SetDirectory( strTemp.c_str() ) == false )
			{
				printf( "[SetupFile] CLog::SetDirectory(%s) error\n", strTemp.c_str() );
				return false;
			}
		}
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
 * @ingroup MySQLSlaveOverWatcher
 * @brief 수정된 설정 파일을 읽는다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CMySQLSlaveOverWatcherSetup::Read( )
{
	if( m_strFileName.length() == 0 ) return false;

	CXmlElement clsXml;

	if( clsXml.ParseFile( m_strFileName.c_str() ) == false ) return false;

	Read( clsXml );
	SetFileSizeTime();

	return true;
}

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief 설정 파일의 정보 중에서 실시간으로 변경 가능한 항목을 다시 저장한다.
 * @param clsXml 설정 파일의 내용을 저장한 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CMySQLSlaveOverWatcherSetup::Read( CXmlElement & clsXml )
{
	CXmlElement * pclsElement;

	pclsElement = clsXml.SelectElement( "Db" );
	if( pclsElement )
	{
		pclsElement->SelectElementData( "WatchPeriod", m_iDbWatchPeriod );
		pclsElement->SelectElementData( "ReWatchPeriod", m_iDbReWatchPeriod );

		if( m_iDbWatchPeriod <= 0 )
		{
			CLog::Print( LOG_ERROR, "%s Db -> WatchPeriod(%d) is not correct and change to 60", __FUNCTION__, m_iDbWatchPeriod );
			m_iDbWatchPeriod = 60;
		}

		if( m_iDbReWatchPeriod <= 0 )
		{
			CLog::Print( LOG_ERROR, "%s Db -> ReWatchPeriod(%d) is not correct and change to 20", __FUNCTION__, m_iDbReWatchPeriod );
			m_iDbReWatchPeriod = 20;
		}
	}

	pclsElement = clsXml.SelectElement( "Log" );
	if( pclsElement )
	{
		int iLogLevel = 0;

		CXmlElement * pclsClient = pclsElement->SelectElement( "Level" );
		if( pclsClient )
		{
			bool bTemp;

			pclsClient->SelectAttribute( "Debug", bTemp );
			if( bTemp ) iLogLevel |= LOG_DEBUG;

			pclsClient->SelectAttribute( "Info", bTemp );
			if( bTemp ) iLogLevel |= LOG_INFO;

			pclsClient->SelectAttribute( "Sql", bTemp );
			if( bTemp ) iLogLevel |= LOG_SQL;
		}

		int			iLogMaxSize = 0;
		int64_t	iLogMaxFolderSize = 0;

		pclsElement->SelectElementData( "MaxSize", iLogMaxSize );
		pclsElement->SelectElementData( "MaxFolderSize", iLogMaxFolderSize );

		CLog::SetLevel( iLogLevel );
		CLog::SetMaxLogSize( iLogMaxSize );
		CLog::SetMaxFolderSize( iLogMaxFolderSize );
	}

	return true;
}

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief 설정파일이 수정되었는지 확인한다.
 * @returns 설정파일이 수정되었으면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CMySQLSlaveOverWatcherSetup::IsChange()
{
	struct stat	clsStat;

	if( stat( m_strFileName.c_str(), &clsStat ) == 0 )
	{
		if( m_iFileSize != clsStat.st_size || m_iFileTime != clsStat.st_mtime ) return true;
	}

	return false;
}

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief 설정파일의 저장 시간을 저장한다.
 */
void CMySQLSlaveOverWatcherSetup::SetFileSizeTime( )
{
	struct stat	clsStat;

	if( stat( m_strFileName.c_str(), &clsStat ) == 0 )
	{
		m_iFileSize = clsStat.st_size;
		m_iFileTime = clsStat.st_mtime;
	}
}

