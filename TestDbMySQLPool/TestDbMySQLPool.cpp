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
	// CDbMySQLConnection ��� ����

	CDbMySQLConnection clsDB;

	if( clsDB.Connect( "127.0.0.1", pszUserId, pszPassWord, pszDbName, 3306 ) == false )
	{
		printf( "clsDB.Connect error\n" );
		return 0;
	}

	// ���� SQL �� �����Ͽ��� SELECT �� ��� �������� ����
	clsDB.QueryOne( "SELECT password(?)", "1234", strRes, 50 );
	printf( "[%s]\n", strRes.c_str() );

	clsDB.Close();

	// ===========================================================================
	// CDbMySQLPool ��� ����

	CDbMySQLPool		clsPool;
	uint64_t	iId;
	int iCount;

	// 2���� DB ������ ����ϴ� DB connection POOL ����
	if( clsPool.Create( 2, "127.0.0.1", pszUserId, pszPassWord, pszDbName, 3306 ) == false )
	{
		printf( "clsPool.Create() error\n" );
		return -1;
	}

	// INSERT, UPDATE, DELETE ���� ����
	clsPool.Execute( "DELETE FROM test" );

	// ���̺��� INSERT �ϰ� AUTO_INCREMENT �� �÷��� ���� �������� ����
	clsPool.Insert( "INSERT INTO test( name ) VALUES( 'name_1' )", &iId );

	// ���̺����� �ϳ��� �÷��� SELECT �Ͽ��� �������� ����
	clsPool.QueryOne( "SELECT count(*) FROM test", iCount );

	// DB connection POOL ���� connection ��ü�� �����ͼ� N �� SQL ���� �����ϴ� ����
	{
		CAutoRelease< CDbMySQLPool, CDbMySQLConnection > clsCon( clsPool );

		if( clsPool.Select( &clsCon.m_pclsData ) )
		{
			clsCon.m_pclsData->Execute( "INSERT INTO test( name ) VALUES( 'name_2' )" );
			clsCon.m_pclsData->Insert( "INSERT INTO test( id, name ) VALUES( NULL, 'test' )", &iId );
		}
	}

	// DB connection POOL ����
	clsPool.Destroy();

	CDbMySQLConnection::Final( );

	return 0;
}