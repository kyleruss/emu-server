#pragma once
// -------------------------------------------------------------------------------

#define ASM_JMP(Offset)		{__asm mov eax, Offset __asm jmp eax }
#define Naked(Function)		void __declspec(naked) Function()
// -------------------------------------------------------------------------------

#define MU_ItemObjectOffset		0x4E0
#define MU_GetItem(x, y)		(x * 512 + y)
#define MU_GetItemObject(x, y)	(x * 512 + y + MU_ItemObjectOffset)
// -------------------------------------------------------------------------------

#define MAX_WIN_WIDTH			640
#define MAX_WIN_HEIGHT			480
#define Color4f(r, b, g, o)		((o << 24) + (g << 16) + (b << 8) + r)