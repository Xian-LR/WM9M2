#include "window.h"
#include "dxCore.h"
#include "shader.h"
#include "mesh.h"
#include "GamesEngineeringBase.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window canvas;
	DxCore dx;
	ShaderManager shaders;
	Plane t;
	LoadMesh mesh;
	GamesEngineeringBase::Timer tim;
	float dt;
	ConstantBuffer* constBufferCPU = new ConstantBuffer();
	constBufferCPU->time = 0;

	std::string vs = "Resources/vsshader.txt";
	std::string ps = "Resources/psshader.txt";
	std::string shaderName = "MyShader";
	std::string model1 = "Models/acacia_003.gem";

	canvas.Init("MyWindow", 1024, 768);
	dx.Init(1024, 768, canvas.hwnd);
	//t.Init(dx);
	mesh.Init(dx, model1);

	shaders.load(shaderName, vs, ps, dx);
	Shader* shader = shaders.getShader(shaderName);
	
	// ...
	while (true) {
	
		dx.clear();
		dt = tim.dt();
		constBufferCPU->time += dt;
		canvas.processMessages();
		
		// 将更新应用到 GPU
		shader->apply(dx);
		shader->UpdateConstantBuffer(dx);
		//t.t += dt;
		//t.draw(&*shader, dx);
		
		mesh.t += dt;
		mesh.draw(&*shader, dx);
		dx.present();
		
	}
}