/*
This simple macro load all the needed developped classes. This macro must be run after CompileUserClasses.C
Assuming that the source directory is added to the path in the bash script, no need to specify it
*/

//needed root classes
#include <iostream>
#include "TROOT.h"

TString arrayClasses[] = {"EventReader","AlpidePixel","AlpideCluster","AlpideChip","AlpideTelescope","AlpideLadder","TelescopeTrack","TestBeamEvent"};
int numberOfClasses = sizeof( arrayClasses ) / sizeof( arrayClasses[0] );

void LoadUserClasses() {
  for(int iClass=0;iClass<numberOfClasses;iClass++){
    gSystem->Load(Form("%s_cpp.so",arrayClasses[iClass].Data()));
  }
  cout << "Classes are loaded" << endl;
}
