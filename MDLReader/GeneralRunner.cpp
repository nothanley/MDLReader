
#include <iostream>
#include "MDLReader.h"
using namespace std;

int main()
{

    MDLReader newFile = MDLReader();
    MDLReader secondFile = MDLReader();

    newFile.openFile( "C:\\Users\\wauke\\Desktop\\baseModel_jh.mdl" );

    cout << "\n\n";
    int mdlModels = newFile.getModelCount();
    for (int i = 0; i < newFile.getModelCount(); i++) {
        MdlSubObj mesh = newFile.subModels[i];
        cout << mesh.name << " : Vertices=" << mesh.verticeCount << " Faces=" << mesh.faceCount << endl;
    }

    /*glm::mat4x4 orignMat;

    orignMat[0][0] = 0.956305f;
    orignMat[0][1] = 0.0f;
    orignMat[0][2] = -0.29372f;
    orignMat[0][3] = 0.0f;

    orignMat[1][0] = 0.0;
    orignMat[1][1] = 1.0f;
    orignMat[1][2] = 0.0f;
    orignMat[1][3] = 0.0f;

    orignMat[2][0] = 0.29372f;
    orignMat[2][1] = 0.0f;
    orignMat[2][2] = 0.956305f;
    orignMat[2][3] = 0.0f;

    orignMat[3][0] = 13.5353f;
    orignMat[3][1] = -0.00766f;
    orignMat[3][2] = -0.007431f;
    orignMat[3][3] = 1.0f;

    glm::mat4x4 parMat;

    parMat[0][0] = 0.0f;
    parMat[0][1] = -0.956305f;
    parMat[0][2] = -0.29372f;
    parMat[0][3] = 0.0f;

    parMat[1][0] = -1.0f;
    parMat[1][1] = 0.0f;
    parMat[1][2] = 0.0f;
    parMat[1][3] = 0.0f;

    parMat[2][0] = 0.0f;
    parMat[2][1] = 0.29372f;
    parMat[2][2] = -0.956305f;
    parMat[2][3] = 0.0f;

    parMat[3][0] = 0.00115129f;
    parMat[3][1] = -164.692f;
    parMat[3][2] = 1.87766f;
    parMat[3][3] = 1.0f;

    glm::mat4x4 outMat;

    outMat = MDLBoneOBJ::dot_4x4(orignMat, parMat);

    cout << "hi";*/
}