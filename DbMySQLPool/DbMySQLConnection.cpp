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

#include "SipPlatformDefine.h"
#include "DbMySQLConnection.h"
#include "errmsg.h"
#include "mysqld_error.h"
#include <stdarg.h>
#include "MemoryDebug.h"

#ifdef WIN32
#pragma comment( lib, "mysqlclient" )
#endif

CDbMySQLConnection::CDbMySQLConnection() : m_bConnected(false), m_psttStmt(NULL), m_psttBind(NULL), m_iBindCount(0), m_iReadTimeout(0), m_iWriteTimeout(0), m_eLogLevel(LOG_SQL)
{
}

CDbMySQLConnection::~CDbMySQLConnection()
{
}

/**
 * @ingroup DbMySQLPool
 * @brief DB �� �����Ѵ�.
 * @param pszHost			MySQL ȣ��Ʈ �̸� or IP �ּ�
 * @param pszUserId		MySQL ���� ���̵�
 * @param pszPassWord MySQL ���� ���̵��� ��й�ȣ
 * @param pszDbName		MySQL ���� �����ͺ��̽� �̸�
 * @param iPort				MySQL ���� ��Ʈ ��ȣ
 * @param pszCharacterSet	character set
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Connect( const char * pszHost, const char * pszUserId, const char * pszPassWord, const char * pszDbName, int iPort, const char * pszCharacterSet )
{
	m_strDbHost = pszHost;
	m_strDbUserId = pszUserId;
	m_strDbPassWord = pszPassWord;
	m_strDbName = pszDbName;
	m_iPort = iPort;

	if( pszCharacterSet )
	{
		m_strCharacterSet = pszCharacterSet;
	}

	return Connect();
}

/**
 * @ingroup DbMySQLPool
 * @brief DB ���� �����Ѵ�.
 */
void CDbMySQLConnection::Close( )
{
	if( m_bConnected == false ) return;

	PrepareClose();

	mysql_close( &m_sttMySQL );
	m_bConnected = false;
}

