#ifndef MDLSUBOBJ_H
#define MDLSUBOBJ_H

#include "BinaryUtils.h"
#include <vector>
#endif MDLSUBOBJ_H

class MdlSubObj
{

	enum {
		POSITION= 1656822844,
		NORMALS= 2865655510,
		BINORMALS= 2552595965,
		TANGENTS= 2555205862,
		BLENDWEIGHTS= 3362741239,
		BLENDINDICES = 2622613563,
		TEXCOORDS = 680036153,
		COLOR= 229197112
	};

	public:

		int verticeCount;
		int faceCount;
		int uvCount = 0;
		int colorCount = 0;
		bool isSkinned = false;
		bool hasMorphs = false;
		std::string name;
		std::string meshName;

		std::vector<float> position;
		std::vector<float> normals;
		std::vector<float> binormals;
		std::vector<float> tangents;
		std::vector<float> blendWeights;
		std::vector<float> blendIndices;
		std::vector<int> triFaces;
		std::vector< std::vector<float> > uvmap;
		std::vector< std::vector<float> > vcolors;
		std::vector<int> weightedBones;
		int skinInfluence;

		//convert-binary
		void appendBinary(char* dataBuffer, std::vector<float> rawData , std::string dataProperty) {
			unsigned int buf = BinaryUtils::hash(dataProperty.c_str());
			switch (buf){
			case POSITION:
				this->setVertices(rawData);
				break;
			case NORMALS:
				this->setNormals(rawData);
				break;
			case BINORMALS:
				this->setBinormals(rawData);
				break;
			case TANGENTS:
				this->setTangents(rawData);
				break;
			case BLENDWEIGHTS:
				this->setBlendWeights(rawData);
				break;
			case BLENDINDICES:
				this->setBlendIndices(rawData);
				break;
			case TEXCOORDS:
				this->addUVMap(rawData);
				break;
			case COLOR:
				this->addVColors(rawData);
				break;
			default:
				break;
			}
		}

		//obj getters;
		std::vector<float> getVertices() {
			return this->position;
		}
		std::vector<float> getNormals() {
			return this->normals;
		}
		static void getBinormals() {

		}
		static void getTangents() {

		}
		static void getBlendWeights() {

		}
		static void getBlendIndices() {

		}
		std::vector<int> getTriFaces() {
			return this->triFaces;
		}
		std::vector< std::vector<float> > getUVMaps() {
			return this->uvmap;
		}
		std::vector<float> getUVMap( int i ) {
			return this->uvmap[i];
		}
		std::vector< std::vector<float> > getVColorSets() {
			return this->vcolors;
		}
		std::vector<float> getVColorSet( int i ) {
			return this->vcolors[i];
		}

		// obj setters;
		void setVertices(std::vector<float> rawData) {
			this->position = rawData;
			this->verticeCount = (rawData.size() / 3);
		}
		void setSkinInfluence( int numWeights ) {
			this->skinInfluence = numWeights;
		}
		void setFaces(std::vector<int> rawData) {
			this->triFaces = rawData;
			this->faceCount = (rawData.size() / 3);
		}
		void setNormals(std::vector<float> rawData) {
			this->normals = rawData;
		}
		void setBinormals(std::vector<float> rawData) {
			this->binormals = rawData;
		}
		void setTangents(std::vector<float> rawData) {
			this->tangents = rawData;
		}
		void setBlendWeights(std::vector<float> rawData) {
			this->blendWeights = rawData;
			this->isSkinned = true;
		}
		void setBlendIndices(std::vector<float> rawData) {
			this->blendIndices = rawData;
			this->setWeightedBones();
		}
		void addUVMap(std::vector<float> rawData) {
			this->uvmap.push_back(rawData);
			this->uvCount++;
		}
		void addVColors(std::vector<float> rawData) {
			this->vcolors.push_back(rawData);
			this->colorCount++;
		}
		void setModelName(std::string modelName) {
			this->name = modelName;
		}
		void setMeshName(std::string mshName) {
			this->meshName = mshName;
		}

	private:
		void setWeightedBones() {

			//collect unique indices
			for (int i = 0; i < blendIndices.size(); i++) {

				float blendIndex = blendIndices[i];	// use int...

				//if exists
				if (std::find(weightedBones.begin(), weightedBones.end(), blendIndex) == weightedBones.end()) {
					weightedBones.push_back(blendIndex);
				}

			}

			//sort???
		}

};

