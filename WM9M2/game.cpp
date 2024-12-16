#include "window.h"
#include "animation.h"
#include "GamesEngineeringBase.h"
#include "camera.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window canvas;
	DxCore dx;
	ShaderManager shaders;
	LoadMesh mesh;
	LoadAnimation uzi;
	LoadAnimation animation;
	GamesEngineeringBase::Timer tim;
	float dt;
	TextureManager textures;
	Camera camera(mathLib::Vec3(0.0f, 5.0f, 15.0f)); 

	//ConstantBuffer* constBufferCPU = new ConstantBuffer();
	//constBufferCPU->time = 0;

	std::string vs2 = "Resources/vshader_ani.txt";
	std::string vs1 = "Resources/vsshader.txt";
	std::string ps = "Resources/psshader.txt";
	std::string shader1 = "Shader1";
	std::string shader2 = "Shader2";
	std::string shader3 = "Shader3";

	std::string model1 = "Models/pine.gem";
	std::string model2 = "Models/TRex.gem";
	std::string model3 = "Models/Uzi.gem";

	canvas.Init("MyWindow", 1024, 768);
	dx.Init(1024, 768, canvas.hwnd);



	shaders.load(shader1, vs1, ps, dx,1);
	shaders.load(shader2, vs2, ps, dx, 0);
	shaders.load(shader3, vs2, ps, dx, 0);

	Shader* shaderst = shaders.getShader(shader1);
	Shader* shaderani = shaders.getShader(shader2);
	Shader* shaderarm = shaders.getShader(shader3);

	mesh.Init(dx, model1, textures);
	animation.Init(dx, model2, textures);
	uzi.Init(dx, model3, textures);

	mesh.scale(mathLib::Vec3(0.025f, 0.025f, 0.025f));
	mesh.translate(mathLib::Vec3(10.0f, 0.0f, 0.0f));
	animation.translate(mathLib::Vec3(0.0f, 0.0f, 0.0f));
	animation.scale(mathLib::Vec3(1.5f, 1.5f, 1.5f));
	uzi.scale(mathLib::Vec3(0.25f, 0.25f, 0.25f));

	POINT mousePos;
	GetCursorPos(&mousePos);
	static int lastX = mousePos.x, lastY = mousePos.y;

	// ...
	while (true) {
	
		dx.clear();
		dt = tim.dt();
		//constBufferCPU->time += dt;
		canvas.processMessages();
		
		//Camera
		bool moveForward = GetAsyncKeyState('W') & 0x8000;
		bool moveBackward = GetAsyncKeyState('S') & 0x8000;
		bool moveLeft = GetAsyncKeyState('A') & 0x8000;
		bool moveRight = GetAsyncKeyState('D') & 0x8000;

		//mathLib::Matrix viewMatrix = camera.getViewMatrix();
		//mathLib::Matrix vp = viewMatrix * mathLib::PerPro(1.0f, 1.0f, 90.0f, 300.0f, 0.1f);
		//shaderani->updateConstantVS("Animated", "animatedMeshBuffer", "VP", &vp);

		GetCursorPos(&mousePos);
		float xOffset = mousePos.x - lastX;
		float yOffset = lastY - mousePos.y; 
		lastX = mousePos.x;
		lastY = mousePos.y;

		camera.processKeyboard(dt, moveForward, moveBackward, moveLeft, moveRight);
		camera.processMouseMovement(-xOffset, yOffset);

		mathLib::Matrix viewMatrix = camera.getViewMatrix();
		mathLib::Matrix vp = viewMatrix * mathLib::PerPro(1.0f, 1.0f, 90.0f, 300.0f, 0.1f);
		shaderani->updateConstantVS("Animated", "animatedMeshBuffer", "VP", &vp);
		shaderst->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vp);
		shaderarm->updateConstantVS("Animated", "animatedMeshBuffer", "VP", &vp);

		//Meshes

		mesh.t += dt;
		mesh.draw(&*shaderst, dx, textures);

		uzi.t += dt;
		uzi.drawArm(&*shaderarm, dx, dt, textures, camera);
		
		animation.t += dt;
		animation.draw(&*shaderani, dx,dt, textures);


		dx.present();
		
	}
}