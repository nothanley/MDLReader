
#include <iostream>
#include "MDLReader.h"
using namespace std;

int main()
{



    MDLReader newFile = MDLReader();
    MDLReader secondFile = MDLReader();

    newFile.openFile( "C:\\Users\\wauke\\Desktop\\baseModel_jsh.mdl" );
    //MDLReader secondFile = MDLReader();
    //secondFile.openFile("C:\\.GamingRoot");
        

    //std::cout << newFile.getNum() << endl;
    //std::cout << secondFile.getNum() << endl;
}