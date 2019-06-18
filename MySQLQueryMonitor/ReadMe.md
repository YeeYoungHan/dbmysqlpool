# MySQLQueryMonitor

### 개요

* MySQL 고부하 SQL 쿼리 모니터링 프로그램
* MySQL 에서 가장 많은 시간이 소요된 SQL 쿼리문을 보여준다.

### 개발자 정보

* 이메일 : websearch@naver.com
* 블로그 : http://blog.naver.com/websearch

### 라이선스

* 본 프로젝트의 라이선스는 GPLv3 이고 기업용 라이선스는 websearch@naver.com 으로 문의해 주세요.

### 설정 파일
본 프로그램을 실행하기 위해서는 MySQLQueryMonitor.xml 파일이 필요합니다. MySQLQueryMonitor.xml 파일의 내용과 각 항목에 대한 설명은 다음과 같습니다.

```
<MySQLQueryMonitor>
	<Db>
		<!-- MySQL 서버의 IP 주소 -->
		<Host>127.0.0.1</Host>
		<!-- MySQL 서버의 포트 번호 -->
		<Port>3306</Port>
		<!-- MySQL 서버 접속용 아이디. -->
		<UserId>user</UserId>
		<!-- MySQL 서버 접속용 비밀번호 -->
		<PassWord>password</PassWord>
		<!-- MySQL 쿼리 모니터링 주기 (초단위) -->
		<MonitorPeriod>1</MonitorPeriod>
		<!-- 화면에 보여줄 SQL 문의 최대 개 -->
		<MaxQueryCount>10</MaxQueryCount>
	</Db>
</MySQLQueryMonitor>
```
