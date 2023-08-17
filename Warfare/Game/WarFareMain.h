#pragma once

LRESULT CALLBACK WndProcMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcSub(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OYBLowLevelKeyboardProc (INT nCode, WPARAM wParam, LPARAM lParam);

#define WH_KEYBOARD				2
#define WH_KEYBOARD_LL			13
#define LLKHF_ALTDOWN			0x00000020
