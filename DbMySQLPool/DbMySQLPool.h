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

#ifndef _DB_MYSQL_POOL_H_
#define _DB_MYSQL_POOL_H_

/**
 * @defgroup DbPool DbPool
 * MySQL DB pool 라이브러리
 */

#include "AutoRelease.h"
#include "DbMySQLConnection.h"
#include "SipMutex.h"
#include <map>
#include <list>

typedef std::list< CDbMySQLConnection * > DB_CONNECTION_LIST;
typedef std::map< CDbMySQLConnection *, CDbMySQLConnection * > DB_CONNECTION_MAP;

/**
 * @ingroup DbPool
 * @brief DB Pool 관리 클래스
 */
class CDbMySQLPool
{
public:
	CDbMySQLPool();
	~CDbMySQLPool();

	bool Create( int iPoolCount, const char * pszHost, const char * pszUserId, const char * pszPassWord, const char * pszDbName, int iPort, const char * pszCharacterSet = NULL );
	void Destroy( );

	bool Select( CDbMySQLConnection ** ppclsDbConn, bool bStandByUntilAvailable = true );
	void Release( CDbMySQLConnection * pclsDbConn );

	bool Execute( const char * pszSQL );
	bool Insert( const char * pszSQL, uint64_t * piId );
	bool Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, MYSQL_ROW & sttRow ) );

private:
	CSipMutex	m_clsMutex;
	DB_CONNECTION_LIST m_clsStandByList;
	DB_CONNECTION_MAP m_clsUseMap;
	bool m_bDestroy;
};

#endif
