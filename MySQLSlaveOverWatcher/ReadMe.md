# MySQLSlaveOverWatcher

### 개요

* MySQL replication slave 에서 SQL 오류가 발생한 경우, 해당 SQL 오류를 무시한 후 MySQL replication slave 를 재시작하는 서비스 프로그램

### 개발자 정보

* 이메일 : websearch@naver.com
* 블로그 : http://blog.naver.com/websearch

### 라이선스

* 본 프로젝트의 라이선스는 GPLv3 이고 기업용 라이선스는 websearch@naver.com 으로 문의해 주세요.

### 실행 방법
MySQLSlaveOverWatcher.exe 프로그램을 도스창 또는 리눅스 터미널에서 실행하는 방법은 다음과 같습니다.

```
dos> MySQLSlaveOverWatcher.exe c:\MySQLSlaveOverWatcher\MySQLSlaveOverWatcher.xml
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
