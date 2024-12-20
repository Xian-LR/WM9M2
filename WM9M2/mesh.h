﻿#pragma once
#include <d3d11.h>
#include "shader.h"
#include "GEMLoader.h"
#include "texture.h"
#include "camera.h"

struct Vertex
{
	// screen space position and colour
	mathLib::Vec3 position;
	mathLib::Color color;
};

struct STATIC_VERTEX
{
	mathLib::Vec3 pos;
	mathLib::Vec3 normal;
	mathLib::Vec3 tangent;
	float tu;
	float tv;
};

struct ANIMATED_VERTEX
{
	mathLib::Vec3 pos;
	mathLib::Vec3 normal;
	mathLib::Vec3 tangent;
	float tu;
	float tv;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};

void SaveMatrixToFile(int i, std::string name) {
	std::ofstream debugFile("debug_output.txt"); // 打开文件，追加模式
	debugFile << name << "      ";

	debugFile << i << " ";


	debugFile << std::endl;
	debugFile.close();
}

void SaveMatrixToFile2(const mathLib::Matrix& matrix, std::string name) {
	std::ofstream debugFile("debug_output2.txt"); // 打开文件，追加模式
	debugFile << name << std::endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			debugFile << matrix.a[i][j] << " ";
		}
		debugFile << std::endl;
	}
	debugFile << std::endl;
	debugFile.close();
}

class Triangle {
	Vertex vertices[3];
public:
	ID3D11Buffer* vertexBuffer;

	// create a triangle
	void Init() {
		vertices[0].position = mathLib::Vec3(0, 1.0f, 0);
		vertices[0].color = mathLib::Color(0, 1.0f, 0);
		vertices[1].position = mathLib::Vec3(-1.0f, -1.0f, 0);
		vertices[1].color = mathLib::Color(1.0f, 0, 0);
		vertices[2].position = mathLib::Vec3(1.0f, -1.0f, 0);
		vertices[2].color = mathLib::Color(0, 0, 1.0f);
	}

	// create buffer and upload vertices to GPU
	void createBuffer(ID3D11Device* device, int N = 3) {
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA uploadData;
		bd.ByteWidth = sizeof(Vertex) * N;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		uploadData.pSysMem = vertices;
		uploadData.SysMemPitch = 0;
		uploadData.SysMemSlicePitch = 0;
		device->CreateBuffer(&bd, &uploadData, &vertexBuffer);
	}

	// ask the GPU to draw a triangle
	void draw(ID3D11DeviceContext* devicecontext) {
		UINT offsets;
		offsets = 0;
		UINT strides = sizeof(Vertex); // size of vertex
		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext->Draw(3, 0);
	}
};

class Mesh {
public:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	int indicesSize;
	UINT strides;

	void Init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, DxCore& device) {
		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		device.device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		device.device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}

	void Init(std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, DxCore& device)
	{
		Init(&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}

	void Init(std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices, DxCore& device)
	{
		Init(&vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}


	void draw(DxCore& devicecontext) {
		UINT offsets = 0;
		devicecontext.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext.devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext.devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		devicecontext.devicecontext->DrawIndexed(indicesSize, 0, 0);
	}
};

class Plane {
public:
	Mesh mash;
	mathLib::Matrix planeWorld;
	mathLib::Matrix vp;
	float t = 0.0f;
	STATIC_VERTEX addVertex(mathLib::Vec3 p, mathLib::Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		//Frame frame;
		//frame.fromVector(n);
		//v.tangent = frame.u;
		v.tangent = mathLib::Vec3(0, 0, 0);
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void Init(DxCore& core) {
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(mathLib::Vec3(-1.5, 0, -1.5), mathLib::Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(mathLib::Vec3(1.5, 0, -1.5), mathLib::Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(mathLib::Vec3(-1.5, 0, 1.5), mathLib::Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(mathLib::Vec3(1.5, 0, 1.5), mathLib::Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		mash.Init(vertices, indices, core);
	}

	void draw(Shader* shader, DxCore& core) {
		mathLib::Vec3 from = mathLib::Vec3(11 * cos(t), 5, 11 * sin(t));
		mathLib::Vec3 to = mathLib::Vec3(0.0f, 0.0f, 0.0f);
		mathLib::Vec3 up = mathLib::Vec3(0.0f, 1.0f, 0.0f);
		vp = mathLib::lookAt(from, to, up)* mathLib::PerPro(1.f, 1.f, 20.f, 100.f, 0.1f);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vp);
		mash.draw(core);
	}
};

class LoadMesh {
public:
	std::vector<Mesh> meshes;
	mathLib::Matrix planeWorld;

	mathLib::Matrix vp;
	float t = 0.0f;
	std::vector<std::string> textureFilenames;


	void Init(DxCore& core, std::string filename, TextureManager& textures) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh mesh;
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}
			mesh.Init(vertices, gemmeshes[i].indices, core);
			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			textures.loadTexture(gemmeshes[i].material.find("diffuse").getValue(), &core);
			meshes.push_back(mesh);
		}
	}

	void translate(mathLib::Vec3 v) {
		planeWorld = planeWorld * mathLib::Matrix::translation(v);
	}

	void scale(mathLib::Vec3 v) {
		planeWorld = planeWorld * mathLib::Matrix::scaling(v);
	}

	void draw(Shader* shader, DxCore& core, TextureManager& textures) {
		//mathLib::Vec3 from = mathLib::Vec3(150.0f, 5.0f, 250.0f);
		//mathLib::Vec3 to = mathLib::Vec3(0.0f, 0.0f, 0.0f);
		//mathLib::Vec3 up = mathLib::Vec3(0.0f, 1.0f, 0.0f);
		//vp = mathLib::lookAt(from, to, up) * mathLib::PerPro(1.f, 1.f, 90.f, 300.f, 0.1f);

		
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		//shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vp);
		shader->apply(core);
		for (int i = 0; i < meshes.size(); i++)
		{
			shader->updateTexturePS(core, "tex", textures.find(textureFilenames[i]));
			meshes[i].draw(core);
		}
		

	}
};


