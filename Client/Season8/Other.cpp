#include "stdafx.h"
#include "Other.h"
#include "Security.h"

Other g_Other;

void Other::Load()
{
	this->ApplyChanges();
	this->ApplyConnection();
}

void Other::ApplyChanges()
{
	//Change paths
	static LPSTR StackLog	= "Logs\\stack_%04d%02d%02d_%02d%02d.txt";
	SetDword((PVOID)(0x00A247AA+1), (DWORD)StackLog);
	static LPSTR StackLog1	= "Logs\\stack_%s.txt";
	SetDword((PVOID)(0x00A2472C+1), (DWORD)StackLog1);
	static LPSTR CrashLog	= "Logs\\CrashLog.txt";
	SetDword((PVOID)(0x00A275E6+1), (DWORD)CrashLog);
	static LPSTR CrashLog1	= "Logs\\CrashLog_%d.txt";
	SetDword((PVOID)(0x00A27633+1), (DWORD)CrashLog1);
	static LPSTR CrashDump	= "Logs\\crash_%04d%02d%02d_%02d%02d.dmp";
	SetDword((PVOID)(0x00A24776+1), (DWORD)CrashDump);
	static LPSTR ErrorLog	= "Logs\\Error.log";
	SetDword((PVOID)(0x00A220B9+1), (DWORD)ErrorLog);
	static LPSTR ErrorLog2	= "Logs\\Error_%d.log";
	SetDword((PVOID)(0x00A221A9+1), (DWORD)ErrorLog2);
	static LPSTR ScreenName	= "Images\\Screen(%02d_%02d-%02d_%02d)-%04d.jpg";
	SetDword((PVOID)(0x004E0C9E+1), (DWORD)ScreenName);
	//MuError.log encryption
	SetRange((LPVOID)0x00A22478, 32, NOP);
	//Dump correct
	BYTE DMP_FIX[] = { 0xE9, 0x05, 0x01, 0x00, 0x00, 0x90 };
	SetBuffer((LPVOID)0x00A25FA1, DMP_FIX, sizeof(DMP_FIX));
	SetBuffer((LPVOID)0x00A260CB, DMP_FIX, sizeof(DMP_FIX));
	//Minidump fix
	SetRange((LPVOID)0x00A25B32, 6, NOP);
	SetRange((LPVOID)0x00A25B47, 6, NOP);
	//CrashLog.txt remove
	SetRange((LPVOID)0x00A27585, 5, NOP);
	SetRange((LPVOID)0x00A27599, 5, NOP);
	//Website loading remove
	BYTE WEB_CHECK[] = { 0xB0, 0x01, 0x90 };
	SetBuffer((LPVOID)0x0043EBF8, WEB_CHECK, sizeof(WEB_CHECK));
	//Login fix
	SetByte((LPVOID)(0x0043E045), SJMP);
	SetByte((LPVOID)(0x0043DF85 + 1), 0);
	SetByte((LPVOID)(0x0043E4CC + 2), 0x18);
	SetDword((LPVOID)(0x0043E4BB + 2), 0x350);
	SetDword((LPVOID)(0x0043E4C4 + 2), 0x350);
	//Path overwrite fix
	SetRange((LPVOID)0x004D8499, 5, NOP);
	//Character create fix
	SetByte((LPVOID)(0x00437E47 + 1), 0x60);
	//Siege minimap fix
	SetRange((LPVOID)0x09DF2293, 6, NOP);
	SetByte((LPVOID)(0x0086D8A8 + 1), 0);
	//Expanded inventory fix
	SetByte((LPVOID)(0x0098EAE3 + 1), 4);
	//Jewel of Life +28 fix
	SetByte((LPVOID)(0x00A100E4 + 3), 7);
	//Stat display fix
	SetRange((LPVOID)0x009C4779, 6, NOP);
	SetRange((LPVOID)0x009C4877, 6, NOP);
	SetRange((LPVOID)0x009C4975, 6, NOP);
	SetRange((LPVOID)0x009C4AE3, 6, NOP);
	SetRange((LPVOID)0x009C4C03, 6, NOP);
	//Set defense rate
	//*(BYTE*)(0x009C78DE+1) = 5;
	//*(BYTE*)(0x009C7931+1) = 5;
	//*(BYTE*)(0x009C7981+1) = 5;
	//Buff time display fix
	SetRange((LPVOID)0x0065D857, 3, 0x02);
	//Disable "Gift" button (CashShop)
	SetByte((LPVOID)(0x00941CE6 + 1), 0);
	SetByte((LPVOID)(0x00943862 + 1), 0);
	//ChatServer disconnect fix
	SetRange((LPVOID)0x004B2491, 5, NOP);
	//Allow move list in Market
	//SetRange((LPVOID)0x009B354C, 50, NOP);
}

