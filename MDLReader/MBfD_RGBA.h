#pragma once
#include <vector>

class MBfD_RGBA {



public:

	enum {
		R16_G16_B16_A16,
		R32_G32_B32,
		R8_G8_B8_A8,
		R32_G32,
		R8,
	};

	static std::vector<float> getData_R32_G32_B32(char* data, int verts, int blockType) {

	}

	static std::vector<float> getData_R8_G8_B8_A8(char* data, int verts, int blockType) {

	}

	static std::vector<float> getData_R16_G16_B16_A16(char* data, int verts, int blockType) {

	}

	static std::vector<float> getData_R32_G32(char* data, int verts, int blockType) {

	}

	static std::vector<float> getData_R8(char* data, int verts, int blockType) {

	}


};