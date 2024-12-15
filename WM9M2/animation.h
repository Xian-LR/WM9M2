﻿#pragma once
#include "mesh.h"
#include <iostream>
#include <fstream>

struct Bone
{
	std::string name;
	mathLib::Matrix offset;
	int parentIndex;
};

struct Skeleton
{
	std::vector<Bone> bones;
	mathLib::Matrix globalInverse;
};



struct AnimationFrame
{
	std::vector<mathLib::Vec3> positions;
	std::vector<mathLib::Quaternion> rotations;
	std::vector<mathLib::Vec3> scales;
};

void SaveMatrixToFile(const mathLib::Matrix& matrix, std::string name) {
	std::ofstream debugFile("debug_output.txt", std::ios::app); // 打开文件，追加模式
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

class AnimationSequence
{
public:
	std::vector<AnimationFrame> frames;
	float ticksPerSecond;
	mathLib::Vec3 interpolate(mathLib::Vec3 p1, mathLib::Vec3 p2, float t) {
		return ((p1 * (1.0f - t)) + (p2 * t));
	}
	mathLib::Quaternion interpolate(mathLib::Quaternion q1, mathLib::Quaternion q2, float t) {
		return mathLib::Quaternion::slerp(q1, q2, t);
	}
	float duration() {
		return ((float)frames.size() / ticksPerSecond);
	}

	void calcFrame(float t, int& frame, float& interpolationFact)
	{
		interpolationFact = t * ticksPerSecond;
		frame = (int)floorf(interpolationFact);
		interpolationFact = interpolationFact - (float)frame;
		frame = min(frame, frames.size() - 1);
	}

	int nextFrame(int frame)
	{
		return min(frame + 1, frames.size() - 1);
	}

	mathLib::Matrix interpolateBoneToGlobal(mathLib::Matrix* matrices, int baseFrame, float interpolationFact, Skeleton* skeleton, int boneIndex) {
		int nextFrameIndex = nextFrame(baseFrame);
		mathLib::Matrix scale = mathLib::Matrix::scaling(interpolate(frames[baseFrame].scales[boneIndex], frames[nextFrameIndex].scales[boneIndex], interpolationFact));
		mathLib::Matrix rotation = interpolate(frames[baseFrame].rotations[boneIndex], frames[nextFrameIndex].rotations[boneIndex], interpolationFact).toMatrix();
		mathLib::Matrix translation = mathLib::Matrix::translation(interpolate(frames[baseFrame].positions[boneIndex], frames[nextFrameIndex].positions[boneIndex], interpolationFact));
		mathLib::Matrix local = scale * rotation * translation;
		if (skeleton->bones[boneIndex].parentIndex > -1) {
			mathLib::Matrix global = local * matrices[skeleton->bones[boneIndex].parentIndex];
			return global;
		}
		return local;

	}
};

class Animation
{
public:
	std::map<std::string, AnimationSequence> animations;
	Skeleton skeleton;
	void calcFrame(std::string name, float t, int& frame, float& interpolationFact) {
		animations[name].calcFrame(t, frame, interpolationFact);
	}

	mathLib::Matrix interpolateBoneToGlobal(std::string name, mathLib::Matrix* matrices, int baseFrame, float 						interpolationFact, int boneIndex) {
		return animations[name].interpolateBoneToGlobal(matrices, baseFrame, interpolationFact, &skeleton, boneIndex);
	}

	void calcFinalTransforms(mathLib::Matrix* matrices)
	{
		for (int i = 0; i < 44; i++)
		{
			matrices[i] = skeleton.bones[i].offset * matrices[i] * skeleton.globalInverse;
		}
	}
};

class AnimationInstance
{
public:
	Animation* animation;
	std::string currentAnimation;
	mathLib::Matrix matrices[256];
	float t;

	void resetAnimationTime()
	{
		t = 0;
	}
	bool animationFinished()
	{
		if (t > animation->animations[currentAnimation].duration())
		{
			return true;
		}
		return false;
	}

	void update(std::string name, float dt) {
		if (name == currentAnimation) {
			t += dt;
		}
		else {
			currentAnimation = name;  t = 0;
		}
		if (animationFinished() == true) { resetAnimationTime(); }
		int frame = 0;
		float interpolationFact = 0;
		animation->calcFrame(name, t, frame, interpolationFact);
		for (int i = 0; i < 44; i++)
		{
			matrices[i] = animation->interpolateBoneToGlobal(name, matrices, frame, interpolationFact, i);
		}
		animation->calcFinalTransforms(matrices);

	}

};

class LoadAnimation {
public:
	std::vector<Mesh> meshes;
	Animation animation;
	mathLib::Matrix planeWorld;

	mathLib::Matrix vp;
	float t = 0.0f;
	AnimationInstance instance;

	void Init(DxCore& core, std::string filename) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(filename, gemmeshes, gemanimation);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh mesh;
			std::vector<ANIMATED_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
			}
			mesh.Init(vertices, gemmeshes[i].indices, core);
			meshes.push_back(mesh);
		}

		for (int i = 0; i < gemanimation.bones.size(); i++)
		{
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}

		for (int i = 0; i < gemanimation.animations.size(); i++)
		{
			std::string name = gemanimation.animations[i].name;
			AnimationSequence aseq;
			aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
			for (int n = 0; n < gemanimation.animations[i].frames.size(); n++)
			{
				AnimationFrame frame;
				for (int index = 0; index < gemanimation.animations[i].frames[n].positions.size(); index++)
				{
					mathLib::Vec3 p;
					mathLib::Quaternion q;
					mathLib::Vec3 s;
					memcpy(&p, &gemanimation.animations[i].frames[n].positions[index], sizeof(mathLib::Vec3));
					frame.positions.push_back(p);
					memcpy(&q, &gemanimation.animations[i].frames[n].rotations[index], sizeof(mathLib::Quaternion));
					frame.rotations.push_back(q);
					memcpy(&s, &gemanimation.animations[i].frames[n].scales[index], sizeof(mathLib::Vec3));
					frame.scales.push_back(s);
				}
				aseq.frames.push_back(frame);
			}
			animation.animations.insert({ name, aseq });
		}
		instance.animation = &animation;
	}

	void draw(Shader* shader, DxCore& core, float dt) {
		mathLib::Vec3 from = mathLib::Vec3(11 * cos(t), 5, 11 * sinf(t));
		mathLib::Vec3 to = mathLib::Vec3(0.0f, 0.0f, 0.0f);
		mathLib::Vec3 up = mathLib::Vec3(0.0f, 1.0f, 0.0f);	

		vp = mathLib::lookAt(from, to, up) * mathLib::PerPro(1.f, 1.f, 90.f, 100.f, 0.1f);
		instance.update("Run", dt);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "VP", &vp);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "bones", instance.matrices);
		shader->apply(core);
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].draw(core);
		}
	}


};