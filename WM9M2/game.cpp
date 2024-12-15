#include "window.h"
#include "animation.h"
#include "GamesEngineeringBase.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window canvas;
	DxCore dx;
	ShaderManager shaders;
//	LoadMesh mesh;
	LoadAnimation animation;
	GamesEngineeringBase::Timer tim;
	float dt;
	//ConstantBuffer* constBufferCPU = new ConstantBuffer();
	//constBufferCPU->time = 0;

	std::string vs2 = "Resources/vshader_ani.txt";
	std::string vs1 = "Resources/vsshader.txt";
	std::string ps = "Resources/psshader.txt";
	std::string shader1 = "Shader1";
	std::string shader2 = "Shader2";
	std::string model1 = "Models/acacia_003.gem";
	std::string model2 = "Models/TRex.gem";

	canvas.Init("MyWindow", 1024, 768);
	dx.Init(1024, 768, canvas.hwnd);

	//mesh.Init(dx, model1);
	animation.Init(dx, model2);


	//shaders.load(shader1, vs1, ps, dx,1);
	shaders.load(shader2, vs2, ps, dx, 0);
	//Shader* shaderst = shaders.getShader(shader1);
	Shader* shaderani = shaders.getShader(shader2);

	// ...
	while (true) {
	
		dx.clear();
		dt = tim.dt();
		//constBufferCPU->time += dt;
		canvas.processMessages();
		
		// 将更新应用到 GPU
		//shaderst->apply(dx);

//		shaderani->apply(dx);
		
		//mesh.t += dt;
		//mesh.draw(&*shaderst, dx);
		
		animation.t += dt;
		animation.draw(&*shaderani, dx,dt);


		dx.present();
		
	}
}