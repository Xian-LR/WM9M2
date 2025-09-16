#pragma once
#include <d3d11.h>
#include "shader.h"
#include "GEMLoader.h"
#include "texture.h"
#include "camera.h"

#include "collision.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
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
	std::ofstream debugFile("debug_output.txt"); // Open file in append mode
	debugFile << name << "      ";

	debugFile << i << " ";


	debugFile << std::endl;
	debugFile.close();
}

void SaveMatrixToFile2(const mathLib::Matrix& matrix, std::string name) {
	std::ofstream debugFile("debug_output2.txt"); // Open file in append mode
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
	std::string groundTexture;
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
	void setTexture(const std::string& file, TextureManager& textures, DxCore& core) {
		groundTexture = file;
		textures.loadTexture(file, &core);
	}

	void Init(DxCore& core, float half = 100.0f, float tile = 40.0f) {
		std::vector<STATIC_VERTEX> vertices;

		vertices.push_back(addVertex(mathLib::Vec3(-half, 0, -half), mathLib::Vec3(0, 1, 0), 0.0f, 0.0f));
		vertices.push_back(addVertex(mathLib::Vec3(half, 0, -half), mathLib::Vec3(0, 1, 0), tile, 0.0f));
		vertices.push_back(addVertex(mathLib::Vec3(-half, 0, half), mathLib::Vec3(0, 1, 0), 0.0f, tile));
		vertices.push_back(addVertex(mathLib::Vec3(half, 0, half), mathLib::Vec3(0, 1, 0), tile, tile));

		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		mash.Init(vertices, indices, core);
	}

	void draw(Shader* shader, DxCore& core, const mathLib::Matrix& VP, TextureManager& textures) {
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &VP);
		if (!groundTexture.empty())
			shader->updateTexturePS(core, "tex", textures.find(groundTexture));
		shader->apply(core);
		mash.draw(core);
	}
};

#include <cfloat>  // FLT_MAX



class LoadMesh {
public:
	std::vector<Mesh> meshes;
	mathLib::Matrix planeWorld;
	std::vector<std::string> textureFilenames;
	float baseLift = 0.0f;
	float t = 0.0f;

	// Local space AABB (before world matrix/ground lift transformation)
	AABB localAABB;

	void Init(DxCore& core, std::string filename, TextureManager& textures) {
		planeWorld.identity();

		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);

		float modelMinY = FLT_MAX;

		// Initialize local AABB
		localAABB = AABB();

		for (int i = 0; i < (int)gemmeshes.size(); ++i) {
			std::vector<STATIC_VERTEX> vertices;
			vertices.reserve(gemmeshes[i].verticesStatic.size());

			for (int j = 0; j < (int)gemmeshes[i].verticesStatic.size(); ++j) {
				STATIC_VERTEX v;
				std::memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);

				if (v.pos.y < modelMinY) modelMinY = v.pos.y;

				// Accumulate to local AABB
				localAABB.expand(v.pos);
			}

			Mesh mesh;
			mesh.Init(vertices, gemmeshes[i].indices, core);
			meshes.push_back(mesh);

			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			textures.loadTexture(textureFilenames.back(), &core);
		}

		// Uniform lift to y=0
		baseLift = -modelMinY;
	}

	void translate(mathLib::Vec3 v) { planeWorld = planeWorld * mathLib::Matrix::translation(v); }
	void scale(mathLib::Vec3 v) { planeWorld = planeWorld * mathLib::Matrix::scaling(v); }

	// Get world space AABB (consistent with draw() method's W matrix)
	AABB getWorldAABB() const {
		mathLib::Matrix lift = mathLib::Matrix::translation({ 0, baseLift, 0 });
		mathLib::Matrix W_final = lift * planeWorld;
		return localAABB.transform(W_final);
	}

	void draw(Shader* shader, DxCore& core, TextureManager& textures, const mathLib::Matrix& VP) {
		for (int i = 0; i < (int)meshes.size(); ++i) {
			mathLib::Matrix W_final = mathLib::Matrix::translation({ 0, baseLift, 0 }) * planeWorld;
			shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &W_final);
			shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &VP);
			shader->updateTexturePS(core, "tex", textures.find(textureFilenames[i]));
			shader->apply(core);
			meshes[i].draw(core);
		}
	}
};

// SkyDome: inward-facing textured sphere that follows the camera
class SkyDome {
public:
	Mesh mesh;
	std::string textureFilename;
	mathLib::Matrix world;
	float rotation = 0.0f;
	float rotationSpeed = 0.03f; // Slow auto-rotation

	// Generate inward-facing triangle sphere: rings × segments, radius
	void Init(DxCore& core, int rings, int segments, float radius,
		const std::string& tex, TextureManager& textures) {
		world.identity();
		textureFilename = tex;
		if (!tex.empty()) textures.loadTexture(tex, &core);

		std::vector<STATIC_VERTEX> vertices;
		std::vector<unsigned int>  indices;

		for (int lat = 0; lat <= rings; ++lat) {
			float v = (float)lat / (float)rings;           
			float theta = v * (float)M_PI;                  
			float st = sinf(theta), ct = cosf(theta);

			for (int lon = 0; lon <= segments; ++lon) {
				float u = (float)lon / (float)segments;    
				float phi = u * 2.0f * (float)M_PI;         
				float sp = sinf(phi), cp = cosf(phi);

				mathLib::Vec3 dir(st * cp, ct, st * sp);    
				STATIC_VERTEX vtx;
				vtx.pos = dir * radius;
				vtx.normal = -dir;                         
				vtx.tangent = mathLib::Vec3(0, 0, 0);
				vtx.tu = u;
				vtx.tv = 1.0f - v;                     
				vertices.push_back(vtx);
			}
		}

		int stride = segments + 1;
		for (int lat = 0; lat < rings; ++lat) {
			for (int lon = 0; lon < segments; ++lon) {
				int i0 = lat * stride + lon;
				int i1 = i0 + 1;
				int i2 = (lat + 1) * stride + lon;
				int i3 = i2 + 1;
				// Reverse winding => inward-facing triangles
				indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
				indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
			}
		}
		mesh.Init(vertices, indices, core);
	}

	void update(float dt) { rotation += rotationSpeed * dt; }

	// Translate by camera position + slight auto-rotation, write W/VP and bind sky texture
	void draw(Shader* shader, DxCore& core,
		const mathLib::Vec3& cameraPos,
		const mathLib::Matrix& VP,
		TextureManager& textures) {
		mathLib::Matrix T = mathLib::Matrix::translation(cameraPos);
		mathLib::Matrix R = mathLib::Matrix::rotateY(rotation);
		mathLib::Matrix W = T * R;

		// Same constant buffer writing as Plane::draw() (staticMeshBuffer: W, VP)
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &W);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &VP);
		if (!textureFilename.empty()) {
			shader->updateTexturePS(core, "tex", textures.find(textureFilename));
		}
		shader->apply(core);
		mesh.draw(core);
	}
};