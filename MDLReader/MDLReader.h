
#include <fstream>
#include <windows.h> 
#include <winsock.h>
#include "MdlSubObj.h"
#include "MBfD_RGBA.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "MDLBoneObj.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma once
using namespace std;
using namespace glm;

class MDLReader {

	string filePath;
	float fileVersion;
	int modelCount;
	int boneCount;
	std::filebuf* fileBuffer = new std::filebuf();
	std::vector<int> materialTable;
	std::vector<float> mainRoot;
	double Pi = 3.14159265358979323846264338327950288;
	double toDegrees = 180 / Pi;
	double toRadians = Pi / 180;

public:
	std::vector<string> stringTable = {};
	std::vector<MDLBoneOBJ> bones;
	std::vector<MdlSubObj> subModels;

	enum {
		MDL = 0x4D444C21,
		TEXT = 0x54455854,
		BONE = 0x424F4E45,
		AtPt = 0x41745074,
		MTL = 0x4D544C21,
		MBfD = 0x4D426644,
		LODs = 0x4C4F4473,
		ENDP = 0x454E4421,
		ENDM = 0x454E444D,
	};
	
	struct meshSkin {

		std::vector<float> indices; //this should really just be uint16
		std::vector<float> weights;
		
	};

	MDLReader() {

		//init random number
		//randomNum = rand();

	}

	void openFile(const char* FilePath) {

		//verifies if file is readable
		ifstream inFile(FilePath);

		if (inFile.good())
			fileBuffer->open(FilePath, ios::in | ios::binary);
		else
			throw("Cannot Open File.");

		//store filepath to object
		filePath = FilePath;
		printf("Opening File: %s\n", FilePath);
		validateMDL();

		//reset-seek
		fileBuffer->pubseekpos(ios_base::beg);

		//reads through container data
		processBlocks();
	}

	std::vector<MdlSubObj> getSubObjs() {
		return subModels;
	}

	int getModelCount() {
		return modelCount;
	}

	std::vector<std::string> getMeshNames() {
		std::vector<std::string> meshNames;
		for (int i = 0; i < modelCount; i++)
			meshNames.push_back(subModels[i].name);
		return meshNames;
	}

	std::vector<MDLBoneOBJ> getArmature() {
		return bones;
	}

	int getBoneCount() {
		return bones.size();
	}

	bool isSkinMesh() {
		return (bones.size() > 0);
	}

	bool hasFaceMorphs() {

	}


private:

	void processBlocks() {

		DWORD blockStart;
		DWORD blockType;
		DWORD blockSize;
		std::iostream fs(fileBuffer);

		fs.read((char*)&blockType, sizeof(DWORD));
		fs.read((char*)&blockSize, sizeof(DWORD));
		blockStart = fs.tellp();

		//allocate-buffer
		char* blockBuffer = (char*)malloc(blockSize);
		fs.read(blockBuffer, blockSize );

		//Block Processing
		switch (ntohl(blockType)) {
			case MDL:
				readMDL(blockStart, blockSize);
				blockStart = 0;
				break;
			case TEXT:
				readTEXT(blockStart, blockSize);
				break;
			case BONE:
				readBONE(blockStart, blockSize);
				break;
			case AtPt:
				break;
			case MTL:
				readMTL(blockStart, blockSize);
				break;
			case MBfD:
				readMBfD(blockStart, blockSize);
				break;
			case LODs:
				readLODs(blockStart, blockSize);
				break;
			default:
				break;
		}


		//recurse-blocks
		fs.seekp(DWORD64(blockStart)+DWORD64(blockSize));
		if (fs.tellp() > (ios_base::end - ios_base::beg ) )
			processBlocks();

	}

	void readMDL( const int blockPos , DWORD &blockSize) {
		(int*)blockSize--;
		cout << "readMDL " << sizeof(blockPos) << endl;

	}

