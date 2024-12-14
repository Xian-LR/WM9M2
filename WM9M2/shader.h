#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include "mathLib.h"
#include "shaderreflection.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


class Shader {
public:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* constantBuffer;
	ConstantBuffer constBufferCPU;

	std::vector<ConstantBuffer> vsConstantBuffers;
	std::vector<ConstantBuffer> psConstantBuffers;
	std::map<std::string, int> textureBindPointsVS;
	std::map<std::string, int> textureBindPointsPS;

	void initConstBuffer(int sizeInBytes, DxCore& devicecontext) {
		constBufferCPU.time = 0.f;
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		bd.ByteWidth = sizeInBytes;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		devicecontext.device->CreateBuffer(&bd, NULL, &constantBuffer);
	}

	// 更新常量缓冲区，将CPU数据同步到GPU
	void UpdateConstantBuffer(DxCore& core) {
		for (auto& buffer : vsConstantBuffers) {
			buffer.upload(&core);
		}
		for (auto& buffer : psConstantBuffers) {
			buffer.upload(&core);
		}
	}

	void updateConstantVS(std::string name, const std::string& cbName, const std::string& variableName, void* data) {
		for (auto& cb : vsConstantBuffers) {
			if (cb.name == cbName) {
				cb.update(variableName, data);
				return;
			}
		}
	}

	void updateConstantPS(std::string name, const std::string& cbName, const std::string& variableName, void* data) {
		for (auto& cb : psConstantBuffers) {
			if (cb.name == cbName) {
				cb.update(variableName, data);
				return;
			}
		}
	}

	//compile vertex shader
	void loadVS(std::string& filename, DxCore& core) {
		ID3DBlob* status;
		ID3DBlob* shader;
		std::string shaderHLSL = readFile(filename);
		// compile vertex shader
		HRESULT hr = D3DCompile(shaderHLSL.c_str(), strlen(shaderHLSL.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &shader, &status);
		if (FAILED(hr)) {
			MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
			exit(0);
		}
		
		core.device->CreateVertexShader(shader->GetBufferPointer(), shader->GetBufferSize(), NULL, &vertexShader);
		// Link Geometry
		D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
		{
			{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		core.device->CreateInputLayout(layoutDesc, 4, shader->GetBufferPointer(), shader->GetBufferSize(), &layout);


		// 使用反射系统提取常量缓冲区和纹理绑定点信息
		ConstantBufferReflection reflection;
		reflection.build(&core, shader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);
		shader->Release();
	}

	//compile pixel shader
	void loadPS(std::string& filename, DxCore& core) {
		ID3DBlob* status;
		ID3DBlob* shader;
		std::string shaderHLSL = readFile(filename);
		// compile pixel shader
		HRESULT hr = D3DCompile(shaderHLSL.c_str(), strlen(shaderHLSL.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &shader, &status);
		if (FAILED(hr)) {
			MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Pixel Shader Error", 0);
			exit(0);
		}
		// create pixel shader
		core.device->CreatePixelShader(shader->GetBufferPointer(), shader->GetBufferSize(), NULL, &pixelShader);
		ConstantBufferReflection reflection;
		reflection.build(&core, shader, psConstantBuffers, textureBindPointsPS, ShaderStage::PixelShader);
		shader->Release();
	}
	//Send shaders to GPU
	void apply(DxCore& core) const {
		core.devicecontext->IASetInputLayout(layout);
		core.devicecontext->VSSetShader(vertexShader, NULL, 0);
		core.devicecontext->PSSetShader(pixelShader, NULL, 0);
		core.devicecontext->PSSetConstantBuffers(0, 1, &constantBuffer);
	}

	// 释放资源
	void Release() {
		if (vertexShader) vertexShader->Release();
		if (pixelShader) pixelShader->Release();
		if (layout) layout->Release();
		for (auto& buffer : vsConstantBuffers) {
			buffer.free();
		}
		for (auto& buffer : psConstantBuffers) {
			buffer.free();
		}
	}

private:
	//read HLSL file
	std::string readFile(std::string& filename) {
		std::ifstream infile;
		infile.open(filename);


		std::stringstream buffer;
		buffer << infile.rdbuf();

		return buffer.str();
	}
};

class ShaderManager {
public:
	std::map<std::string, Shader> shaders;

	void load(std::string& name, std::string& vsFilename, std::string& psFilename, DxCore& core) {
		Shader shader;
		ID3DBlob* shaderBlob = nullptr;
		shader.loadVS(vsFilename, core);
		shader.loadPS(psFilename, core);
		shaders[name] = shader;
	}

	Shader* getShader(std::string& name) {
		auto it = shaders.find(name);
		if (it != shaders.end()) {
			return &it->second;
		}
		return nullptr;
	}

	void apply(std::string& name, DxCore& core) {
		Shader* shader = getShader(name);
		if (shader) {
			shader->apply(core);
		}
	}


};