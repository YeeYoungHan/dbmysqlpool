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

#ifndef _DB_CONNECTION_H_
#define _DB_CONNECTION_H_

#include "SipPlatformDefine.h"
#include "SipTcp.h"
#include "mysql.h"
#include "StringUtility.h"
#include "Log.h"

#if MYSQL_VERSION_ID >= 50610
#define USE_PLUGIN_DIR
#endif

/**
 * @ingroup DbMySQLPool
 * @brief MySQL DB 연결 관리 및 쿼리 실행 클래스
 */
class CDbMySQLConnection
{
public:
	CDbMySQLConnection();
	~CDbMySQLConnection();

	bool Connect( const char * pszHost, const char * pszUserId, const char * pszPassWord, const char * pszDbName, int iPort, const char * pszCharacterSet = NULL );
	void Close( );

	bool Execute( const char * pszSQL );
	bool Execute( const char * pszSQL, int iArgCount, ... );
	bool Execute( const char * pszSQL, STRING_LIST & clsArgList );

	bool Insert( const char * pszSQL, uint64_t * piId );
	bool Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, MYSQL_ROW & sttRow ) );
	bool QueryOne( const char * pszSQL, int & iData );
	bool QueryOne( const char * pszSQL, uint8_t & cData );
	bool QueryOne( const char * pszSQL, uint32_t & iData );
	bool QueryOne( const char * pszSQL, uint64_t & iData );
	bool QueryOne( const char * pszSQL, int64_t & iData );
	bool QueryOne( const char * pszSQL, std::string & strData );
	bool QueryOne( const char * pszSQL, const char * pszArg, std::string & strData, int iDataSize );

	bool Prepare( const char * pszSQL );
	bool Bind( int iIndex, const char * pszVal );
	bool PrepareExecute( uint64_t * piId = NULL );
	bool PrepareClose( );

	uint64_t GetAffectedRow( );
	uint32_t GetError();
	bool IsExistTable( const char * pszTableName );

	void SetReadTimeout( int iSecond );
	void SetWriteTimeout( int iSecond );

#ifdef USE_PLUGIN_DIR
	void SetPluginDir( const char * pszPluginDir );
#endif

	void SetLogLevel( EnumLogLevel eLogLevel );

	static void Final( );

private:
	std::string	m_strDbHost;
	std::string	m_strDbUserId;
	std::string	m_strDbPassWord;
	std::string	m_strDbName;
	std::string m_strCharacterSet;
	std::string m_strPluginDir;
	int		m_iPort;

	bool	m_bConnected;
	MYSQL	m_sttMySQL;
	MYSQL_STMT * m_psttStmt;
	MYSQL_BIND * m_psttBind;
	int m_iBindCount;
	STRING_VECTOR m_clsBindList;

	int m_iReadTimeout;
	int m_iWriteTimeout;

	EnumLogLevel m_eLogLevel;

	bool Query( const char * pszSQL );
	bool Connect( );
};

#endif