	void readTEXT( const int blockPos , const int blockSize) {

		DWORD stringCount;
		std::iostream block( fileBuffer );
		
		block.seekp(blockPos);
		block.read((char*)&stringCount, sizeof(DWORD));

		cout << "String Count: " << stringCount << endl;

		//collect strings
		for (int i = 1; i <= stringCount; i++) {
			DWORD strPos;
			char stringEntry[256];
			block.seekp(blockPos+ (4 * i));
			block.read((char*)&strPos, sizeof(DWORD));
			block.seekp(blockPos + 4 + (stringCount * 4) + strPos);
			block.getline(stringEntry, 256, '\0');
			stringTable.push_back(stringEntry);
		}

	}

	void readBONE(const int blockPos, const int blockSize) {

		DWORD boneCount, blockFlag1, blockFlag2;
		std::iostream block(fileBuffer);
		block.seekp(blockPos);
		block.read((char*)&blockFlag1, sizeof(DWORD));
		block.read((char*)&boneCount, sizeof(DWORD));
		block.read((char*)&blockFlag2, sizeof(DWORD));
		boneCount = (blockSize - 12) / 32;

		for (int i = 0; i < boneCount; i++) {
			string boneID, parentID;
			short boneIndex, parentIndex;
			float matVecX , matVecY , matVecZ;
			float posVecX , posVecY , posVecZ , null;
			glm::mat3x3 rotMatrix;
			bool isIKSolver = false;

			block.read((char*)&boneIndex, sizeof(short));
			block.read((char*)&parentIndex, sizeof(short));

			//bone transform matrix local space
			block.read((char*)&posVecX, sizeof(DWORD));
			block.read((char*)&posVecY, sizeof(DWORD));
			block.read((char*)&posVecZ, sizeof(DWORD));
			block.read((char*)&matVecX, sizeof(DWORD));
			block.read((char*)&matVecY, sizeof(DWORD));
			block.read((char*)&matVecZ, sizeof(DWORD));
			block.read((char*)&null, sizeof(DWORD));

			if (boneIndex == 0 && parentIndex == 0) { isIKSolver = true; } //Fixes rig, but is hacky

			glm::mat4x4 newMat = MDLBoneOBJ::euler2rot(matVecX, matVecY, matVecZ);
			newMat[0][3] = 0.0f; newMat[1][3] = 0.0f; newMat[2][3] = 0.0f; newMat[3][3] = 1.0f;
			newMat[3][0] = posVecX; newMat[3][1] = posVecY; newMat[3][2] = posVecZ; newMat[3][3] = 1.0f;

			//store boneObj
				boneID = stringTable[boneIndex];
				MDLBoneOBJ newBone = MDLBoneOBJ(bones.size(), boneID, newMat);

				if (parentIndex != -1) {
					parentIndex = findBoneIndex(this->bones, stringTable[parentIndex]);
					newBone.setParent(parentIndex);
				}

			//Append
			if (!isIKSolver)
				bones.push_back(newBone);
		}

		this->boneCount = boneCount;
	}

	static int findBoneIndex(std::vector<MDLBoneOBJ> boneList , std::string boneName) {

		for (auto &bone : boneList) {
			if (bone.name == boneName) {
				return bone.id;
			}
		}

		return -1;
	}

	void readMTL(const int blockPos, const int blockSize) {

		int mtlCount;
		std::iostream block(fileBuffer);
		block.seekp(blockPos);
		block.read((char*)&mtlCount, sizeof(DWORD));

		for (int i = 0; i < mtlCount; i++) {
			int mtlIndex;

			block.read((char*)&mtlIndex, sizeof(DWORD));
			materialTable.push_back(mtlIndex);
		}
	}

