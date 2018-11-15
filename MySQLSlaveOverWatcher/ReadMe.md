# MySQLSlaveOverWatcher

### 개요

* MySQL replication slave 에서 SQL 오류가 발생한 경우, 해당 SQL 오류를 무시한 후 MySQL replication slave 를 재시작하는 서비스 프로그램
* 해당 SQL 오류 무시 및 MySQL replication slave 를 재시작하는 방법은 아래의 포스트를 참고하세요.
  * https://blog.naver.com/websearch/221079433407

### 개발자 정보

* 이메일 : websearch@naver.com
* 블로그 : http://blog.naver.com/websearch

### 라이선스

* 본 프로젝트의 라이선스는 GPLv3 이고 기업용 라이선스는 websearch@naver.com 으로 문의해 주세요.

### 설정 파일
본 프로그램을 실행하기 위해서는 MySQLSlaveOverWatcher.xml 파일이 필요합니다. MySQLSlaveOverWatcher.xml 파일의 내용과 각 항목에 대한 설명은 다음과 같습니다.

```
<MySQLSlaveOverWatcher>
	<Db>
		<!-- MySQL 서버의 IP 주소 -->
		<Host>127.0.0.1</Host>
		<!-- MySQL 서버의 포트 번호 -->
		<Port>3306</Port>
		<!-- MySQL 서버 접속용 아이디. -->
		<UserId>user</UserId>
		<!-- MySQL 서버 접속용 비밀번호 -->
		<PassWord>password</PassWord>
		<!-- MySQL replication slave 동작 상태를 검사하는 주기 (초단위) -->
		<WatchPeriod>60</WatchPeriod>
		<!-- MySQL replication slave 동작 상태에 오류가 감지된 경우, 해당 SQL 오류를 무시한 후
		     MySQL replication slave 를 재시작하고 다시 MySQL replication slave 동작 상태를 
		     검사하는 주기 (ms 단위) -->
		<ReWatchPeriod>20</ReWatchPeriod>
	</Db>
	<Log>
		<!-- 로그 저장 폴더 -->
		<Folder>c:\temp\mysqlslave</Folder>
		<!-- 로그 레벨 설정 -->
		<Level Debug="true" Sql="true"></Level>
	</Log>
</MySQLSlaveOverWatcher>
```

### 실행 방법
MySQLSlaveOverWatcher.exe 프로그램을 도스창 또는 리눅스 터미널에서 실행하는 방법은 다음과 같습니다. 설정 파일은 full path 로 입력해 주어야 리눅스 데몬으로 실행시 주기적으로 설정 파일의 변경 유무를 확인할 수 있다.

```
dos> MySQLSlaveOverWatcher.exe c:\MySQLSlaveOverWatcher\MySQLSlaveOverWatcher.xml
```

### 리눅스 실행 방법
mysqlslaveoverwatcher.sh 스크립트에서 아래의 내용을 수정하면 mysqlslaveoverwatcher.sh 스크립트로 리눅스에서 MySQLSlaveOverWatcher.exe 프로그램을 시작/종료할 수 있습니다.

```
root_dir=/home/websearch/OpenProject/DbMySQLPool/MySQLSlaveOverWatcher
```

mysqlslaveoverwatcher.sh 스크립트로 MySQLSlaveOverWatcher.exe 프로그램을 시작하는 방법은 다음과 같습니다.

```
# ./mysqlslaveoverwatcher.sh start
```

mysqlslaveoverwatcher.sh 스크립트로 MySQLSlaveOverWatcher.exe 프로그램을 종료하는 방법은 다음과 같습니다.

```
# ./mysqlslaveoverwatcher.sh stop
```

### 윈도우 서비스로 인스톨 방법
MySQLSlaveOverWatcher.exe 프로그램을 윈도우 서비스로 인스톨하는 방법은 다음과 같습니다.

* 관리자 권한으로 도스창을 실행한다.
* 도스창에서 MySQLSlaveOverWatcher.exe 설치 폴더로 이동한다.
* MySQLSlaveOverWatcher.exe 설치 폴더에 MySQLSlaveOverWatcher.xml 파일이 존재해야 한다.
* 아래와 같이 실행한다.

```
dos> MySQLSlaveOverWatcher.exe -i
```

* 윈도우 서비스에서 MySQLSlaveOverWatcher 를 시작하면 MySQLSlaveOverWatcher 서비스가 시작됩니다.

### 윈도우 서비스로 언인스톨 방법
MySQLSlaveOverWatcher.exe 프로그램을 윈도우 서비스에서 언인스톨하는 방법은 다음과 같습니다.

* 관리자 권한으로 도스창을 실행한다.
* 도스창에서 MySQLSlaveOverWatcher.exe 설치 폴더로 이동한다.

* 아래와 같이 실행한다.

```
dos> MySQLSlaveOverWatcher.exe -u
```

