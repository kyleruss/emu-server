#pragma once

// PROTOTYPE DEFINE
typedef void(*pFunctionVOID) ( ... );
typedef BYTE(*pFunctionBYTE) ( ... );
typedef WORD(*pFunctionWORD) ( ... );
typedef DWORD(*pFunctionDWORD) ( ... );

typedef DWORD(__thiscall *pCDC_GetMessageBox)(DWORD);
extern pCDC_GetMessageBox CDC_GetMessageBox;

typedef void(__thiscall *pCDC_MessageBox)(DWORD,DWORD,DWORD,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD);
extern pCDC_MessageBox CDC_MessageBox;

void __cdecl MyDrawInterface();
extern pFunctionDWORD CDC_GetMessageThis;
extern pFunctionVOID CallScaleForm;