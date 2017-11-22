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

#include "DbSetData.h"
#include "MemoryDebug.h"

/**
 * @ingroup DbPool
 * @brief DB ���� fetch �� row �� �ϳ��� �÷����� �����Ѵ�.
 * @param sttRow	MYSQL_ROW
 * @param iCol		�÷� �ε���. �Լ� ȣ����, �÷� �ε����� 1 �����Ѵ�.
 * @param strData �÷��� ���� ����
 */
void DbSetData( MYSQL_ROW & sttRow, int & iCol, std::string & strData )
{
	if( sttRow[iCol] )
	{
		strData = sttRow[iCol];
	}

	++iCol;
}

/**
 * @ingroup DbPool
 * @brief DB ���� fetch �� row �� �ϳ��� �÷����� �����Ѵ�.
 * @param sttRow	MYSQL_ROW
 * @param iCol		�÷� �ε���. �Լ� ȣ����, �÷� �ε����� 1 �����Ѵ�.
 * @param iData		�÷��� ���� ����
 */
void DbSetData( MYSQL_ROW & sttRow, int & iCol, int & iData )
{
	if( sttRow[iCol] )
	{
		iData = atoi(sttRow[iCol]);
	}

	++iCol;
}

/**
 * @ingroup DbPool
 * @brief DB ���� fetch �� row �� �ϳ��� �÷����� �����Ѵ�.
 * @param sttRow	MYSQL_ROW
 * @param iCol		�÷� �ε���. �Լ� ȣ����, �÷� �ε����� 1 �����Ѵ�.
 * @param iData		�÷��� ���� ����
 */
void DbSetData( MYSQL_ROW & sttRow, int & iCol, uint32_t & iData )
{
	if( sttRow[iCol] )
	{
		iData = GetUInt32(sttRow[iCol]);
	}

	++iCol;
}

/**
 * @ingroup DbPool
 * @brief DB ���� fetch �� row �� �ϳ��� �÷����� �����Ѵ�.
 * @param sttRow	MYSQL_ROW
 * @param iCol		�÷� �ε���. �Լ� ȣ����, �÷� �ε����� 1 �����Ѵ�.
 * @param iData		�÷��� ���� ����
 */
void DbSetData( MYSQL_ROW & sttRow, int & iCol, int64_t & iData )
{
	if( sttRow[iCol] )
	{
		iData = atoll(sttRow[iCol]);
	}

	++iCol;
}

/**
 * @ingroup DbPool
 * @brief DB ���� fetch �� row �� �ϳ��� �÷����� �����Ѵ�.
 * @param sttRow	MYSQL_ROW
 * @param iCol		�÷� �ε���. �Լ� ȣ����, �÷� �ε����� 1 �����Ѵ�.
 * @param iData		�÷��� ���� ����
 */
void DbSetData( MYSQL_ROW & sttRow, int & iCol, uint64_t & iData )
{
	if( sttRow[iCol] )
	{
		iData = GetUInt64(sttRow[iCol]);
	}

	++iCol;
}
