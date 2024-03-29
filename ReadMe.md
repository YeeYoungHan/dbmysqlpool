﻿# MySQL DB pool 라이브러리
C++ STL 기반으로 MySQL DB pool 개발 프로젝트입니다.

### 개요

* C++ STL 라이브러리를 이용한 MySQL DB pool 라이브러리 개발
 
### 개발자 정보
본 프로젝트의 개발자 정보는 다음과 같습니다.

* 이메일 : websearch@naver.com
* 블로그 : http://blog.naver.com/websearch

### 라이선스

* 본 프로젝트의 라이선스는 GPLv3 이고 기업용 라이선스는 websearch@naver.com 으로 문의해 주세요.

### API 문서

* https://yeeyounghan.github.io/doc/DbMySQLPool/html/index.html

### 빌드 방법

* 윈도우
  * VC++ 2008 로 DbMySQLPool.sln 파일을 열어서 빌드한다.

* 리눅스
  * C++, mysqlclient 라이브러리를 인스톨한 후, 아래와 같이 실행하여서 빌드한다.

```
# make
```

### 폴더 설명

* DbMySQLPool
  * C++ STL 라이브러리를 이용한 MySQL DB pool 라이브러리

* mysql
  * MS 윈도우용 mysql 클라이언트 라이브러리

* MySQLQueryMonitor
  * MySQL 쿼리 모니터링 프로그램 ( 가장 시간이 많이 소요되는 SQL 리스트를 보여준다. )

* MySQLSlaveOverWatcher
  * MySQL slave 감시 서비스

* MySQLAlterTable
  * ALTER TABLE 명령을 DB 에 포함된 모든 테이블에 실행한다.

* Publish
  * WIN32 용 빌드 버전 저장 폴더

* ServerPlatform
  * 윈도우/리눅스 서비스 개발을 위한 라이브러리

* SipPlatform
  * 멀티 OS 지원을 위한 라이브러리

* TestDbMySQLPool
  * MySQL DB pool 라이브러리 테스트 프로그램

* XmlParser
  * XML 문자열 또는 파일 파서 라이브러리

### DB pool 사용법
아래의 블로그 포스트에 DB pool 사용법이 기술되어 있습니다.

* https://blog.naver.com/websearch/220909953904 : C++ MySQL DB pool 라이브러리
* https://blog.naver.com/websearch/220918790404 : N 개 컬럼 fetch 하는 방법
* https://blog.naver.com/websearch/220918799383 : 동적 SQL 문 실행하는 방법
* https://blog.naver.com/websearch/220919689037 : C++ MySQL DB pool 의 장점
* https://blog.naver.com/websearch/221146053989 : N 개 컬럼 fetch 하면서 컬럼 NULL 체크하는 방법
* https://blog.naver.com/websearch/221384048874 : INSERT, UPDATE, DELETE SQL 큐 생성 및 큐 쓰레드 생성 방법
* https://blog.naver.com/websearch/221399110325 : MySQLSlaveOverWatcher 0.01 버전 배포
* https://blog.naver.com/websearch/221498734022 : MySQL read, write timeout 설정 방법
* https://blog.naver.com/websearch/222204750631 : MySQL 8.x 로그인 기능 추가
