#include "stdafx.h"
#include "CQuery.h"
#include "Window.h"

// Class Constructor
CQuery::CQuery()
{
	AffectCount=-1;
	ret=SQL_SUCCESS;

	SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&hEnv);
	SQLSetEnvAttr(hEnv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);
}

// Class Destructor
CQuery::~CQuery()
{
	Clear();
	if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if (hDbc) SQLDisconnect(hDbc);
	if (hDbc) SQLFreeHandle(SQL_HANDLE_DBC,hDbc);
	if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
}

BOOL CQuery::Connect(int Type, char *ConStr, char *UID, char *PWD)
{
	SQLCHAR InCon[255];
	SQLCHAR OutCon[255];
    SQLSMALLINT cbOutCon;

	int ii=1;
	SQLRETURN Ret;
	SQLINTEGER NativeError;
	SQLCHAR SqlState[6], Msg[255];
	SQLSMALLINT MsgLen;
	char str[256];

	m_Type = Type;
	strcpy(m_szConnect, ConStr);
	strcpy(m_Id, UID);
	strcpy(m_Pass, PWD);

	SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hDbc);
	switch (Type) {
	case 1:
		wsprintf((char *)InCon,"DRIVER={Microsoft Access Driver (*.mdb)};DBQ=%s;",ConStr);
		ret=SQLDriverConnect(hDbc,NULL,(SQLCHAR *)InCon,sizeof(InCon),OutCon,
			sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case 2:
		wsprintf((char *)InCon, "FileDsn=%s",ConStr);
		ret=SQLDriverConnect(hDbc,NULL,(SQLCHAR *)InCon,sizeof(InCon),OutCon,
			sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case 3:
		ret=SQLConnect(hDbc,(SQLCHAR *)ConStr,SQL_NTS,(SQLCHAR *)UID,SQL_NTS,
			(SQLCHAR *)PWD,SQL_NTS);
		break;
	}

	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) {
		while (Ret=SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, ii, SqlState, &NativeError, 
			Msg, sizeof(Msg), &MsgLen)!=SQL_NO_DATA) {
			wsprintf(str, "SQLSTATE:%s, 진단정보:%s",(LPCTSTR)SqlState,(LPCTSTR)Msg);
			//::MessageBox(NULL,str,"SQL Error",0);
			g_Window.ServerLogAdd(S_TYPE,str);
			ii++;
		}
		return FALSE;
	}

	ret=SQLAllocHandle(SQL_HANDLE_STMT,hDbc,&hStmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) {
		hStmt=0;
		return FALSE;
	}
	return TRUE;
}

BOOL CQuery::ReConnect()
{
	return Connect(m_Type, m_szConnect, m_Id, m_Pass);
}

#include "Server.h"

BOOL CQuery::Exec(LPCTSTR szSQL)
{
	int c;

	if( S_TYPE != ST_CASHSHOPSERVER )
	{
		g_Window.ServerLogAdd(S_TYPE,"%s", szSQL);
	}

	ret=SQLExecDirect(hStmt,(SQLCHAR *)szSQL,SQL_NTS);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA)) {
		PrintDiag();
		return FALSE;
	}

	SQLRowCount(hStmt,&AffectCount);
	SQLNumResultCols(hStmt,&nCol);
	if (nCol > MAXCOL) {
		//::MessageBox(NULL,"nCol > MAXCOL","CQuery error",MB_OK);
		g_Window.ServerLogAdd(S_TYPE,"CQuery error : nCol > MAXCOL");
		return FALSE;
	}

	if (nCol == 0) {
		Clear();
		return TRUE;
	}

	for (c=0;c<nCol;c++) {
		SQLBindCol(hStmt,c+1,SQL_C_CHAR,Col[c],255,&lCol[c]);
		SQLDescribeCol(hStmt,c+1,ColName[c],30,NULL,NULL,NULL,NULL,NULL);
	}
	return TRUE;
}

int CQuery::ExecGetInt(LPCTSTR szSQL)
{
	int i;

	//LogAdd("%s", szSQL);

	if (Exec(szSQL) == FALSE) 
		return CQUERYERROR;

	if (Fetch()==SQL_NO_DATA) {
		Clear();
		return CQUERYEOF;
	}
	i=GetInt(1);
	Clear();
	return i;
}

BOOL CQuery::ExecGetStr(LPCTSTR szSQL, char *buf)
{
//	LogAdd("%s", szSQL);

	if (Exec(szSQL) == FALSE) {
		//strcpy(buf, "CQUERYERROR");
		buf[0] = '\0';
		return FALSE;
	}

	if (Fetch()==SQL_NO_DATA) {
		Clear();
		//strcpy(buf,"EOF");
		buf[0] = '\0';
		return FALSE;
	}
	GetStr(1,buf);
	Clear();
	return TRUE;
}

SQLRETURN CQuery::Fetch()
{
	ret=SQLFetch(hStmt);
	return ret;
}

void CQuery::Clear()
{
	//SQLCloseCursor(hStmt);
	SQLFreeStmt(hStmt, SQL_CLOSE);
	SQLFreeStmt(hStmt, SQL_UNBIND);
}

int CQuery::FindCol(char *name)
{
	int i;
	for (i=0;i<nCol;i++) {
		if (stricmp(name,(LPCTSTR)ColName[i])==0)
			return i+1;
	}
	return -1;
}

int CQuery::GetInt(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA) {
		return CQUERYNULL;
	} else {
		return atoi(Col[nCol-1]);
	}
}

