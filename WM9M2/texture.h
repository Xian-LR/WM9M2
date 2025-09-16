#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include "GEMLoader.h"


class Sampler {
public:
	ID3D11SamplerState* state;

	void init(DxCore& dxcore) {
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		dxcore.device->CreateSamplerState(&samplerDesc, &state);

	}
	void bind(DxCore& core) {
		core.devicecontext->PSSetSamplers(0, 1, &state);

	}
};

class Texture {
public:
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	Sampler sampler;
	void init(DxCore* core, int width, int height, int channels, unsigned char* data, DXGI_FORMAT format) {
		D3D11_TEXTURE2D_DESC texDesc;
		memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		memset(&initData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = data;
		initData.SysMemPitch = width * channels;
		core->device->CreateTexture2D(&texDesc, &initData, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		core->device->CreateShaderResourceView(texture, &srvDesc, &srv);


	}

	void load(std::string filename, DxCore* dxcore, bool isNormalMap = false) {
		std::cout << "Attempting to load texture: " << filename << std::endl;

		int width = 0;
		int height = 0;
		int channels = 0;
		unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, 0);

		if (texels == nullptr) {
			std::cout << "Failed to load texture file: " << filename << std::endl;
			srv = nullptr;
			return;
		}

		std::cout << "Successfully loaded: " << filename
			<< " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;

		// Choose appropriate format
		DXGI_FORMAT format = isNormalMap ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		if (channels == 3) {
			channels = 4;
			unsigned char* texelsWithAlpha = new unsigned char[width * height * channels];
			for (int i = 0; i < (width * height); i++) {
				texelsWithAlpha[i * 4] = texels[i * 3];
				texelsWithAlpha[(i * 4) + 1] = texels[(i * 3) + 1];
				texelsWithAlpha[(i * 4) + 2] = texels[(i * 3) + 2];
				texelsWithAlpha[(i * 4) + 3] = 255;
			}
			init(dxcore, width, height, channels, texelsWithAlpha, format);
			delete[] texelsWithAlpha;
		}
		else {
			init(dxcore, width, height, channels, texels, format);
		}

		sampler.init(*dxcore);
		sampler.bind(*dxcore);
		stbi_image_free(texels);

		if (srv == nullptr) {
			std::cerr << "Error: SRV not created for " << filename << std::endl;
		}
		else {
			std::cout << "Successfully created SRV for " << filename << std::endl;
		}
	}
	void free() {
		srv->Release();
		texture->Release();
	}


};

// TextureManager with normal map support

class TextureManager {
public:
	std::map<std::string, Texture*> textures;
	ID3D11ShaderResourceView* defaultNormalSRV; // Default normal map

	void init(DxCore* core) {
		// Create default normal map (upward normal: RGB(128,128,255) = normal(0,0,1))
		createDefaultNormalMap(core);
	}

	void createDefaultNormalMap(DxCore* core) {
		// Create 1x1 default normal map
		unsigned char defaultNormalData[4] = { 128,128, 255, 255 }; // Default upward normal

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = 1;
		desc.Height = 1;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = defaultNormalData;
		initData.SysMemPitch = 4; // 4 bytes per pixel

		ID3D11Texture2D* defaultNormalTexture = nullptr;
		core->device->CreateTexture2D(&desc, &initData, &defaultNormalTexture);
		core->device->CreateShaderResourceView(defaultNormalTexture, nullptr, &defaultNormalSRV);

		defaultNormalTexture->Release();
	}

	void loadTexture(const std::string& filename, DxCore* core) {
		if (textures.find(filename) == textures.end()) {
			Texture* texture = new Texture();
			texture->load(filename, core);
			textures.insert({ filename, texture });
		}
	}



	// Load normal texture based on base texture name
	void loadNormalTexture(const std::string& baseTextureName, DxCore* core) {
		// Construct normal map filename from base texture name
		std::string normalFileName;
		size_t lastDot = baseTextureName.find_last_of('.');
		if (lastDot != std::string::npos) {
			normalFileName = baseTextureName.substr(0, lastDot) + "_Normal" + baseTextureName.substr(lastDot);
		}
		else {
			normalFileName = baseTextureName + "_Normal";
		}

		if (textures.find(normalFileName) == textures.end()) {
			Texture* texture = new Texture();
			texture->load(normalFileName, core, true);  // Load actual normal map file

			if (texture->srv != nullptr) {
				textures.insert({ normalFileName, texture });
			}
			else {
				delete texture;
			}
		}
	}

	ID3D11ShaderResourceView* find(std::string name) {
		auto it = textures.find(name);
		if (it != textures.end()) {
			return it->second->srv;
		}
		return nullptr;
	}

	// Find normal map, return default if not found
	ID3D11ShaderResourceView* findNormalMap(std::string baseName) {
		// Construct actual normal map filename
		std::string normalFileName;
		size_t lastDot = baseName.find_last_of('.');
		if (lastDot != std::string::npos) {
			normalFileName = baseName.substr(0, lastDot) + "_Normal" + baseName.substr(lastDot);
		}
		else {
			normalFileName = baseName + "_Normal";
		}

		auto it = textures.find(normalFileName);
		if (it != textures.end()) {
			return it->second->srv;
		}
		return defaultNormalSRV;
	}

	~TextureManager() {
		for (auto it = textures.cbegin(); it != textures.cend(); ) {
			it->second->free();
			delete it->second;
			textures.erase(it++);
		}
		if (defaultNormalSRV) defaultNormalSRV->Release();
	}
};