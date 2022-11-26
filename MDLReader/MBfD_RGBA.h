
#include <vector>
#include "windows.h"
#include <map>
#include "BinaryUtils.h"
#include <iostream>

class MBfD_RGBA {

public:

	enum {
		R16_G16_B16_A16 = 3631935628,
		R32_G32_B32 = 251819603,
		R8_G8_B8_A8 = 3503152076,
		R32_G32 = 1382734287,
		R8 = 5859695,
	};

	static float bitFloat(float value, std::string signValue) {

		if (signValue == "snorm")
			value = float(value / 127.0);
		else if (signValue == "unorm")
			value = float(value / 255.0);
		return value;
	}

	static float shortFloat(float value, std::string signValue) {
		if (signValue == "snorm")
			value = float(value / 32767.0);
		else if (signValue == "unorm")
			value = float(value / 65535.0);
		return value;
	}

	static std::vector<float> convertBinaryDataSet(char* data, int verts, std::string dataType, std::string blockType) {
		unsigned int buf = BinaryUtils::hash(blockType.c_str());
		std::vector<float> dataSet;

		switch (buf)
		{
		case R16_G16_B16_A16:
			dataSet = getData_R16_G16_B16_A16(data, verts, dataType, blockType);
			break;
		case R32_G32_B32:
			dataSet = getData_R32_G32_B32(data, verts, dataType, blockType);
			break;
		case R8_G8_B8_A8:
			dataSet = getData_R8_G8_B8_A8(data, verts, dataType, blockType);
			break;
		case R32_G32:
			dataSet = getData_R32_G32(data, verts, dataType, blockType);
			break;
		case R8:
			dataSet = getData_R8(data, verts, dataType, blockType);
			break;
		default:
			break;
		}

		return dataSet;

	}

	static int getDataSetSize(int verts, std::string blockType) {

		unsigned int size = verts;
		unsigned int buf = BinaryUtils::hash(blockType.c_str());
		switch(buf)
		{
			case R16_G16_B16_A16:
				size *= 8;
				break;
			case R32_G32_B32:
				size *= 12;
				break;
			case R8_G8_B8_A8:
				size *= 4;
				break;
			case R32_G32:
				size *= 8;
				break;
			case R8:
				size *= 1;
				break;
			default:
				break;
		}

		//check alignment
		size = BinaryUtils::roundUp(size, 4);

		return size;
	}

	static std::vector<float> getData_R32_G32_B32(char* data, int verts, std::string dataType, std::string blockType) {
		std::vector<float> dataSet;

		for (int i = 0; i < verts; i++) {
			float r32, g32, b32;

			memcpy((char*)&r32, (data + (12*i)), sizeof(DWORD));
			memcpy((char*)&g32, (data + (12*i)+4), sizeof(DWORD));
			memcpy((char*)&b32, (data + (12*i)+8), sizeof(DWORD));

			dataSet.push_back( r32 );
			dataSet.push_back( g32 );
			dataSet.push_back( b32 );

		}

		return dataSet;
	}