void CharSetFix()
{
	BYTE CharSetHok1[] = {0xEB,0x11};
	HookBUFFER((LPVOID)0x004381C7,CharSetHok1,sizeof(CharSetHok1));

	BYTE CharSetHok2[] = {0xEB,0x14};
	HookBUFFER((LPVOID)0x00438348,CharSetHok2,sizeof(CharSetHok2));

	BYTE CharSetHok3[] = {0x6A,0x00};
	HookBUFFER((LPVOID)0x0043DF85,CharSetHok3,sizeof(CharSetHok3));

	BYTE CharSetHok4[] = {0x8B, 0x80, 0x50, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x0043E4BB,CharSetHok4,sizeof(CharSetHok4));

	BYTE CharSetHok5[] = {0x8B, 0x89, 0x50, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x0043E4C4,CharSetHok5,sizeof(CharSetHok5));

	BYTE CharSetHok6[] = {0xFF,0x50,0x18};
	HookBUFFER((LPVOID)0x0043E4CC,CharSetHok6,sizeof(CharSetHok6));

	BYTE CharSetHok7[] = {0xC7, 0x85, 0x04, 0xFF, 0xFF, 0xFF, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x004492B9,CharSetHok7,sizeof(CharSetHok7));
	
	BYTE CharSetHok8[] = {0x7F,0x00};
	HookBUFFER((LPVOID)0x00452709,CharSetHok8,sizeof(CharSetHok8));

	BYTE CharSetHok9[] = {0xE9,0xE4,0x00,0x00,0x00,0x90};
	HookBUFFER((LPVOID)0x0049EA59,CharSetHok9,sizeof(CharSetHok9));

	BYTE CharSetHok10[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x004B3749,CharSetHok10,sizeof(CharSetHok10));

	BYTE CharSetHok11[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x004B7524,CharSetHok11,sizeof(CharSetHok11));

	BYTE CharSetHok12[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x004B7545,CharSetHok12,sizeof(CharSetHok12));

	BYTE CharSetHok13[] = {0xEB, 0x36};
	HookBUFFER((LPVOID)0x005079DD,CharSetHok13,sizeof(CharSetHok13));

// 	BYTE CharSetHok14[] = {0xEB, 0x5C};
// 	HookBUFFER((LPVOID)0x0050E183,CharSetHok14,sizeof(CharSetHok14)); //中文战盟、中文角色
	
	BYTE CharSetHok15[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x005961E4,CharSetHok15,sizeof(CharSetHok15));

	BYTE CharSetHok16[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00596202,CharSetHok16,sizeof(CharSetHok16));

	BYTE CharSetHok17[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x0078FEEB,CharSetHok17,sizeof(CharSetHok17));

	BYTE CharSetHok18[] = {0xEB, 0x55};
	HookBUFFER((LPVOID)0x007DFC14,CharSetHok18,sizeof(CharSetHok18));

	BYTE CharSetHok19[] = {0xEB, 0x55};
	HookBUFFER((LPVOID)0x004EF3F4,CharSetHok19,sizeof(CharSetHok19));

	BYTE CharSetHok20[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x007901E2,CharSetHok20,sizeof(CharSetHok20));

	BYTE CharSetHok21[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00793FC6,CharSetHok21,sizeof(CharSetHok21));

	BYTE CharSetHok22[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x0079401F,CharSetHok22,sizeof(CharSetHok22));

	BYTE CharSetHok23[] = {0x68, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00831E26,CharSetHok23,sizeof(CharSetHok23));

	BYTE CharSetHok24[] = {0x74, 0x00};
	HookBUFFER((LPVOID)0x00A1228A,CharSetHok24,sizeof(CharSetHok24));

	BYTE CharSetHok25[] = {0xC7, 0x45, 0xE4, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00A1228C,CharSetHok25,sizeof(CharSetHok25));

	BYTE CharSetHok26[] = {0x25, 0x58, 0xFC, 0xFF, 0xFF};
	HookBUFFER((LPVOID)0x00A123E8,CharSetHok26,sizeof(CharSetHok26));

	BYTE CharSetHok27[] = {0x05, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00A123ED,CharSetHok27,sizeof(CharSetHok27));
	
	BYTE CreateChar[] = {0x55, 0x8B, 0xEC, 0x53, 0x8B, 0x5D, 0x08, 0x56, 0x57, 0x33, 0xFF, 0x80, 0x3B, 0x00, 0x74, 0x66, 0x8D, 0x34, 0x3B, 0x56, 0xE8, 0xEB, 0x45, 0x7C, 0x00, 0x83, 0xC4, 0x04, 0x83, 0xF8, 0x01, 0x75,
		0x2C, 0x8A, 0x06, 0x3C, 0x21, 0x72, 0x04, 0x3C, 0x2F, 0x76, 0x18, 0x3C, 0x3A, 0x72, 0x04, 0x3C, 0x40, 0x76, 0x10, 0x3C, 0x5B, 0x72, 0x04, 0x3C, 0x60, 0x76, 0x08, 0x3C, 0x7B, 0x72, 0x30, 0x3C,
		0x7E, 0x77, 0x2C, 0x5F, 0x5E, 0xB8, 0x01, 0x00, 0x00, 0x00, 0x5B, 0x5D, 0xC3, 0x8A, 0x0E, 0x80, 0xF9, 0xB0, 0x72, 0xEF, 0x80, 0xF9, 0xF7, 0x77, 0xEA, 0x8A, 0x44, 0x3B, 0x01, 0x3C, 0xA1, 0x72,
		0xE2, 0x3C, 0xFE, 0x77, 0xDE, 0x80, 0xF9, 0xD7, 0x75, 0x04, 0x3C, 0xFA, 0x73, 0xD5, 0x47, 0x47, 0x80, 0x3C, 0x3B, 0x00, 0x75, 0x9A, 0x5F, 0x5E, 0x33, 0xC0, 0x5B, 0x5D, 0xC3};
	HookBUFFER((LPVOID)0x0050E168,CreateChar,sizeof(CreateChar)); // New code.. but worked only with Character Name Greate. not include the Guild name create.strange shit


	BYTE OtherCharSetFix1[] = {0x81, 0x7D, 0xF0, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00E0CF48,OtherCharSetFix1,sizeof(OtherCharSetFix1));

	BYTE OtherCharSetFix2[] = {0x81, 0x7D, 0x20, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00EEB3CD,OtherCharSetFix2,sizeof(OtherCharSetFix2));

	BYTE OtherCharSetFix3[] = {0x81, 0x7D, 0xC0, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F112BC,OtherCharSetFix3,sizeof(OtherCharSetFix3));

	BYTE OtherCharSetFix4[] = {0xC7, 0x85, 0x58, 0xFF, 0xFF, 0xFF, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F11374,OtherCharSetFix4,sizeof(OtherCharSetFix4));

	BYTE OtherCharSetFix5[] = {0x81, 0xBD, 0x58, 0xFF, 0xFF, 0xFF, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F1137E,OtherCharSetFix5,sizeof(OtherCharSetFix5));

	BYTE OtherCharSetFix6[] = {0x3D, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F1479D,OtherCharSetFix6,sizeof(OtherCharSetFix6));

	BYTE OtherCharSetFix7[] = {0x81, 0x7D, 0xA0, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F2F45C,OtherCharSetFix7,sizeof(OtherCharSetFix7));

	BYTE OtherCharSetFix8[] = {0x81, 0x7D, 0xA0, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F2F54C,OtherCharSetFix8,sizeof(OtherCharSetFix8));

	BYTE OtherCharSetFix9[] = {0xC7, 0x45, 0xC8, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F2F64D,OtherCharSetFix9,sizeof(OtherCharSetFix9));

	BYTE OtherCharSetFix10[] = {0x81, 0x7D, 0xC8, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F2F667,OtherCharSetFix10,sizeof(OtherCharSetFix10));

	BYTE OtherCharSetFix11[] = {0xC7, 0x40, 0x04, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F44326,OtherCharSetFix11,sizeof(OtherCharSetFix11));

	BYTE OtherCharSetFix12[] = {0x81, 0x7A, 0x04, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F44598,OtherCharSetFix12,sizeof(OtherCharSetFix12));
	
	BYTE OtherCharSetFix13[] = {0x81, 0x7A, 0x04, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F448D6,OtherCharSetFix13,sizeof(OtherCharSetFix13));

	BYTE OtherCharSetFix14[] = {0xC7, 0x45, 0xE4, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F45A5F,OtherCharSetFix14,sizeof(OtherCharSetFix14));

	BYTE OtherCharSetFix15[] = {0xC7, 0x85, 0x7C, 0xFF, 0xFF, 0xFF, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F7B96A,OtherCharSetFix15,sizeof(OtherCharSetFix15));

	BYTE OtherCharSetFix16[] = {0x3D, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00F8DF3A,OtherCharSetFix16,sizeof(OtherCharSetFix16));

	BYTE OtherCharSetFix17[] = {0xC7, 0x85, 0xFC, 0xFE, 0xFF, 0xFF, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00FA9622,OtherCharSetFix17,sizeof(OtherCharSetFix17));

	BYTE OtherCharSetFix18[] = {0x81, 0x7D, 0xEC, 0xA8, 0x03, 0x00, 0x00};
	HookBUFFER((LPVOID)0x00FAC7EB,OtherCharSetFix18,sizeof(OtherCharSetFix18));

	BYTE OtherCharSetFix19[] = {0xC7, 0x40, 0x04, 0xA8, 0x03, 0x00, 0x00}; // Added New Value
	HookBUFFER((LPVOID)0xA11FFC,OtherCharSetFix19,sizeof(OtherCharSetFix19));
}

