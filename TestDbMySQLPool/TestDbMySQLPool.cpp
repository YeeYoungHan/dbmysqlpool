#include "SipPlatformDefine.h"
#include "DbMySQLConnection.h"
#include "DbMySQLPool.h"
#include "Log.h"
#include "MemoryDebug.h"

int main( int argc, char * argv[] )
{
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

	// ===========================================================================
	// CDbMySQLConnection 사용 예제

	CDbMySQLConnection clsDB;

	if( clsDB.Connect( "127.0.0.1", pszUserId, pszPassWord, pszDbName, 3306 ) == false )
	{
		printf( "clsDB.Connect error\n" );
		return 0;
	}

	// 동적 SQL 을 실행하여서 SELECT 한 결과 가져오는 예제
	clsDB.QueryOne( "SELECT password(?)", "1234", strRes, 50 );
	printf( "[%s]\n", strRes.c_str() );

	clsDB.Close();

	// ===========================================================================
	// CDbMySQLPool 사용 예제

	CDbMySQLPool		clsPool;
	uint64_t	iId;
	int iCount;

	// 2개의 DB 연결을 사용하는 DB connection POOL 생성
	if( clsPool.Create( 2, "127.0.0.1", pszUserId, pszPassWord, pszDbName, 3306 ) == false )
	{
		printf( "clsPool.Create() error\n" );
		return -1;
	}

	// INSERT, UPDATE, DELETE 실행 예제
	clsPool.Execute( "DELETE FROM test" );

	// 테이블에 INSERT 하고 AUTO_INCREMENT 된 컬럼의 값을 가져오는 예제
	clsPool.Insert( "INSERT INTO test( name ) VALUES( 'name_1' )", &iId );

	// 테이블에서 하나의 컬럼을 SELECT 하여서 가져오는 예제
	clsPool.QueryOne( "SELECT count(*) FROM test", iCount );

	// DB connection POOL 에서 connection 객체를 가져와서 N 번 SQL 문을 실행하는 예제
	{
		CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsCon( clsPool );

		if( clsPool.Select( &clsCon.m_pclsData ) )
		{
			clsCon.m_pclsData->Execute( "INSERT INTO test( name ) VALUES( 'name_2' )" );
			clsCon.m_pclsData->Insert( "INSERT INTO test( id, name ) VALUES( NULL, 'test' )", &iId );
			clsCon.m_pclsData->Execute( "INSERT INTO test( id, name ) VALUES( ?, ? )", 2, "1", "new_name" );
		}
	}

	// DB connection POOL 종료
	clsPool.Destroy();

	CDbMySQLConnection::Final( );

	return 0;
}
