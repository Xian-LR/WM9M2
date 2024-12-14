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
	GamesEngineeringBase::Timer tim;
	float dt;

	ConstantBuffer* constBufferCPU = new ConstantBuffer();
	constBufferCPU->time = 0;
	
	canvas.Init("MyWindow", 1024, 768);
	dx.Init(1024, 768, canvas.hwnd);
	t.Init(dx);

	std::string vs = "Resources/vsshader.txt";
	std::string ps = "Resources/psshader.txt";
	std::string shaderName = "MyShader";

	shaders.load(shaderName, vs, ps, dx);
	Shader* shader = shaders.getShader(shaderName);


	while (true) {
		dx.clear();
		dt = tim.dt();
		constBufferCPU->time += dt;

		canvas.processMessages();

		// 将更新应用到 GPU
		
		shader->apply(dx);
		shader->UpdateConstantBuffer(dx);

		t.t += dt;
		t.draw(&*shader, dx);
		dx.present();
	}
}