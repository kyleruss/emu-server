#pragma once

extern LRESULT OldWindowProc;
extern LRESULT NewWindowProc;
extern HWND* MainWindow;

BOOL KeyboardSetHook(BOOL bState);
BOOL MouseSetHook(BOOL bState);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void WindowState();
void NotifyIcon(int bState);