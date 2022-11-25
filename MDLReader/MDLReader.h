#pragma once
#include <fstream>
#include <windows.h> 
#include <winsock.h>
#include "MdlSubObj.h"
#include "MBfD_RGBA.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class MDLReader {

	string filePath;
	float fileVersion;
	int modelCount;
	int boneCount;
	std::filebuf* fileBuffer = new std::filebuf();
	std::vector<string> stringTable = {};
	std::vector<string> materialTable;
	std::vector<MdlSubObj> subModels;
	std::vector<float> mainRoot;

public:

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

	void getSubObjs() {

	}

	void getModelCount() {

	}

	void getMeshNames() {

	}

	void getArmature() {

	}

	void getBoneCount() {

	}

	bool isSkinMesh() {

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

			block.read((char*)&boneIndex, sizeof(short));
			block.read((char*)&parentIndex, sizeof(short));

			//do something with this data
			block.read((char*)&matVecX, sizeof(DWORD));
			block.read((char*)&matVecY, sizeof(DWORD));
			block.read((char*)&matVecZ, sizeof(DWORD));
			block.read((char*)&posVecX, sizeof(DWORD));
			block.read((char*)&posVecY, sizeof(DWORD));
			block.read((char*)&posVecZ, sizeof(DWORD));
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
			materialTable.push_back(stringTable[mtlIndex]);
		}
	}

	void readMBfD(const int blockPos, const int blockSize) {

		int models;
		std::iostream block(fileBuffer);
		block.seekp(blockPos);
		block.read((char*)&models, sizeof(DWORD));
		// for some reason we have to redeclare to a dummy stringTable
		std::vector<string> mainText = this->stringTable; 

		for (int i = 0; i < 1; i++) {

			int modelID;
			int modelFlagA;
			int modelFlagB;
			float modelRootX, modelRootY, modelRootZ;
			int meshVerts;
			int modelDataSets;
			string modelName;

			block.read((char*)&modelName, sizeof(DWORD));
			block.read((char*)&modelFlagA, sizeof(DWORD));
			block.read((char*)&modelFlagB, sizeof(SHORT));

			//disregard block hash
			block.seekp(int(block.tellp()) + 28);
			block.read((char*)&meshVerts, sizeof(DWORD));
			block.read((char*)&modelDataSets, sizeof(DWORD));
			
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
				block.read((char*)&property, sizeof(SHORT));
				dataProperty = mainText[property];

				block.read((char*)&property, sizeof(SHORT));
				dataFormat = mainText[property];

				block.read((char*)&type, sizeof(DWORD));
				dataType = mainText[type];

				//check alignment
				filePos = BinaryUtils::roundUp(block.tellp(),4);
				block.seekp(filePos);

				//get vector set from data block
				char dataBuffer;
				u_long size = MBfD_RGBA::getDataSetSize(meshVerts, dataFormat);
				block.read(&dataBuffer, size);

				std::vector<float> rawData 
					= MBfD_RGBA::convertBinaryDataSet(&dataBuffer, meshVerts, 
						dataType, dataFormat);

				//do-something with data
				block.seekp(filePos + MBfD_RGBA::getDataSetSize(meshVerts, dataFormat));
				cout << dataProperty << endl;
			}




		}

		this->modelCount = models;

	}

	void readLODs(const int blockPos, const int blockSize) {

		cout << "readLODs " << sizeof(blockPos) << endl;

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