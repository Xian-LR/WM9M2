#pragma once
#include <D3D11.h>
#include <vector>
#include <dxgi1_6.h>

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DXGI.lib")

class Adapter {
public:
	IDXGIAdapter1* adapter;

	void findAdapter();
};
