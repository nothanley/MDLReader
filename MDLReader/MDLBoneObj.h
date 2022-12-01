#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>


class MDLBoneOBJ {


public:

	int id;
	std::string name = "null";
	std::vector<float> translate;
	glm::mat4x4 matrix;
	std::vector<float> head;
	std::vector<float> tail;
	std::vector<float> zAxis;
	bool hasParent = false;
	int parent = -1;


	MDLBoneOBJ() {
		this->id = -1;
	}

	MDLBoneOBJ( int boneID ) {
		this->id = boneID;
	}

	MDLBoneOBJ(int boneID , std::string boneName) {
		this->id = boneID;
		this->name = boneName;
	}

	MDLBoneOBJ(int boneID, std::string boneName, glm::mat4x4 inMat ) {
		this->id = boneID;
		this->name = boneName;
		setMatrix(inMat);
	}

	glm::mat4x4* getMatrix() { return &matrix; }

	std::vector<float>* getTranslate() { return &translate; }

	std::vector<float>* getZaxis() { return &zAxis; }

	int getParent() {
		if (hasParent) {
			return this->parent;
		}

		return NULL;
	}

	int getID() { return this->id; }

	void setHead(std::vector<float> boneHead ){
		this->head = boneHead;
	}

	void setTail(std::vector<float> boneTail ) {
		this->tail = boneTail;
	}

	void setMatrix(glm::mat4x4 &inMat ) {
		this->matrix = inMat;
		this->setTranslate( { inMat[3][0], inMat[3][1], inMat[3][2] } );
		this->setZaxis( { inMat[2][0], inMat[2][1], inMat[2][2] } );
		this->setHead(translate);
		this->setTail(  { translate[0] +.001f , translate[1] + .001f , translate[2] + .001f } );
	}

	void setName( std::string boneName ) {
		this->name = boneName;
	}

	void setParent(int boneObj) {
		this->parent = boneObj;
		this->hasParent = true;
	}

	void setZaxis(std::vector<float> newZ ) {
		this->zAxis = newZ;
		matrix[2][0] = newZ[0];
		matrix[2][1] = newZ[1]; 
		matrix[2][2] = newZ[2];
	}

	void setTranslate( std::vector<float> position ) {
		this->translate = position;
		matrix[3][0] = position[0];
		matrix[3][1] = position[1];
		matrix[3][2] = position[2];
		this->setHead(translate);
	}

	void setID( int boneID ) {
		this->id = boneID;
	}


	static glm::mat4x4 dot_4x4(glm::mat4x4 a, glm::mat4x4 b) {

		glm::mat4x4 c;


		for (int i = 0; i < 4; i++){

			for (int j = 0; j < 4; j++){
				float num = 0;

				for (int k = 0; k < 4; k++) {
					num += a[i][k] * b[k][j];
				}

				c[i][j] = num;
			}

		}


		return c;
	}

	static glm::mat4x4 euler2rot(float x, float y, float z) {

		glm::mat4 transformX = glm::eulerAngleX(x);
		glm::mat4 transformY = glm::eulerAngleY(y);
		glm::mat4 transformZ = glm::eulerAngleZ(z);

		glm::mat4 transform1 =
			transformZ * transformY * transformX; // or some other order

		return transform1;
		
	}

};