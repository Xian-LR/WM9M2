#include "adapter.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void Adapter::findAdapter() {
	IDXGIAdapter1* adapterf;
	std::vector<IDXGIAdapter1*> adapters;
	IDXGIFactory6* factory = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory6), (void**)&factory);
	int i = 0;
	while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(adapterf);
		i++;
	}

	long long maxVideoMemory = 0;
	int useAdapterIndex = 0;
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC desc;
		adapters[i]->GetDesc(&desc);
		if (desc.DedicatedVideoMemory > maxVideoMemory)
		{
			maxVideoMemory = desc.DedicatedVideoMemory;
			useAdapterIndex = i;
		}
	}
	adapter = adapters[useAdapterIndex];
	factory->Release();
}