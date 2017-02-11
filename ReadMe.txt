===================
MySQL DB pool 라이브러리
===================

1. 개요

 * C++ STL 라이브러리를 이용한 MySQL DB pool 라이브러리 개발
 
2. 개발자 정보
  
 * 이영한
 * 이메일 : websearch@naver.com
 * 블로그 : http://blog.naver.com/websearch

3. 라이선스

 * 본 프로젝트의 라이선스는 GPLv3 이고 기업용 라이선스는 websearch@naver.com 으로 문의해 주세요.

4. DB pool 사용 예제

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
		}
	}

	// DB connection POOL 종료
	clsPool.Destroy();


