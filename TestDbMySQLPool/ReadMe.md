# TestDbMySQLPool

### 개요

* DbMySQLPool 라이브러리 테스트 프로그램 개발

### 개발자 정보

* 이메일 : websearch@naver.com
* 블로그 : http://blog.naver.com/websearch

### 라이선스

* 본 프로젝트의 라이선스는 GPLv3 이고 기업용 라이선스는 websearch@naver.com 으로 문의해 주세요.

### 실행 방법

```
dos> TestDbMySQLPool.exe {db user id} {db password} {db name} {db port} {plugin path}
```

* MySQL 5.x 에서 실행하는 방법

```
dos> TestDbMySQLPool.exe root 1234 dbname
```

* MySQL 8.x 에서 실행하는 방법
  * mysql\32bit 에 caching_sha2_password.dll 이 포함되어 있다.

```
dos> TestDbMySQLPool.exe root 1234 dbname 3306 C:\DbMysqlPool\trunk\mysql\32bit
```

