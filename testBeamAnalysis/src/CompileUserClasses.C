/*
This simple macro compile all the needed developped classes and create their xxxxx.so so one does need to compile them in each macro.
TODO (not important since there are not too many classes): The order of the classes in the array below cannot be changed because of the dependencies of the classes. Make the code so that this dependencies can be found and compile the classes accordignly.
*/

//needed root classes
#include <iostream>
#include "TROOT.h"

TString sourceDirectory = "./";
TString arrayClasses[] = {"EventReader","AlpidePixel","AlpideCluster","AlpideChip","TelescopeTrack","AlpideTelescope","AlpideLadder","TestBeamEvent"};
int numberOfClasses = sizeof( arrayClasses ) / sizeof( arrayClasses[0] );
void CompileUserClasses() {
  for(int iClass=0;iClass<numberOfClasses;iClass++){
    gROOT->LoadMacro(Form("%s/%s.cpp+",sourceDirectory.Data(),arrayClasses[iClass].Data()));
  }
  cout << "Classes are compiled" << endl;
}
