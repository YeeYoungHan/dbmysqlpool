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

#include "DbCheckSqlInjection.h"
#include "StringUtility.h"
#include "Log.h"

/**
 * @ingroup DbMySQLPool
 * @brief SQL injection 공격 문자열이 포함되어 있는지 검사한다.
 * @param strValue 검사 문자열
 * @returns SQL injection 공격 문자열이 포함되어 있으면 false 를 리턴하고 그렇지 않으면 true 를 리턴한다.
 */
bool DbCheckSqlInjection( std::string & strValue )
{
	static const char * pszTest = "'\"/\\;: +-";

	for( int i = 0; pszTest[i]; ++i )
	{
		if( SearchString( strValue, pszTest[i] ) )
		{
			CLog::Print( LOG_ERROR, "%s value(%s) contain %c", __FUNCTION__, strValue.c_str(), pszTest[i] );
			return false;
		}
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief / 를 제외한 SQL injection 공격 문자열이 포함되어 있는지 검사한다.
 * @param strValue 검사 문자열
 * @returns SQL injection 공격 문자열이 포함되어 있으면 false 를 리턴하고 그렇지 않으면 true 를 리턴한다.
 */
bool DbCheckSqlInjectionNotSlash( std::string & strValue )
{
	static const char * pszTest = "'\"\\;: +-";

	for( int i = 0; pszTest[i]; ++i )
	{
		if( SearchString( strValue, pszTest[i] ) )
		{
			CLog::Print( LOG_ERROR, "%s value(%s) contain %c", __FUNCTION__, strValue.c_str(), pszTest[i] );
			return false;
		}
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief 공백을 제외한 SQL injection 공격 문자열이 포함되어 있는지 검사한다.
 * @param strValue 검사 문자열
 * @returns SQL injection 공격 문자열이 포함되어 있으면 false 를 리턴하고 그렇지 않으면 true 를 리턴한다.
 */
bool DbCheckSqlInjectionNotSpace( std::string & strValue )
{
	static const char * pszTest = "'\"/\\;:+-";

	for( int i = 0; pszTest[i]; ++i )
	{
		if( SearchString( strValue, pszTest[i] ) )
		{
			CLog::Print( LOG_ERROR, "%s value(%s) contain %c", __FUNCTION__, strValue.c_str(), pszTest[i] );
			return false;
		}
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief '-' 를 제외한 SQL injection 공격 문자열이 포함되어 있는지 검사한다.
 * @param strValue 검사 문자열
 * @returns SQL injection 공격 문자열이 포함되어 있으면 false 를 리턴하고 그렇지 않으면 true 를 리턴한다.
 */
bool DbCheckSqlInjectionNotMinus( std::string & strValue )
{
	static const char * pszTest = "'\"/\\;: +";

	for( int i = 0; pszTest[i]; ++i )
	{
		if( SearchString( strValue, pszTest[i] ) )
		{
			CLog::Print( LOG_ERROR, "%s value(%s) contain %c", __FUNCTION__, strValue.c_str(), pszTest[i] );
			return false;
		}
	}

	return true;
}

/**
 * @ingroup DbMySQLPool
 * @brief 공백을 제외한 SQL injection 공격 문자열이 존재하면 공백으로 수정한다.
 * @param strValue 검사 문자열
 */
void DbReplaceSqlInjectionNotSpace( std::string & strValue )
{
	static const char * pszTest = "'\"/\\;:+-";

	if( strValue.length() == 0 ) return;

	for( int i = 0; pszTest[i]; ++i )
	{
		ReplaceString( strValue, pszTest[i], ' ' );
	}
}
