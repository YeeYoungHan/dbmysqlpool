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

#include "MySQLLogExecute.h"

CDbMySQLConnection gclsDB;

int main( int argc, char * argv[] )
{
	char *pszIp, *pszUserId, *pszPassWord, *pszDbName, *pszGeneralLogFile;
	int iDbPort = 3306;
	bool bBulk = false;

	if( argc < 7 )
	{
		printf( "[Usage] %s {db ip} {db user id} {db password} {db name} {db port} {general log file} {1:bulk}\n", argv[0] );
		return 0;
	}

#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	pszIp = argv[1];
	pszUserId = argv[2];
	pszPassWord = argv[3];
	pszDbName = argv[4];
	iDbPort = atoi(argv[5]);
	pszGeneralLogFile = argv[6];

	if( argc >= 8 )
	{
		if( !strcmp( argv[7], "1" ) ) bBulk = true;
	}

#ifndef _DEBUG
	if( gclsDB.Connect( pszIp, pszUserId, pszPassWord, pszDbName, iDbPort) == false )
	{
		printf( "clsDB.Connect error\n" );
		return 0;
	}
#endif
	
	FILE * fd = fopen( pszGeneralLogFile, "r" );
	if( fd == NULL )
	{
		printf( "fopen(%s) error(%d)\n", pszGeneralLogFile, GetError() );
		return 0;
	}

	char szBuf[8192];
	int iLen;
	std::string strSQL;
	STRING_LIST clsList;
	STRING_LIST::iterator itList;

	memset( szBuf, 0, sizeof(szBuf) );

	while( fgets( szBuf, sizeof(szBuf), fd ) )
	{
		iLen = strlen( szBuf );

		if( iLen >= 2 && szBuf[iLen-2] == '\r' )
		{
			szBuf[iLen-2] = '\0';
		}
		else if( iLen >= 1 && szBuf[iLen-1] == '\n' )
		{
			szBuf[iLen-1] = '\0';
		}

		if( iLen >= 20 && !strncmp( szBuf + 9, "Query", 5 ) )
		{
			if( strSQL.empty() == false )
			{
				clsList.push_back( strSQL );
			}

			strSQL = szBuf + 15;
		}
		else if( iLen >= 34 && !strncmp( szBuf + 23, "Query", 5 ) )
		{
			if( strSQL.empty() == false )
			{
				clsList.push_back( strSQL );
			}

			strSQL = szBuf + 29;
		}
		else if( strSQL.empty() == false )
		{
			strSQL.append( szBuf );
		}

		memset( szBuf, 0, sizeof(szBuf) );
	}

	if( strSQL.empty() == false )
	{
		clsList.push_back( strSQL );
	}

	fclose( fd );

	struct timeval sttStart, sttEnd;
	const char * pszBuf;

	strSQL.clear();

	gettimeofday( &sttStart, NULL );

	for( itList = clsList.begin(); itList != clsList.end(); ++itList )
	{
		if( bBulk )
		{
			if( strSQL.empty() )
			{
				strSQL = *itList;
			}
			else
			{
				pszBuf = itList->c_str();

				pszBuf = strstr( pszBuf, "VALUES" );
				if( pszBuf == NULL )
				{
					printf( "VALUES is not found\n" );
					return 0;
				}

				strSQL.append( "," );
				strSQL.append( pszBuf + 6 );
			}
		}
		else
		{
			gclsDB.Execute( itList->c_str() );
		}
	}

	if( bBulk )
	{
		gclsDB.Execute( strSQL.c_str() );
	}

	gettimeofday( &sttEnd, NULL );

	int iMiliSecond = DiffTimeval( &sttStart, &sttEnd );

	printf( "%d ms\n", iMiliSecond );

	return 0;
}
