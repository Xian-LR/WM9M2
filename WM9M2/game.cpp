#include "window.h"
#include "dxCore.h"
#include "shader.h"
#include "mesh.h"
#include "GamesEngineeringBase.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window canvas;
	DxCore dx;
	ShaderManager shaders;
	Triangle t;
	GamesEngineeringBase::Timer tim;
	float dt;

	ConstantBuffer* constBufferCPU = new ConstantBuffer();
	constBufferCPU->time = 0;

	t.Init();
	canvas.Init("MyWindow", 1024, 768);
	dx.Init(1024, 768, canvas.hwnd);

	std::string s = "Resources/vertex_shader.txt";
	std::string shaderName = "MyShader";
	shaders.load(shaderName, s, s, dx.device);
	Shader* shader = shaders.getShader(shaderName);
	t.createBuffer(dx.device);

	while (true) {
		dx.clear();
		dt = tim.dt();
		constBufferCPU->time += dt;

		canvas.processMessages();

		// 将更新应用到 GPU
		shader->UpdateConstantBuffer(dx.devicecontext);
		shader->apply(dx.devicecontext);

		t.draw(dx.devicecontext);
		dx.present();
	}
}