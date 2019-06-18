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

#ifndef _MYSQL_QUERY_MONITOR_SETUP_H_
#define _MYSQL_QUERY_MONITOR_SETUP_H_

#include "XmlElement.h"

/**
 * @ingroup MySQLQueryMonitor
 * @brief IP-PBX ���� ���� Ŭ����
 */
class CMySQLQueryMonitorSetup
{
public:
	CMySQLQueryMonitorSetup();
	~CMySQLQueryMonitorSetup();

	bool Read( const char * pszFileName );
	bool Read();
	bool IsChange();

	// ================================================================
	// DB ����

	/** DB IP �ּ� */
	std::string		m_strDbHost;

	/** DB ��Ʈ ��ȣ */
	int						m_iDbPort;

	/** DB ����� ���̵� */
	std::string		m_strDbUserId;

	/** DB ����� ��й�ȣ */
	std::string		m_strDbPassWord;

	/** DB ���� �ֱ� (�ʴ���) */
	int						m_iDbMonitorPeriod;

	/** ������ �ִ� ���� ���� */
	int						m_iMaxQueryCount;

private:
	bool Read( CXmlElement & clsXml );
	void SetFileSizeTime( );

	std::string	m_strFileName;	// ���� ���� �̸�
	time_t			m_iFileTime;		// ���� ���� ���� �ð�
	int					m_iFileSize;		// ���� ���� ũ��
};

extern CMySQLQueryMonitorSetup gclsSetup;

#endif