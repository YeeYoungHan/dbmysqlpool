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

#include "MySQLAlterTable.h"

bool FetchTable( void * pclsData, MYSQL_ROW & sttRow )
{
	STRING_LIST * pclsList = (STRING_LIST *)pclsData;
	int iCol = 0;
	std::string strTable;

	DbSetData( sttRow, iCol, strTable );

	pclsList->push_back( strTable );

	return true;
}

int main( int argc, char * argv[] )
{
	if( argc < 7 )
	{
		printf( "[Usage] %s {db ip} {db userid} {db password} {db name} {db port} {alter command}\n", argv[0] );
		return 0;
	}

	CDbMySQLConnection clsDB;
	const char * pszHost = argv[1];
	const char * pszUserId = argv[2];
	const char * pszPassWord = argv[3];
	const char * pszDbName = argv[4];
	int iDbPort = atoi(argv[5]);
	const char * pszCommand = argv[6];

	if( clsDB.Connect( pszHost, pszUserId, pszPassWord, pszDbName, iDbPort, "utf8" ) == false )
	{
		printf( "db connect error\n" );
		return 0;
	}

	char szSQL[1024];
	STRING_LIST clsTableList;

	snprintf( szSQL, sizeof(szSQL), "SELECT TABLE_NAME FROM information_schema.tables WHERE table_schema = '%s'", pszDbName );
	if( clsDB.Query( szSQL, &clsTableList, FetchTable ) == false )
	{
		printf( "select table error\n" );
	}
	else
	{
		STRING_LIST::iterator itTL;

		for( itTL = clsTableList.begin(); itTL != clsTableList.end(); ++itTL )
		{
			snprintf( szSQL, sizeof(szSQL), "ALTER TABLE %s %s", itTL->c_str(), pszCommand );
			if( clsDB.Execute( szSQL ) == false )
			{
				printf( "alter table error\n" );
				//break;
			}
		}
	}

	clsDB.Close();

	return 0;
}
