#include "stdafx.h"
#include "winutil.h"

#define MAX_BUXCCODE				3

static BYTE	bBuxCode[MAX_BUXCCODE] = {0xFC, 0xCF, 0xAB};

void BuxConvert(char *buf, int size)
{
	for( int n=0; n<size; n++) buf[n] ^= (char)bBuxCode[n%MAX_BUXCCODE];
}

BOOL SQLSyntexCheck(char *SQLString)
{
	char *temp;
	temp = strchr( SQLString, '\'');
	if( temp == NULL ) 
	{
		temp = strchr( SQLString, 0x20);
		if( temp == NULL ) return TRUE;
		return FALSE;
	}
	return FALSE;
}

BOOL SpaceSyntexCheck(char *string)
{
	char *temp;
	temp = strchr( string, 0x20);
	if( temp == NULL ) return TRUE;
	return FALSE;
}

// 한글인지 체크해서 한글일때는 완성된 글자만 넣는다.
BOOL StrHangulCheck(char * str)
{
	BYTE code1, code2, code;
	int i=0, n=0;
	char sztemp[256]="";
	BOOL result = TRUE;
	int  count=255;

	while( count-- )
	{
		code1 = *(str+i);
		i++;
		if( code1 == '\0' ) break;
		if( code1 < 128 ) {
			sztemp[n] = code1; n++;
			continue;
		}
		code2 = *(str+i);
		i++;
		code = (code2)|(code1)<<8;
		if( code > 0 ) {
			sztemp[n] = code1; n++;
			sztemp[n] = code2; n++;
		}
		else result = FALSE;
		if( code2 == '\0' ) break;
	}
	strcpy( str, sztemp);
	return TRUE;
}

BOOL CheckSymbol(char* szString)
{
	if(szString == NULL)
		return TRUE;

	int len = strlen(szString);

	for(int i = 0; i < len;i++)
	{
		if(isalnum(szString[i]) == FALSE)
			return TRUE;
	}
return FALSE;
}

void FileSaveString(char *filename, char * string)
{
	FILE * fp;

	fp = fopen(filename, "at");
	if( fp == NULL ) return;
	fprintf(fp,string);
	fprintf(fp,"\n");
	fclose(fp);
}

void PHeadSetB(LPBYTE lpBuf, BYTE head, int size)
{
	lpBuf[0] =0xC1;		// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
}
	

void PHeadSubSetB(LPBYTE lpBuf, BYTE head, BYTE sub, int size)
{
	lpBuf[0] =0xC1;	// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
	lpBuf[3] =sub;
}

void PHeadSetW(LPBYTE lpBuf, BYTE head,  int size) 
{
	lpBuf[0] = 0xC2;	// Packets Header
	lpBuf[1]= SET_NUMBERH(size);
	lpBuf[2]= SET_NUMBERL(size);
	lpBuf[3]= head;
}

void PHeadSubSetW(LPBYTE lpBuf, BYTE head, BYTE sub,  int size)
{
	lpBuf[0]=0xC2;	// packet header
	lpBuf[1]=SET_NUMBERH(size);
	lpBuf[2]=SET_NUMBERL(size);
	lpBuf[3]=head;
	lpBuf[4]=sub;
}


void PHeadSetBE( LPBYTE lpBuf, BYTE head,int size)
{
	lpBuf[0] =0xC3;
	lpBuf[1] =size;
	lpBuf[2] =head;
}

void PHeadSubSetBE(LPBYTE lpBuf, BYTE head,BYTE sub, int size)
{
	lpBuf[0] =0xC3;
	lpBuf[1] =size;
	lpBuf[2] =head;
	lpBuf[3] =sub;
}