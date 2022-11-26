
#include <fstream>
#include <windows.h> 
#include <winsock.h>
#include "MdlSubObj.h"
#include "MBfD_RGBA.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
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
	std::vector<string> stringTable = {};
	std::vector<int> materialTable;
	std::vector<float> mainRoot;
	std::vector<glm::mat4x4> matrixPallete;
	double Pi = 3.14159265358979323846264338327950288;
	double toDegrees = 180 / Pi;
	double toRadians = Pi / 180;

public:

	std::vector<MdlSubObj> subModels;

	enum {
		MDL = 0x4D444C21,
		TEXT = 0x54455854,
		BONE = 0x424F4E45,
		AtPt = 0x41745074,
		MTL = 0x4D544C21,
		MBfD = 0x4D426644,
		LODs = 0x4C4F4473,
		END = 0x454E4421,
		ENDM = 0x454E444D,
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

	std::vector<glm::mat4x4> getArmature() {
		return matrixPallete;
	}

	int getBoneCount() {
		return matrixPallete.size();
	}

	bool isSkinMesh() {
		return (matrixPallete.size() > 0);
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

	void readMDL( const int blockPos , const int blockSize) {

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

		DWORD boneCount;
		std::iostream block(fileBuffer);
		block.seekp(blockPos);
		block.read((char*)&boneCount, sizeof(DWORD));

		for (int i = 0; i < boneCount; i++) {
			string boneID, parentID;
			short boneIndex, parentIndex;
			float matVecX , matVecY , matVecZ;
			float posVecX , posVecY , posVecZ;
			glm::mat3x3 rotMatrix;

			block.read((char*)&boneIndex, sizeof(short));
			block.read((char*)&parentIndex, sizeof(short));

			//do something with this data
			block.read((char*)&posVecX, sizeof(DWORD));
			block.read((char*)&posVecY, sizeof(DWORD));
			block.read((char*)&posVecZ, sizeof(DWORD));
			block.read((char*)&matVecX, sizeof(DWORD));
			block.read((char*)&matVecY, sizeof(DWORD));
			block.read((char*)&matVecZ, sizeof(DWORD));

			//grab Mat
			boneID = stringTable[boneIndex];
			glm::mat4x4 newMat = glm::eulerAngleXYZ(matVecX, matVecY, matVecZ);
			newMat[3][0] = posVecX; newMat[3][1] = posVecY; newMat[3][2] = posVecZ;


			//cache-matrix
			matrixPallete.push_back(newMat);
		}

		this->boneCount = boneCount;

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


			//	vertex-remapping (Not using this currently)
			int filePos = block.tellp();
			filePos += (meshVerts * 4);
			block.seekp(filePos);

			//	FaceMorphs tbd...
			DWORD morphCount;
			block.read((char*)&morphCount, 4);

			// Skip to Next
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

	void readLODs(const int blockPos, const int blockSize) {

		cout << "readLODs " << sizeof(blockPos) << endl;
		int subLods;
		std::iostream block(fileBuffer);
		block.seekp(blockPos);
		block.read((char*)&subLods, sizeof(DWORD));

		for (int i = 0; i < this->modelCount; i++) {
			DWORD lodCount;
			SHORT lodIndex;
			DWORD faceCount;
			std::vector<int> triFaces;
			

			block.read((char*)&lodCount, sizeof(DWORD));
			block.read((char*)&lodIndex, 2);
			block.read((char*)&faceCount, sizeof(DWORD));

			//do-get-verts func here

			if ((faceCount / 3) > 65535) {
				for (int j = 0; j < int(faceCount / 3); j++) {
					int fx, fy, fz;
					block.read((char*)&fx,4);
					block.read((char*)&fy,4);
					block.read((char*)&fz,4);

					triFaces.push_back(fx);
					triFaces.push_back(fy);
					triFaces.push_back(fz);
				}
			}
			else {
				for (int j = 0; j < int(faceCount / 3); j++) {
					int fx, fy, fz;
					block.read((char*)&fx, 2);
					block.read((char*)&fy, 2);
					block.read((char*)&fz, 2);

					triFaces.push_back(fx);
					triFaces.push_back(fy);
					triFaces.push_back(fz);
				}
			}

			DWORD modelIndex;
			string meshID;

			int filePos = block.tellp();
			block.seekp(filePos + 6);
			block.read((char*)&modelIndex, 4);
			meshID = stringTable[materialTable[modelIndex]];
			block.seekp(filePos + 10);

			subModels[i].setFaces(triFaces);

			int fileAlign = BinaryUtils::roundUp(block.tellp(),4);
			block.seekp(fileAlign);

			// Skip to Next
			DWORD charBuf = 0;
			while (ntohl(charBuf) != ENDM) {
				block.read((char*)&charBuf, 4);
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