#include "mathLib.h"



namespace mathLib {
	mathLib::Matrix PerPro(float height, float width, float radians, float Far, float Near) {
		mathLib::Matrix PerPro;
		float aspectRatio = width / height;
		float fovScale = 1.0f / tan(radians / 2.0f);

		PerPro.m[0] = fovScale / aspectRatio;
		PerPro.m[1] = 0;
		PerPro.m[2] = 0;
		PerPro.m[3] = 0;
		PerPro.m[4] = 0;
		PerPro.m[5] = fovScale;
		PerPro.m[6] = 0;
		PerPro.m[7] = 0;
		PerPro.m[8] = 0;
		PerPro.m[9] = 0;
		PerPro.m[10] = -(Far + Near) / (Far - Near);
		PerPro.m[11] = -(2 * Far * Near) / (Far - Near);
		PerPro.m[12] = 0;
		PerPro.m[13] = 0;
		PerPro.m[14] = -1;
		PerPro.m[15] = 0;
		return PerPro;
	}
}