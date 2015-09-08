#pragma once

#pragma pack(push, 1)
struct CommonInfo
{
	char IP[50];
	char Version[6];
	char Serial[17];
	WORD Port;
	WORD ChatEncoding;
#if (COMMONZ_DOWNGRADE == 0)
	BYTE ChatGlobalColorR;
	BYTE ChatGlobalColorG;
	BYTE ChatGlobalColorB;
#endif
};
#pragma pack(pop)

static BYTE XOR[3] = { 0xFC, 0xCF, 0xAB };

class Other
{
public:
	void	Load();
	void	ApplyChanges();
	void	ApplyConnection();

	BYTE* getGlobalColor() {
		BYTE Color[3] = { m_Info->ChatGlobalColorR,
			m_Info->ChatGlobalColorG,
			m_Info->ChatGlobalColorB };
		return Color;
	}
	BYTE ChatGlobalColor[3];
private:
	CommonInfo* m_Info;
}; extern Other g_Other;