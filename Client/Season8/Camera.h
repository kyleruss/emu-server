#pragma once
// ----------------------------------------------------------------------------------------------

enum ObjState
{
	SelectServer	= 2,
	SwitchCharacter = 4,
	GameProcess		= 5,
};
// ----------------------------------------------------------------------------------------------

#define pPlayerState			*(int*)0x11C2320
#define	pCamThis				((LPVOID(__cdecl*)()) 0x004E20D6)
#define pGetCameraZoom			((float(__thiscall*)(LPVOID This)) 0x004E20A6)
#define	pCursorX				*(int*)0x8B4ACFC
#define	pCursorY				*(int*)0x8B4AD00
//#define oCam_Zoom				0x1037DB0
#define oCam_PosZ				0x1031A64
#define oCam_RotZ				0x8B4ACC8
#define oCam_RotZDef			0x1033AB0
#define oCam_RotY				0x1033AC0
#define oCam_ClipX				0x1038D28
#define oCam_ClipY				0x1033A78
#define oCam_ClipZ				0x1038D78
#define oCam_ClipXCS			0x1038D58
#define oCam_ClipYCS			0x1038D68
#define oCam_ClipX2				0x1038D38
#define oCam_ClipY2				0x1038D40
#define oCam_ClipZ2				0x1037DA4
#define oCam_ClipGL				0x1031BCC
// ----------------------------------------------------------------------------------------------

#define ROTATION_SENS	5
#define ZOOM_MAX		60
#define ZOOM_MIN		15
#define ZOOM_INTER		1
#define CAMERA_DEBUG	1
#define PERCF(x, y)		((float)x / 100.0f * (float)y)
// ----------------------------------------------------------------------------------------------

typedef struct
{
	float	RotationY;
	float	RotationZ;
	float	PositionZ;
	double	ClipX;
	float	ClipY;
	double	ClipZ;
	double	ClipX2;
	float	ClipY2;
	float	ClipZ2;
	float	ClipGL;
} DefaultData, * lpDefData;
// ----------------------------------------------------------------------------------------------

class Camera
{
public:
				Camera();
				~Camera();
	// ----
	void		Load();
	void		Init();
	void		Switch();
	void		Run(MOUSEHOOKSTRUCTEX * Mouse, WPARAM wParam);
	void		Rotate();
	// ----
	static float __fastcall GetZoom(LPVOID This, LPVOID EDX);
	// ----
	bool		IsActive;
	float		ZoomPercent;
	float		Zoom;
	// ----
private:
	DefaultData Default;
	BYTE		RotateMode;
	bool		InMove;
	int			TempCursorX;
	int			TempCursorY;
	// ----
}; extern Camera g_Camera;
// ----------------------------------------------------------------------------------------------