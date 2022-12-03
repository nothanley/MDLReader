
#include <iostream>
#include "MDLReader.h"
using namespace std;

int main()
{

    MDLReader newFile = MDLReader();
    MDLReader secondFile = MDLReader();

    newFile.openFile( "C:\\Users\\wauke\\Desktop\\Turnbuckle\\2k22Backstage.mdl" );

    cout << "\n\n";
    int mdlModels = newFile.getModelCount();
    for (int i = 0; i < newFile.getModelCount(); i++) {
        MdlSubObj mesh = newFile.subModels[i];
        cout << mesh.name << " : Alias=" << mesh.meshName << " Vertices = " << mesh.verticeCount << " Faces = " << mesh.faceCount << endl;
    }

    cout << "\n\n";
    int as = newFile.getModelCount();

}