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
#include "XmlElement.h"
#include "Log.h"
#include "MemoryDebug.h"

CMySQLSlaveOverWatcherSetup gclsSetup;

CMySQLSlaveOverWatcherSetup::CMySQLSlaveOverWatcherSetup()
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

	if( clsXml.ParseFile( pszFileName ) == false ) return false;

	// DB
	pclsElement = clsXml.SelectElement( "Db" );
	if( pclsElement ) 
	{
		pclsElement->SelectElementTrimData( "Host", m_strDbHost );
		pclsElement->SelectElementData( "Port", m_iDbPort );
		pclsElement->SelectElementTrimData( "UserId", m_strDbUserId );
		pclsElement->SelectElementTrimData( "PassWord", m_strDbPassWord );
		pclsElement->SelectElementTrimData( "DbName", m_strDbName );
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