/**
 * @ingroup DbMySQLPool
 * @brief INSERT / UPDATE / DELETE SQL ���� �����Ѵ�.
 * @param pszSQL SQL ��
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Execute( const char * pszSQL )
{
	return Query( pszSQL );
}

/**
 * @ingroup DbMySQLPool
 * @brief INSERT ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param piId		AUTO_INCREMENT �� ������ ���� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Insert( const char * pszSQL, uint64_t * piId )
{
	if( Query( pszSQL ) == false ) return false;

	if( piId )
	{
		*piId = mysql_insert_id( &m_sttMySQL );
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT ���� �����Ѵ�.
 * @param pszSQL		SQL ��
 * @param pclsData	���� ���α׷� ����
 * @param FetchRow	1���� Row ���� ����Ǵ� �Լ�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, MYSQL_ROW & sttRow ) )
{
	if( Query( pszSQL ) == false ) return false;

	MYSQL_RES * psttRes = mysql_use_result( &m_sttMySQL );
	if( psttRes == NULL )
	{
		CLog::Print( LOG_ERROR, "mysql_use_result - %.2048s - error(%u) - %s", pszSQL, mysql_errno( &m_sttMySQL ), mysql_error( &m_sttMySQL ) );
		return false;
	}

	MYSQL_ROW sttRow;
	
	while( ( sttRow = mysql_fetch_row( psttRes ) ) )
	{
		if( FetchRow( pclsData, sttRow ) == false ) break;
	}

	mysql_free_result( psttRes );

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::QueryOne( const char * pszSQL, int & iData )
{
	std::string strData;

	iData = 0;
	if( QueryOne( pszSQL, strData ) == false ) return false;

	iData = atoi( strData.c_str() );

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param cData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::QueryOne( const char * pszSQL, uint8_t & cData )
{
	std::string strData;

	cData = 0;
	if( QueryOne( pszSQL, strData ) == false ) return false;

	cData = atoi( strData.c_str() );

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::QueryOne( const char * pszSQL, uint32_t & iData )
{
	std::string strData;

	iData = 0;
	if( QueryOne( pszSQL, strData ) == false ) return false;

	iData = GetUInt32( strData.c_str() );

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::QueryOne( const char * pszSQL, uint64_t & iData )
{
	std::string strData;

	iData = 0;
	if( QueryOne( pszSQL, strData ) == false ) return false;

	iData = GetUInt64( strData.c_str() );

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief SELECT count(*) �� ���� 1���� row, column �� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param iData		�˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::QueryOne( const char * pszSQL, int64_t & iData )
{
	std::string strData;

	iData = 0;
	if( QueryOne( pszSQL, strData ) == false ) return false;

	iData = atoll( strData.c_str() );

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief �� ���� row, column �� �������� SQL ���� �����Ѵ�.
 * @param pszSQL	SQL ��
 * @param strData �˻� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::QueryOne( const char * pszSQL, std::string & strData )
{
	strData.clear();

	if( Query( pszSQL ) == false ) return false;

	MYSQL_RES * psttRes = mysql_use_result( &m_sttMySQL );
	if( psttRes == NULL )
	{
		CLog::Print( LOG_ERROR, "mysql_use_result - %.2048s - %s", pszSQL, mysql_error( &m_sttMySQL ) );
		return false;
	}

	MYSQL_ROW sttRow;
	bool bRes = false;
	
	if( ( sttRow = mysql_fetch_row( psttRes ) ) )
	{
		if( sttRow[0] )
		{
			strData = sttRow[0];
			bRes = true;
		}
	}

	mysql_free_result( psttRes );

	return bRes;
}


/**
 * @ingroup DbMySQLPool
 * @brief SELECT password(?) �� ���� SQL ���� ������ ����� �����´�.
 * @param pszSQL		SQL ��
 * @param pszArg		bind �� ���ڿ�
 * @param strData		����� ������ ����
 * @param iDataSize ��� ���ڿ��� �ִ� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::QueryOne( const char * pszSQL, const char * pszArg, std::string & strData, int iDataSize )
{
	if( Prepare( pszSQL ) == false || Bind( 0, pszArg ) == false || PrepareExecute( ) == false )
	{
		PrepareClose();
		return false;
	}

	MYSQL_BIND    sttBind;

//#if _MSC_VER == VC2008_VERSION
	my_bool				bError = 0, bNull = 0;
//#else
//	bool					bError = false, bNull = false;
//#endif

	unsigned long iLength;
	char	* pszData = NULL;

	pszData = (char *)malloc( iDataSize );
	if( pszData == NULL )
	{
		PrepareClose();
		return false;
	}

	memset( pszData, 0, iDataSize );
	memset( &sttBind, 0, sizeof(sttBind) );

	sttBind.buffer_type= MYSQL_TYPE_STRING;
	sttBind.buffer = pszData;
	sttBind.buffer_length = iDataSize - 1;
	sttBind.is_null = &bError;
	sttBind.length = &iLength;
	sttBind.error = &bError;

	if( mysql_stmt_bind_result( m_psttStmt, &sttBind ) )
	{
		PrepareClose();
		free( pszData );
		return false;
	}

	if( !mysql_stmt_fetch( m_psttStmt ) )
	{
		if( !bNull )
		{
			strData = pszData;
		}
	}
	
	PrepareClose();
	free( pszData );

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief DB SQL ���� �����Ѵ�. DB ���� ������ �߻��ϸ� DB �� �翬���� ��, SQL ���� �����Ѵ�.
 * @param pszSQL SQL ��
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Query( const char * pszSQL )
{
	char cTryCount = 0;
	bool bRes = false;

	if( m_bConnected == false )
	{
		if( Connect( ) == false )
		{
			CLog::Print( LOG_ERROR, "%.2048s - DB connect error", pszSQL );
			return false;
		}
	}

	for( int i = 0; i < 2; ++i )
	{
		if( mysql_query( &m_sttMySQL, pszSQL ) != 0 )
		{
			++cTryCount;
			unsigned int iErrorNo = mysql_errno( &m_sttMySQL );

			if( cTryCount == 1 && ( iErrorNo == CR_SERVER_GONE_ERROR || iErrorNo == CR_SERVER_LOST ) )
			{
				Close( );
				if( Connect( ) == false )
				{
					CLog::Print( LOG_ERROR, "%.2048s - db connection is closed and db connection error - %s", pszSQL, mysql_error( &m_sttMySQL ) );
					break;
				}
			}
			else
			{
				if( iErrorNo == ER_NO_SUCH_TABLE || iErrorNo == ER_BAD_TABLE_ERROR )
				{
					CLog::Print( m_eLogLevel, "%.2048s - error(%u) - %s", pszSQL, iErrorNo, mysql_error( &m_sttMySQL ) );
				}
				else
				{
					CLog::Print( LOG_ERROR, "%.2048s - error(%u) - %s", pszSQL, iErrorNo, mysql_error( &m_sttMySQL ) );
				}
				break;
			}
		}
		else
		{
			CLog::Print( m_eLogLevel, "%.2048s", pszSQL );
			bRes = true;
			break;
		}
	}

	return bRes;
}

/**
 * @ingroup DbMySQLPool
 * @brief �̹� ������ DB ���� ������ DB �� �����Ѵ�.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Connect( )
{
	if( m_strDbHost.empty() ) return false;

	if( mysql_init( &m_sttMySQL ) == NULL )
	{
		CLog::Print( LOG_ERROR, "%s mysql_init error", __FUNCTION__ );
		return false;
	}

	if( m_strPluginDir.empty() == false )
	{
		mysql_options( &m_sttMySQL, MYSQL_PLUGIN_DIR, m_strPluginDir.c_str() );
	}
	
	if( m_iReadTimeout > 0 )
	{
		mysql_options( &m_sttMySQL, MYSQL_OPT_READ_TIMEOUT, (char *)&m_iReadTimeout );
	}

	if( m_iWriteTimeout > 0 )
	{
		mysql_options( &m_sttMySQL, MYSQL_OPT_WRITE_TIMEOUT, (char *)&m_iWriteTimeout );
	}
	
	if( mysql_real_connect( &m_sttMySQL, m_strDbHost.c_str(), m_strDbUserId.c_str(), m_strDbPassWord.c_str(), m_strDbName.c_str(), m_iPort, NULL, 0 ) == NULL )
	{
		CLog::Print( LOG_ERROR, "%s mysql_real_connect error - %s", __FUNCTION__, mysql_error( &m_sttMySQL ) );
		mysql_close( &m_sttMySQL );
		return false;
	}

	CLog::Print( LOG_INFO, "%s mysql_real_connect success", __FUNCTION__ );
	m_bConnected = true;

	if( m_strCharacterSet.empty() == false )
	{
		if( mysql_set_character_set( &m_sttMySQL, m_strCharacterSet.c_str() ) != 0 )
		{
			CLog::Print( LOG_ERROR, "%s mysql_set_character_set(%s) error", __FUNCTION__, m_strCharacterSet.c_str() );
		}
	}
	else
	{
		CLog::Print( LOG_DEBUG, "%s mysql default character_set is (%s)", __FUNCTION__, mysql_character_set_name( &m_sttMySQL ) );
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief prepareStmt �� �����Ѵ�.
 * @param pszSQL SQL ��
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Prepare( const char * pszSQL )
{
	if( m_bConnected == false )
	{
		if( Connect( ) == false )
		{
			CLog::Print( LOG_ERROR, "%.2048s - DB connect error", pszSQL );
			return false;
		}
	}

	PrepareClose();

	for( int i = 0; i < 2; ++i )
	{
		m_psttStmt = mysql_stmt_init( &m_sttMySQL );
		if( m_psttStmt == NULL )
		{
			CLog::Print( LOG_ERROR, "%.2048s - mysql_stmt_init error - %s", pszSQL, mysql_error( &m_sttMySQL ) );
			return false;
		}

		if( mysql_stmt_prepare( m_psttStmt, pszSQL, (int)strlen(pszSQL) ) != 0 )
		{
			unsigned int iErrorNo = mysql_errno( &m_sttMySQL );

			if( i == 0 && ( iErrorNo == CR_SERVER_GONE_ERROR || iErrorNo == CR_SERVER_LOST ) )
			{
				Close( );
				if( Connect( ) == false )
				{
					CLog::Print( LOG_ERROR, "%.2048s - db connection is closed and db connection error - %s", pszSQL, mysql_error( &m_sttMySQL ) );
					return false;
				}

				PrepareClose();
			}
			else
			{
				CLog::Print( LOG_ERROR, "%.2048s - mysql_stmt_prepare error - %s", pszSQL, mysql_stmt_error( m_psttStmt ) );
				return false;
			}
		}
		else
		{
			CLog::Print( m_eLogLevel, "%.2048s", pszSQL );
			break;
		}
	}

	m_iBindCount = mysql_stmt_param_count( m_psttStmt );
	if( m_iBindCount > 0 )
	{
		m_psttBind = (MYSQL_BIND *)malloc( sizeof(MYSQL_BIND) * m_iBindCount );
		if( m_psttBind == NULL )
		{
			CLog::Print( LOG_ERROR, "%.2048s - bind malloc error", pszSQL );
			PrepareClose();
			return false;
		}

		memset( m_psttBind, 0, sizeof(MYSQL_BIND) * m_iBindCount );

		for( int i = 0; i < m_iBindCount; ++i )
		{
			m_clsBindList.push_back( "" );
		}
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief prepareStmt �� bind �� ������ �����Ѵ�.
 * @param iIndex bind ���� �ε���
 * @param pszVal ��
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Bind( int iIndex, const char * pszVal )
{
	if( m_psttStmt == NULL )
	{
		CLog::Print( LOG_ERROR, "Bind error - not called Prepare function" );
		return false;
	}

	if( iIndex >= m_iBindCount || iIndex < 0 )
	{
		CLog::Print( LOG_ERROR, "Bind error - index(%d) is not correct - bind count(%d)", iIndex, m_iBindCount );
		return false;
	}

	m_clsBindList[iIndex] = pszVal;

	m_psttBind[iIndex].buffer_type = MYSQL_TYPE_STRING;
	m_psttBind[iIndex].buffer = (void *)m_clsBindList[iIndex].c_str();
	m_psttBind[iIndex].buffer_length = (int)m_clsBindList[iIndex].length();

	/*
	m_psttBind[iIndex].buffer_length = STRING_SIZE;
	m_psttBind[iIndex].is_null = 0;
	m_psttBind[iIndex].length= NULL;
	*/

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief prepareStmt �� �����Ѵ�.
 * @param piId AUTO_INCREMENT �� ������ ���� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::PrepareExecute( uint64_t * piId )
{
	if( m_psttStmt == NULL )
	{
		CLog::Print( LOG_ERROR, "Bind error - not called Prepare function" );
		return false;
	}

	if( mysql_stmt_bind_param( m_psttStmt, m_psttBind ) != 0 )
	{
		CLog::Print( LOG_ERROR, "mysql_stmt_bind_param error - %s", mysql_stmt_error( m_psttStmt ) );
		return false;
	}

	if( mysql_stmt_execute( m_psttStmt ) != 0 )
	{
		CLog::Print( LOG_ERROR, "mysql_stmt_execute error - %s", mysql_stmt_error( m_psttStmt ) );
		return false;
	}

	if( piId )
	{
		*piId = mysql_stmt_insert_id( m_psttStmt );
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief prepareStmt �� �����Ѵ�.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::PrepareClose( )
{
	if( m_psttBind )
	{
		free( m_psttBind );
		m_psttBind = NULL;
	}

	if( m_psttStmt )
	{
		mysql_stmt_close( m_psttStmt );
		m_psttStmt = NULL;
	}

	m_iBindCount = 0;
	m_clsBindList.clear();

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief prepare statement �� SQL INSERT, UPDATE, DELETE ����� �����Ѵ�.
 * @param pszSQL		���� SQL ��
 * @param iArgCount Bind ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Execute( const char * pszSQL, int iArgCount, ... )
{
	bool bRes = false, bError = false;
	va_list pArgList;

	if( Prepare( pszSQL ) == false ) return false;

	va_start( pArgList, iArgCount );

	for( int i = 0; i < iArgCount; ++i )
	{
		char * pszArg = va_arg( pArgList, char * );
		if( pszArg == NULL )
		{
			CLog::Print( LOG_ERROR, "%s arg(%d) is NULL", __FUNCTION__, i );
			bError = true;
			break;
		}

		if( Bind( i, pszArg ) == false )
		{
			bError = true;
			break;
		}
	}

	va_end( pArgList );

	if( bError )
	{
		PrepareClose( );
		return false;
	}

	if( PrepareExecute( ) ) 
	{
		bRes = true;
	}

	PrepareClose( );

	return bRes;
}

/**
 * @ingroup DbMySQLPool
 * @brief prepare statement �� SQL INSERT, UPDATE, DELETE ����� �����Ѵ�.
 * @param pszSQL			���� SQL ��
 * @param clsArgList	Bind ���� ����Ʈ
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDbMySQLConnection::Execute( const char * pszSQL, STRING_LIST & clsArgList )
{
	bool bRes = false, bError = false;
	STRING_LIST::iterator itList;
	int iIndex = 0;

	if( Prepare( pszSQL ) == false ) return false;

	for( itList = clsArgList.begin(); itList != clsArgList.end(); ++itList )
	{
		if( Bind( iIndex, itList->c_str() ) == false )
		{
			bError = true;
			break;
		}

		++iIndex;
	}

	if( bError )
	{
		PrepareClose( );
		return false;
	}

	if( PrepareExecute( ) ) 
	{
		bRes = true;
	}

	PrepareClose( );

	return bRes;
}

/**
 * @ingroup DbMySQLPool
 * @brief It returns the number of rows changed, deleted, or inserted by the last statement if it was an UPDATE, DELETE, or INSERT.
 * @returns An integer greater than zero indicates the number of rows affected or retrieved. Zero indicates that no records were updated for an UPDATE statement, no rows matched the WHERE clause in the query or that no query has yet been executed. -1 indicates that the query returned an error 
 */
uint64_t CDbMySQLConnection::GetAffectedRow( )
{
	if( m_bConnected == false )
	{
		return 0;
	}

	return mysql_affected_rows( &m_sttMySQL );
}

/**
 * @ingroup DbMySQLPool
 * @brief mysql_errno �� �����Ѵ�.
 * @returns mysql_errno �� �����Ѵ�.
 */
uint32_t CDbMySQLConnection::GetError()
{
	return mysql_errno( &m_sttMySQL );
}

/**
 * @ingroup DbMySQLPool
 * @brief ���̺��� �����ϴ��� �˻��Ѵ�.
 * @param pszTableName ���̺� �̸�
 * @returns ���̺��� �����ϸ� true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CDbMySQLConnection::IsExistTable( const char * pszTableName )
{
	char szSQL[1048];
	int iCount;

	snprintf( szSQL, sizeof(szSQL), "select count(*) from information_schema.tables where table_schema = '%s' "
		"and table_name = '%s'"
		, m_strDbName.c_str(), pszTableName );
	if( QueryOne( szSQL, iCount ) == false ) return false;

	if( iCount == 1 ) return true;

	return false;
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL read timeout �ð��� �����Ѵ�. �� �޼ҵ�� Connect() �޼ҵ带 ȣ���ϱ� ���� ȣ���ؾ� ��ȿ�ϴ�.
 * @param iSecond MySQL read timeout �ð� (�ʴ���)
 */
void CDbMySQLConnection::SetReadTimeout( int iSecond )
{
	if( iSecond > 0 )
	{
		m_iReadTimeout = iSecond;
	}
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL write timeout �ð��� �����Ѵ�. �� �޼ҵ�� Connect() �޼ҵ带 ȣ���ϱ� ���� ȣ���ؾ� ��ȿ�ϴ�.
 * @param iSecond MySQL write timeout �ð� (�ʴ���)
 */
void CDbMySQLConnection::SetWriteTimeout( int iSecond )
{
	if( iSecond > 0 )
	{
		m_iWriteTimeout = iSecond;
	}
}

void CDbMySQLConnection::SetPluginDir( const char * pszPluginDir )
{
	m_strPluginDir = pszPluginDir;
}

/**
 * @ingroup DbMySQLPool
 * @brief SQL �α� ������ �����Ѵ�.
 * @param eLogLevel SQL �α� ���� ( LOG_SQL, LOG_SQL1, LOG_SQL2, LOG_SQL3 �߿��� 1���� �Է��Ѵ�. )
 */
void CDbMySQLConnection::SetLogLevel( EnumLogLevel eLogLevel )
{
	m_eLogLevel = eLogLevel;
}

/**
 * @ingroup DbMySQLPool
 * @brief MySQL ���̺귯�� ����� ������ ���� ȣ���Ͽ��� �޸𸮸� �����Ѵ�.
 */
void CDbMySQLConnection::Final( )
{
	mysql_library_end();
}
