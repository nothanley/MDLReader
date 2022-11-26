
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


}