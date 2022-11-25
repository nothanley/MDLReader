#pragma once
#include <iostream>
#include <fstream>
using namespace std;

class MDLReader {


	string filePath;
	std::filebuf* fileBuffer = 
		new std::filebuf();

	
	enum {
		MDL,
		TEXT,
		BONE,
		MTL,
		MBfD,
		LODs,
		END,
		ENDM,
	};


public:
	
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
	}


	// void getDummy() {
	//}

	//

	//int getNum() {
	//	return randomNum;
	//}


};