void Other::ApplyConnection()
{
	std::ifstream myReadFile;

#ifdef VMPROTECT
	myReadFile.open(VMProtectDecryptStringA("Data\\Custom\\Common.z"));
	if( !myReadFile.is_open() )
	{
		MessageBox(NULL, VMProtectDecryptStringA("Common.z not found"), VMProtectDecryptStringA("zClient"), ERROR);
		return;
	}
#else
	myReadFile.open("Data\\Custom\\Common.z");
	if( !myReadFile.is_open() )
	{
		MessageBox(NULL, "Common.z not found", "zClient", ERROR);
		return;
	}
#endif

	char Data[sizeof(CommonInfo) + 1] = { 0 };

	while( !myReadFile.eof() ) 
	{
			myReadFile >> Data;
	}

	myReadFile.close();

	for( int i = 0; i < sizeof(CommonInfo); i++ )
	{
		Data[i] = Data[i] ^ XOR[i % 3];	
	}
	m_Info = (CommonInfo*)Data;

	ChatGlobalColor[0] = m_Info->ChatGlobalColorR;
	ChatGlobalColor[1] = m_Info->ChatGlobalColorG;
	ChatGlobalColor[2] = m_Info->ChatGlobalColorB;

	SetBuffer((LPVOID)oServerIP, (LPBYTE)m_Info->IP, sizeof(m_Info->IP));
	SetWord((LPVOID)oServerPort, m_Info->Port);
	SetBuffer((LPVOID)oClientVersion, (LPBYTE)m_Info->Version, sizeof(m_Info->Version));
	SetBuffer((LPVOID)oSerialOffset, (LPBYTE)m_Info->Serial, sizeof(m_Info->Serial) - 1);

	BYTE CHAT_HOOK[] = {0xB8,0xE3,0x04,0x00,0x00,0x90};
	SetBuffer((LPVOID)0x00A120C8, CHAT_HOOK, sizeof(CHAT_HOOK));
	SetDword((LPVOID)(0x00A120C8 + 1), m_Info->ChatEncoding); //push value

	if(	m_Info->ChatEncoding == 936 )
	{
		//PUSH_3A8CharSetFix();
		CharSetFix();
	}
}