	void readMBfD(const int blockPos, const int blockSize) {

		int models;
		std::iostream block(fileBuffer);
		block.seekp(blockPos);
		block.read((char*)&models, sizeof(DWORD));

		// for some reason we have to redeclare to a dummy stringTable
		std::vector<string> mainText = this->stringTable; 

		for (int i = 0; i < models; i++) {

			int modelID;
			int modelFlagA;
			int modelFlagB;
			float modelRootX, modelRootY, modelRootZ;
			int meshVerts;
			int modelDataSets;
			string modelName;

			MdlSubObj modelObject;

			block.read((char*)&modelID, sizeof(DWORD));
			block.read((char*)&modelFlagA, sizeof(DWORD));
			block.read((char*)&modelFlagB, sizeof(SHORT));

			//disregard block hash
			block.seekp(int(block.tellp()) + 28);
			block.read((char*)&meshVerts, sizeof(DWORD));
			block.read((char*)&modelDataSets, sizeof(DWORD));


			modelName = mainText[modelID];
			modelObject.setModelName(modelName);

			//iter over current model's subdata
			for (int k = 0; k < modelDataSets; k++) {
				string dataProperty;
				string dataFormat;
				string dataType;
				DWORD type;
				SHORT property;
				int filePos = block.tellp();

				//gather dataSet properties
				block.seekp(filePos + 12);
				block.read((char*)&property, 2);
				dataProperty = mainText[property];

				block.read((char*)&property, 2);
				dataFormat = mainText[property];

				block.read((char*)&type, 4);
				dataType = mainText[type];

				//check alignment
				filePos = BinaryUtils::roundUp(block.tellp(),4);
				block.seekp(filePos);

				//get vector set from data block
				int size = MBfD_RGBA::getDataSetSize(meshVerts, dataFormat);
				char* dataBuffer = (char*)malloc(size);
				block.read(dataBuffer, std::streamsize(size) );

				std::vector<float> rawData 
					= MBfD_RGBA::convertBinaryDataSet(dataBuffer, meshVerts,
						dataType, dataFormat);

				//do-something with data
				block.seekp(filePos + MBfD_RGBA::getDataSetSize(meshVerts, dataFormat));

				modelObject.appendBinary(dataBuffer, rawData, dataProperty);
				delete [] dataBuffer;
			}

			//New Skin Buffer
			if (modelObject.name == "M_Head") {
				cout << "Hi";
			}
			SHORT maxJointIndex; block.read((char*)&maxJointIndex, 2);
			SHORT skinInfluence; block.read((char*)&skinInfluence, 2);
			if (skinInfluence!=0){
				DWORD skinBfSize = ((skinInfluence*2) * meshVerts) + (skinInfluence * meshVerts);
				char* dataBuffer = (char*)malloc(skinBfSize);
				block.read(dataBuffer, std::streamsize(skinBfSize));
				meshSkin skin = readSkin(dataBuffer, skinInfluence, meshVerts);
				modelObject.setBlendWeights(skin.weights);
				modelObject.setBlendIndices(skin.indices);
				delete[] dataBuffer;
			}
			modelObject.setSkinInfluence(skinInfluence);

			//	vertex-remapping
			char* dataBuffer = (char*)malloc(meshVerts * 4);
			block.read(dataBuffer, std::streamsize(meshVerts * 4));
			std::vector<uint32_t> vertexMap = collectRemap(dataBuffer, meshVerts);
			//std::vector<float> newVerts;
			//for (int w = 0; w < vertexMap.size(); w++) {
			//	uint32_t index = vertexMap[w];
			//	newVerts.push_back(modelObject.getVertices().at(index));
			//}
			//modelObject.setVertices(newVerts);

			//	FaceMorphs tbd...
			DWORD morphCount;
			block.read((char*)&morphCount, 4);

			// Skip to Next
			uint32_t filePos;
			DWORD charBuf = 0;
			while (ntohl(charBuf) != ENDM) {
				block.read((char*)&charBuf, 4);}
			filePos = block.tellp();
			block.seekp(filePos + 4);

			//append model
			subModels.push_back(modelObject);

		}

		this->modelCount = models;

	}

	std::vector<uint32_t> collectRemap(const char* remapBuffer, int meshVerts) {

		//iter and collect vertInds
		std::vector<uint32_t> remapVector;

		for (int i = 0; i < meshVerts; i++) {
			DWORD index;
			memcpy((char*)&index, (remapBuffer+4*i), sizeof(DWORD) );
			remapVector.push_back(index);
		}

		return remapVector;
	}


