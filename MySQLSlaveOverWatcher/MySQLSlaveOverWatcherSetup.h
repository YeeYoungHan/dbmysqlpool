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

#ifndef _MYSQL_SLAVE_OVERWATCHER_SETUP_H_
#define _MYSQL_SLAVE_OVERWATCHER_SETUP_H_

#include <string>

/**
 * @ingroup MySQLSlaveOverWatcher
 * @brief IP-PBX 정보 저장 클래스
 */
class CMySQLSlaveOverWatcherSetup
{
public:
	CMySQLSlaveOverWatcherSetup();
	~CMySQLSlaveOverWatcherSetup();

	bool Read( const char * pszFileName );


	// ================================================================
	// DB 설정

	/** DB IP 주소 */
	std::string		m_strDbHost;

	/** DB 포트 번호 */
	int						m_iDbPort;

	/** DB 사용자 아이디 */
	std::string		m_strDbUserId;

	/** DB 사용자 비밀번호 */
	std::string		m_strDbPassWord;

	/** DB 이름 */
	std::string		m_strDbName;
};

extern CMySQLSlaveOverWatcherSetup gclsSetup;

#endif
