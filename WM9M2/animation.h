#pragma once
#include "mesh.h"
#include <iostream>
#include <fstream>
#include <cfloat>

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

	int boneSize() {
		return skeleton.bones.size();
	}

	void calcFrame(std::string name, float t, int& frame, float& interpolationFact) {
		animations[name].calcFrame(t, frame, interpolationFact);
	}

	mathLib::Matrix interpolateBoneToGlobal(std::string name, mathLib::Matrix* matrices, int baseFrame, float interpolationFact, int boneIndex) {
		return animations[name].interpolateBoneToGlobal(matrices, baseFrame, interpolationFact, &skeleton, boneIndex);
	}

	void calcFinalTransforms(mathLib::Matrix* matrices)
	{
		for (int i = 0; i < skeleton.bones.size(); i++)
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
			currentAnimation = name;
			t = 0;
		}
		if (animationFinished() == true) {
			resetAnimationTime();
		}

		int frame = 0;
		float interpolationFact = 0;
		animation->calcFrame(name, t, frame, interpolationFact);

		for (int i = 0; i < animation->boneSize(); i++)
		{
			matrices[i] = animation->interpolateBoneToGlobal(name, matrices, frame, interpolationFact, i);
		}

		animation->calcFinalTransforms(matrices);
	}
};

class LoadAnimation {
public:
	std::vector<Mesh> meshes;
	std::vector<mathLib::Vec3> meshCenters;  // Store the center of each mesh for offset correction
	Animation animation;
	mathLib::Matrix planeWorld;
	mathLib::Matrix vp;
	float t = 0.0f;
	float armScale = 1.0f;
	AnimationInstance instance;
	std::vector<std::string> textureFilenames;

	void Init(DxCore& core, std::string filename, TextureManager& textures) {
		planeWorld.identity();

		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(filename, gemmeshes, gemanimation);

		// Calculate the overall bounding box to find the model center
		mathLib::Vec3 overallMin(FLT_MAX, FLT_MAX, FLT_MAX);
		mathLib::Vec3 overallMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh mesh;
			std::vector<ANIMATED_VERTEX> vertices;

			// Calculate per-mesh bounds
			mathLib::Vec3 meshMin(FLT_MAX, FLT_MAX, FLT_MAX);
			mathLib::Vec3 meshMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));

				// Track bounds
				meshMin.x = min(meshMin.x, v.pos.x);
				meshMin.y = min(meshMin.y, v.pos.y);
				meshMin.z = min(meshMin.z, v.pos.z);
				meshMax.x = max(meshMax.x, v.pos.x);
				meshMax.y = max(meshMax.y, v.pos.y);
				meshMax.z = max(meshMax.z, v.pos.z);

				overallMin.x = min(overallMin.x, v.pos.x);
				overallMin.y = min(overallMin.y, v.pos.y);
				overallMin.z = min(overallMin.z, v.pos.z);
				overallMax.x = max(overallMax.x, v.pos.x);
				overallMax.y = max(overallMax.y, v.pos.y);
				overallMax.z = max(overallMax.z, v.pos.z);

				vertices.push_back(v);
			}

			// Store mesh center for this mesh
			mathLib::Vec3 meshCenter = (meshMin + meshMax) * 0.5f;
			meshCenters.push_back(meshCenter);

			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			textures.loadTexture(gemmeshes[i].material.find("diffuse").getValue(), &core);

			mesh.Init(vertices, gemmeshes[i].indices, core);
			meshes.push_back(mesh);
		}

		// Calculate overall model center
		mathLib::Vec3 modelCenter = (overallMin + overallMax) * 0.5f;

		// Print diagnostic information
		std::cout << "Uzi Model Analysis:" << std::endl;
		std::cout << "Overall bounds: (" << overallMin.x << ", " << overallMin.y << ", " << overallMin.z
			<< ") to (" << overallMax.x << ", " << overallMax.y << ", " << overallMax.z << ")" << std::endl;
		std::cout << "Model center: (" << modelCenter.x << ", " << modelCenter.y << ", " << modelCenter.z << ")" << std::endl;
		for (int i = 0; i < meshCenters.size(); i++) {
			std::cout << "Mesh " << i << " center: (" << meshCenters[i].x << ", " << meshCenters[i].y << ", " << meshCenters[i].z << ")" << std::endl;
		}

		// Load skeleton
		for (int i = 0; i < gemanimation.bones.size(); i++)
		{
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}

		// Load animations
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

	void translate(mathLib::Vec3 v) {
		planeWorld = planeWorld * mathLib::Matrix::translation(v);
	}

	void scale(mathLib::Vec3 v) {
		planeWorld = planeWorld * mathLib::Matrix::scaling(v);
		armScale = v.x;  // Assuming uniform scaling
	}

	// Draw for regular animated models (like TRex)
	void draw(Shader* shader, DxCore& core, float dt, TextureManager& textures, const mathLib::Matrix& VP) {
		instance.update("Run", dt);

		shader->updateConstantVS("Animated", "animatedMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "VP", &VP);
		shader->updateConstantVS("Animated", "animatedMeshBuffer", "bones", instance.matrices);
		shader->apply(core);

		for (int i = 0; i < meshes.size(); ++i) {
			shader->updateTexturePS(core, "tex", textures.find(textureFilenames[i]));
			meshes[i].draw(core);
		}
	}

};