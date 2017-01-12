#include "SipPlatformDefine.h"
#include "DbMySQLConnection.h"
#include "Log.h"
#include "MemoryDebug.h"

int main( int argc, char * argv[] )
{
	CDbMySQLConnection clsDB;
	char * pszUserId, * pszPassWord, * pszDbName;
	std::string strRes;

	if( argc != 4 )
	{
		printf( "[Usage] %s {db user id} {db password} {db name}\n", argv[0] );
		return 0;
	}

#ifdef WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	CLog::SetLevel( LOG_INFO | LOG_SQL | LOG_DEBUG );

	pszUserId = argv[1];
	pszPassWord = argv[2];
	pszDbName = argv[3];

	if( clsDB.Connect( "127.0.0.1", pszUserId, pszPassWord, pszDbName, 3306 ) == false )
	{
		printf( "clsDB.Connect error\n" );
		return 0;
	}

	clsDB.QueryOne( "SELECT password(?)", "1234", strRes, 50 );

	printf( "[%s]\n", strRes.c_str() );

	clsDB.Close();
	mysql_library_end();

	return 0;
}