	meshSkin readSkin(const char* skinBuffer, int skinInfluence, int meshVerts) {
		
		std::vector<float> indices;
		std::vector<float> weights;
		DWORD bufferTell = (skinInfluence * 2 * meshVerts);

		//iter and collect blendindices
		for (int i = 0; i < meshVerts; i++) {
			for (int j = 0; j < skinInfluence; j++) {
				uint16_t index;
				const void* offset = skinBuffer + (i * skinInfluence * 2) + (2 * j);
				memcpy((char*)&index, offset, 2);
				indices.push_back((float)index);
			}
		}

		//iter and collect blendweights
		for (int i = 0; i < meshVerts; i++) {
			for (int j = 0; j < skinInfluence; j++) {
				uint8_t snormWeight;
				const void* offset = (skinBuffer + bufferTell + (i * skinInfluence) + j);
				memcpy((char*)&snormWeight, offset, 1);
				weights.push_back(float(snormWeight / 255.0));
			}
		}

		meshSkin skin; skin.indices = indices; skin.weights = weights;
		return skin;
	};

	void readLODs(const int blockPos, const int blockSize) {

		cout << "readLODs " << sizeof(blockPos) << endl;
		int subLods;
		std::iostream block(fileBuffer);
		block.seekp(blockPos);
		block.read((char*)&subLods, sizeof(DWORD));


		cout << "subLods " << subLods << endl;
		subLods =  /* We will only collect the highest LODs */ subLods;

		for (int k = 0; k < subLods; k++) {

			//cout << "POS LODS " << k << " @: " << block.tellp() << endl;
			DWORD lodSets;
			block.read((char*)&lodSets, sizeof(DWORD));

			for (int i = 0; i < lodSets; i++) {
	
				SHORT lodIndex;
				DWORD faceCount;
				std::vector<int> triFaces;

				block.read((char*)&lodIndex, 2);
				block.read((char*)&faceCount, sizeof(DWORD));
				int modelVerts = subModels[i].verticeCount;

				if (modelVerts > 65535) {
					for (int j = 0; j < int(faceCount / 3); j++) {
						uint32_t fx, fy, fz;
						block.read((char*)&fx, 4);
						block.read((char*)&fy, 4);
						block.read((char*)&fz, 4);

						triFaces.push_back(fx);
						triFaces.push_back(fy);
						triFaces.push_back(fz);
					}
				}
				else {
					for (int j = 0; j < int(faceCount / 3); j++) {
						uint16_t fx, fy, fz;
						block.read((char*)&fx, 2);
						block.read((char*)&fy, 2);
						block.read((char*)&fz, 2);

						triFaces.push_back(fx);
						triFaces.push_back(fy);
						triFaces.push_back(fz);
					}
				}


				//cout << "POS: " << block.tellp();
				//Assign mats
				string meshID;
				DWORD lodEntries;
				string shapeName = subModels[i].name;
				block.seekp(BinaryUtils::roundUp(block.tellp(), 4));
				block.read((char*)&lodEntries, 4);

				/* Seperate faces can have their own material */
				for (int p = 0; p < lodEntries; p++) {
					DWORD mtlIndex;
					DWORD affectedFaceIndexFirst;
					DWORD affectedFaceIndexSize;

					block.read((char*)&mtlIndex, 4);
					block.read((char*)&affectedFaceIndexFirst, 4);
					block.read((char*)&affectedFaceIndexSize, 4);

					meshID = stringTable[materialTable[mtlIndex]];
				}

				/* Assign only last mtl entry */
				if (k == 0) {
					subModels[i].setFaces(triFaces);
					subModels[i].setMeshName(meshID);
				}

				// Skip to Next
				uint64_t endBuffer;
				block.read((char*)&endBuffer, 8);

			}


		}
	}

	void validateMDL() {

		DWORD magicSig;
		std::iostream fs(fileBuffer);

		//seeks magic and validates
		fileBuffer->pubseekpos(ios_base::beg);
		fs.read( (char*)&magicSig , sizeof(DWORD) );

		//validate type and version
		if ( ntohl(magicSig) == MDL ) {
			fs.read((char*)&magicSig, sizeof(DWORD));
			fileVersion = float(magicSig / 16);
			cout << "MDL Version: " << fileVersion << endl;

			//only supports 2022 MDL containers
			switch (int(fileVersion)) {
				case 2:
					cout << "Verified MDL container." << endl;
					break;
				default:
					throw("Unsupported MDL version.");
			}
		}
		else {
			throw("File is not a valid MDL container.");
		}

	}



};