	static std::vector<float> getData_R8_G8_B8_A8(char* data, int verts, std::string dataType, std::string blockType) {
		std::vector<float> dataSet;

		for (int i = 0; i < verts; i++) {
			float r8f, g8f, b8f, a8f;

			if (dataType == "snorm") {
				int8_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (4 * i) + 0), sizeof(BYTE) );
				memcpy((char*)&g8, (data + (4 * i) + 1), sizeof(BYTE));
				memcpy((char*)&b8, (data + (4 * i) + 2), sizeof(BYTE));
				memcpy((char*)&a8, (data + (4 * i) + 3), sizeof(BYTE));


				r8f = bitFloat(r8, dataType);
				g8f = bitFloat(g8, dataType);
				b8f = bitFloat(b8, dataType);
				a8f = bitFloat(a8, dataType);
			}
			else if (dataType == "unorm") {
				uint8_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (4 * i) + 0), 1);
				memcpy((char*)&g8, (data + (4 * i) + 1), 1);
				memcpy((char*)&b8, (data + (4 * i) + 2), 1);
				memcpy((char*)&a8, (data + (4 * i) + 3), 1);

				r8f = bitFloat(r8, dataType);
				g8f = bitFloat(g8, dataType);
				b8f = bitFloat(b8, dataType);
				a8f = bitFloat(a8, dataType);
			}
			else if (dataType == "sint") {
				int8_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (4 * i) + 0), 1);
				memcpy((char*)&g8, (data + (4 * i) + 1), 1);
				memcpy((char*)&b8, (data + (4 * i) + 2), 1);
				memcpy((char*)&a8, (data + (4 * i) + 3), 1);

				r8f = r8;
				g8f = g8;
				b8f = b8;
				a8f = a8;
			}
			else if (dataType == "uint") {
				uint8_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (4 * i) + 0), 1);
				memcpy((char*)&g8, (data + (4 * i) + 1), 1);
				memcpy((char*)&b8, (data + (4 * i) + 2), 1);
				memcpy((char*)&a8, (data + (4 * i) + 3), 1);

				r8f = r8;
				g8f = g8;
				b8f = b8;
				a8f = a8;
			}


			dataSet.push_back(r8f);
			dataSet.push_back(g8f);
			dataSet.push_back(b8f);
			dataSet.push_back(a8f);
		}

		return dataSet;
	}

	static std::vector<float> getData_R16_G16_B16_A16(char* data, int verts, std::string dataType, std::string blockType) {
		std::vector<float> dataSet;

		for (int i = 0; i < verts; i++) {
			float r8f, g8f, b8f, a8f;

			if (dataType == "snorm") {
				int16_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (8 * i) + 0), 2);
				memcpy((char*)&g8, (data + (8 * i) + 2), 2);
				memcpy((char*)&b8, (data + (8 * i) + 4), 2);
				memcpy((char*)&a8, (data + (8 * i) + 6), 2);

				r8f = shortFloat(r8, dataType);
				g8f = shortFloat(g8, dataType);
				b8f = shortFloat(b8, dataType);
				a8f = shortFloat(a8, dataType);
			}
			else if (dataType == "unorm") {
				uint16_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (8 * i) + 0), 2);
				memcpy((char*)&g8, (data + (8 * i) + 2), 2);
				memcpy((char*)&b8, (data + (8 * i) + 4), 2);
				memcpy((char*)&a8, (data + (8 * i) + 6), 2);

				r8f = shortFloat(r8, dataType);
				g8f = shortFloat(g8, dataType);
				b8f = shortFloat(b8, dataType);
				a8f = shortFloat(a8, dataType);
			}
			else if (dataType == "sint") {
				int16_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (8 * i) + 0), 2);
				memcpy((char*)&g8, (data + (8 * i) + 2), 2);
				memcpy((char*)&b8, (data + (8 * i) + 4), 2);
				memcpy((char*)&a8, (data + (8 * i) + 6), 2);

				r8f = r8;
				g8f = g8;
				b8f = b8;
				a8f = a8;
			}
			else if (dataType == "uint") {
				uint16_t r8, g8, b8, a8;

				memcpy((char*)&r8, (data + (8 * i) + 0), 2);
				memcpy((char*)&g8, (data + (8 * i) + 2), 2);
				memcpy((char*)&b8, (data + (8 * i) + 4), 2);
				memcpy((char*)&a8, (data + (8 * i) + 6), 2);

				r8f = r8;
				g8f = g8;
				b8f = b8;
				a8f = a8;
			}


			dataSet.push_back(r8f);
			dataSet.push_back(g8f);
			dataSet.push_back(b8f);
			dataSet.push_back(a8f);
		}

		return dataSet;
	}

	static std::vector<float> getData_R32_G32(char* data, int verts, std::string dataType, std::string blockType) {
		std::vector<float> dataSet;

		for (int i = 0; i < verts; i++) {
			float r32, g32;

			memcpy((char*)&r32, (data + (8 * i) + 0), 4);
			memcpy((char*)&g32, (data + (8 * i) + 4), 4);

			dataSet.push_back(r32);
			dataSet.push_back(g32);

		}

		return dataSet;
	}

	static std::vector<float> getData_R8(char* data, int verts, std::string dataType, std::string blockType) {
		std::vector<float> dataSet;

		for (int i = 0; i < verts; i++) {
			float r8f;

			if (dataType == "snorm") {
				int8_t r8;

				memcpy((char*)&r8, (data + i), 1);

				r8f = bitFloat(r8, dataType);
			}
			else if (dataType == "unorm") {
				uint8_t r8;

				memcpy((char*)&r8, (data + i), 1);

				r8f = bitFloat(r8, dataType);
			}
			else if (dataType == "sint") {
				int8_t r8;

				memcpy((char*)&r8, (data + i), 1);

				r8f = r8;
			}
			else if (dataType == "uint") {
				uint8_t r8;

				memcpy((char*)&r8, (data + i), 1);

				r8f = r8;
			}


			dataSet.push_back(r8f);
		}

		return dataSet;
	}


};