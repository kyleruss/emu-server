#ifndef __CQUERY_H__
#define __CQUERY_H__

class CQuery
{
public:
	enum { MAXCOL=100, BLOBBATCH=10000, CQUERYNULL=-100, CQUERYEOF=-101, 
		CQUERYNOCOL=-102, CQUERYERROR=-103 };
private:
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLRETURN ret;
	char Col[MAXCOL][255];
	
	int	m_Type;
	char m_szConnect[128];
	char m_Id[20];
	char m_Pass[20];

	int FindCol(char *name);
	BOOL ReConnect();
public:
	SQLINTEGER AffectCount;
	SQLHSTMT hStmt;
	SQLSMALLINT nCol;
	SQLCHAR ColName[MAXCOL][50];
	SQLINTEGER lCol[MAXCOL];

	DWORD S_TYPE;

	void PrintDiag();
	CQuery();
	~CQuery();
	BOOL Connect(int Type, char *ConStr, char *UID=NULL, char *PWD=NULL);
	BOOL Exec(LPCTSTR szSQL);
	int ExecGetInt(LPCTSTR szSQL);
	BOOL ExecGetStr(LPCTSTR szSQL, char *buf);
	SQLRETURN Fetch();
	void Clear();
	int GetInt(int nCol);
	int GetInt(char *sCol);
	__int64 GetInt64(int nCol);
	__int64 GetInt64(char* sCol);
	float GetFloat(int nCol);
	float GetFloat(char *sCol);
	void GetStr(int nCol, char *buf);
	void GetStr(char *sCol, char *buf);
	int ReadBlob(LPCTSTR szSQL, void *buf);
	void WriteBlob(LPCTSTR szSQL, void *buf, int size);
	int BindParameterBinaryOutput(int nCol, BYTE *nValue, int iSize, long *lLength);
	int BindParameterIntOutput(int nCol, int *nValue, long *lLength);
	int MoreResults();
};


#endif