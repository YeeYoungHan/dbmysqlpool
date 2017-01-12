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

/**
 * @ingroup DbPool
 * @brief DB 연결 관리 및 쿼리 실행 클래스
 */
class CDbMySQLConnection
{
public:
	CDbMySQLConnection();
	~CDbMySQLConnection();

	bool Connect( const char * pszHost, const char * pszUserId, const char * pszPassWord, const char * pszDbName, int iPort, const char * pszCharacterSet = NULL );
	void Close( );

	bool Execute( const char * pszSQL );
	bool Insert( const char * pszSQL, uint64_t * piId );
	bool Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, MYSQL_ROW & sttRow ) );
	bool QueryOne( const char * pszSQL, int & iData );
	bool QueryOne( const char * pszSQL, uint8_t & cData );
	bool QueryOne( const char * pszSQL, uint32_t & iData );
	bool QueryOne( const char * pszSQL, uint64_t & iData );
	bool QueryOne( const char * pszSQL, std::string & strData );
	bool QueryOne( const char * pszSQL, const char * pszArg, std::string & strData );

	bool Prepare( const char * pszSQL );
	bool Bind( int iIndex, const char * pszVal );
	bool PrepareExecute( uint64_t * piId = NULL );
	bool PrepareClose( );

	bool PrepareExecute( const char * pszSQL, const char * pszArg );

	uint64_t GetAffectedRow( );
	uint32_t GetError();
	bool IsExistTable( const char * pszTableName );

private:
	std::string	m_strDbHost;
	std::string	m_strDbUserId;
	std::string	m_strDbPassWord;
	std::string	m_strDbName;
	std::string m_strCharacterSet;
	int		m_iPort;

	bool	m_bConnected;
	MYSQL	m_sttMySQL;
	MYSQL_STMT * m_psttStmt;
	MYSQL_BIND * m_psttBind;
	int m_iBindCount;
	STRING_VECTOR m_clsBindList;

	bool Query( const char * pszSQL );
	bool Connect( );
};

#endif
