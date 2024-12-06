#pragma once
#include "Windows.h"
#include <string>
#include "memory.h"

#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

class Window {
public:
	HWND hwnd; // handle to the window
	HINSTANCE hInstance; // handle to the instance about the window
	std::string name;
	int width;
	int height;
	DWORD style;
	bool keys[256];
	int mousex;
	int mousey;
	bool mouseButtons[3];

	void Init(std::string window_name, int window_width, int window_height, int window_x = 0, int window_y = 0);
	void updateMouse(int x, int y);
	void processMessages();
};