int CQuery::GetInt(char *sCol)
{
	int n;
	n=FindCol(sCol);
	if (n==-1) {
		return CQUERYNOCOL;
	} else {
		return GetInt(n);
	}
}

__int64 CQuery::GetInt64(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA) {
		return CQUERYNULL;
	} else {
		return _atoi64(Col[nCol-1]);
	}
}

__int64 CQuery::GetInt64(char *sCol)
{
	int n;
	n=FindCol(sCol);
	if (n==-1) {
		return CQUERYNOCOL;
	} else {
		return GetInt64(n);
	}
}

float CQuery::GetFloat(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA) {
		return CQUERYNULL;
	} else {
		return (float)atof(Col[nCol-1]);
	}
}

float CQuery::GetFloat(char *sCol)
{
	int n;
	n=FindCol(sCol);
	if (n==-1) {
		return CQUERYNOCOL;
	} else {
		return GetFloat(n);
	}
}


void CQuery::GetStr(int nCol, char *buf)
{
	if (nCol > nCol) {
		//strcpy(buf, "CQUERYNOCOL");
		buf[0] = '\0';
		return;
	}

	if (lCol[nCol-1]==SQL_NULL_DATA) 
	{
		buf[0] = '\0';
		//lstrcpy(buf,"NULL");
	} else {
		lstrcpy(buf,Col[nCol-1]);
	}
}

void CQuery::GetStr(char *sCol, char *buf)
{
	int n;
	n=FindCol(sCol);
	if (n==-1) {
		//lstrcpy(buf,"ERROR:Colume not found");
		buf[0] = '\0';
	} else {
		GetStr(n, buf);
	}
}

void CQuery::PrintDiag()
{
	int ii;
	SQLRETURN Ret;
	SQLINTEGER NativeError;
	SQLCHAR SqlState[6], Msg[255];
	SQLSMALLINT MsgLen;
	char str[256];

	ii=1;
	while (Ret=SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, ii, SqlState, &NativeError, 
		Msg, sizeof(Msg), &MsgLen)!=SQL_NO_DATA) {
		wsprintf(str, "SQLSTATE:%s, SQLSTRING:%s",(LPCTSTR)SqlState,(LPCTSTR)Msg);
		//::MessageBox(NULL,str,"진단 정보",0);
		g_Window.ServerLogAdd(S_TYPE,str);
		//LogAdd(str);
		ii++;
	}

	if( strcmp((LPCTSTR)SqlState, "08S01") == 0 )
	{
		ReConnect();
	}
}

int CQuery::ReadBlob(LPCTSTR szSQL, void *buf)
{
	SQLCHAR BinaryPtr[BLOBBATCH];
	SQLINTEGER LenBin;
	char *p;
	int nGet;
	int TotalGet=0;

	//LogAdd("%s", szSQL);

	ret=SQLExecDirect(hStmt,(SQLCHAR *)szSQL,SQL_NTS);
	if (ret!=SQL_SUCCESS) {
		PrintDiag();
		return -1;
	}

	while ((ret=SQLFetch(hStmt)) != SQL_NO_DATA) {
		p=(char *)buf;
		while ((ret=SQLGetData(hStmt,1,SQL_C_BINARY,BinaryPtr,sizeof(BinaryPtr),
			&LenBin))!=SQL_NO_DATA) {
			if (LenBin==SQL_NULL_DATA) {
				Clear();
				return 0;
			}
			if (ret==SQL_SUCCESS)
				nGet=LenBin;
			else
				nGet=BLOBBATCH;
			TotalGet+=nGet;
			memcpy(p,BinaryPtr,nGet);
			p+=nGet;
		}
	}
	Clear();
	return TotalGet;
}

void CQuery::WriteBlob(LPCTSTR szSQL, void *buf, int size)
{
	SQLINTEGER cbBlob;
	char tmp[BLOBBATCH],*p;
	SQLPOINTER pToken;
	int nPut;

	//LogAdd("%s", szSQL);

	cbBlob=SQL_LEN_DATA_AT_EXEC(size);
	SQLBindParameter(hStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_LONGVARBINARY,
		size,0,(SQLPOINTER)1,0,&cbBlob);
	SQLExecDirect(hStmt,(SQLCHAR *)szSQL,SQL_NTS);
	ret=SQLParamData(hStmt, &pToken);
	while (ret==SQL_NEED_DATA) {
		if (ret==SQL_NEED_DATA) {
			if ((int)pToken==1) {
				for (p=(char *)buf;p<(char *)buf+size;p+=BLOBBATCH) {
					nPut=min(BLOBBATCH,(char *)buf+size-p);
					memcpy(tmp,p,nPut);
					SQLPutData(hStmt,(PTR)tmp,nPut);
				}
			}
		}
		ret=SQLParamData(hStmt, &pToken);
	}
	Clear();
}

int CQuery::BindParameterBinaryOutput(int nCol, BYTE *nValue, int iSize, long *lLength)
{
	return SQLBindParameter(hStmt, nCol, SQL_PARAM_OUTPUT, SQL_C_BINARY, SQL_VARBINARY, iSize, 0, nValue, iSize, lLength);
}

int CQuery::BindParameterIntOutput(int nCol, int* nValue, long *lLength)
{
	return SQLBindParameter(hStmt, nCol, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, nValue, sizeof(int), lLength);
}

int CQuery::MoreResults()
{
	return SQLMoreResults(hStmt);
}