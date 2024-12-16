#include "window.h"
#include "animation.h"
#include "GamesEngineeringBase.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window canvas;
	DxCore dx;
	ShaderManager shaders;
	LoadMesh mesh;
	LoadAnimation uzi;
	//LoadAnimation animation;
	GamesEngineeringBase::Timer tim;
	float dt;
	TextureManager textures;
	//ConstantBuffer* constBufferCPU = new ConstantBuffer();
	//constBufferCPU->time = 0;

	std::string vs2 = "Resources/vshader_ani.txt";
	std::string vs1 = "Resources/vsshader.txt";
	std::string ps = "Resources/psshader.txt";
	std::string shader1 = "Shader1";
	std::string shader2 = "Shader2";

	std::string model1 = "Models/pine.gem";
	std::string model2 = "Models/TRex.gem";
	std::string model3 = "Models/Uzi.gem";

	canvas.Init("MyWindow", 1024, 768);
	dx.Init(1024, 768, canvas.hwnd);



	shaders.load(shader1, vs1, ps, dx,1);
	shaders.load(shader2, vs2, ps, dx, 0);

	Shader* shaderst = shaders.getShader(shader1);
	Shader* shaderani = shaders.getShader(shader2);


	mesh.Init(dx, model1, textures);
//	animation.Init(dx, model2, textures);
	uzi.Init(dx, model3, textures);

	mesh.scale(mathLib::Vec3(0.3f, 0.3f, 0.3f));
	mesh.translate(mathLib::Vec3(100.0f, -50.0f, 0.0f));
	//animation.translate(mathLib::Vec3(-10.0f, 0.0f, 0.0f));

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
		//mesh.draw(&*shaderst, dx, textures);

		uzi.t += dt;
		uzi.draw(&*shaderani, dx, dt, textures);
		
		//animation.t += dt;
		//animation.draw(&*shaderani, dx,dt, textures);


		dx.present();
		
	}
}