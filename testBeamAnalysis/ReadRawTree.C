
//needed root classes
#include "Riostream.h"
#include <TTree.h>
#include <TFile.h>
#include "TLeaf.h"
#include "TBranch.h"
#include "TChain.h"
#include "TROOT.h"
#include "TH1.h"
#include "TEventList.h"
#include "TDirectory.h"
#include "TEntryList.h"

//needed macros
#include "Common.C"

//needed user defined classes
#include "EventReader.hpp"
#include "TestBeamEvent.hpp"
#include "AlpideLadder.hpp"
#include "AlpideChip.hpp"
#include "AlpideTelescope.hpp"

Int_t arrayBadPixels[][4] = {
  {0,2,244,726},
  {0,8,201,191},
  {3010,6,8,962},
  {3010,7,214,225},
  {3010,7,380,608},
  {3010,7,289,822},
  {3025,6,150,221},
  {3025,6,191,917},
  {3025,6,216,444},
  {3025,6,219,90},
  {3025,6,325,712},
  {3025,6,356,666},
  {3025,8,353,201},
  {3025,6,187,927},
  {3025,7,-1,36},
  {3025,7,-1,37},
  {3025,8,206,908},
};
Int_t numberOfBadPixels = sizeof( arrayBadPixels ) / sizeof( arrayBadPixels[0] );
Bool_t IsThisABadPixel(Int_t deviceID, Int_t chipID, Int_t row, Int_t col){
  for(Int_t iBadPixel=0;iBadPixel<numberOfBadPixels;iBadPixel++){
    if(deviceID == arrayBadPixels[iBadPixel][0]){
      if(chipID == arrayBadPixels[iBadPixel][1]){
        if( (row == arrayBadPixels[iBadPixel][2]) || (arrayBadPixels[iBadPixel][2] == -1) ){
          if(col == arrayBadPixels[iBadPixel][3]){
            return kTRUE;
          }
        }
      }
    }
  }
  return kFALSE;
}

using namespace std;
//_______________________________________________________________________________________________
void ReadRawTree(TString runName, TString outputFileName, TString outputDirectory)
{

  //-----------------------------------------------------------------------------------------------------------------//
  //Configure the ladders geometry
  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    cLadderID[iLadder] = cLadderID_All[cParticipatedLadders[iLadder]];
    cNumberOfChipsInLadder[iLadder] = cNumberOfChipsInLadder_All[cParticipatedLadders[iLadder]];
    cXPositionOfFirstChipInLadder[iLadder] = cXPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cYPositionOfFirstChipInLadder[iLadder] = cYPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cZPositionOfFirstChipInLadder[iLadder] = cZPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cXRotationOfChipsInLadder[iLadder] = cXRotationOfChipsInLadder_All[cParticipatedLadders[iLadder]];
    cYRotationOfChipsInLadder[iLadder] = cYRotationOfChipsInLadder_All[cParticipatedLadders[iLadder]];
  }
  //-----------------------------------------------------------------------------------------------------------------//


  //-----------------------------------------------------------------------------------------------------------------//
  //Define ladders, telescope and their chips to be added to the output event tree
  AlpideLadder* arrayOfLadders[cNumberOfLadders];
  AlpideChip* arrayOfChipsInLadders[cNumberOfLadders][3];
  AlpideTelescope *telescope = new AlpideTelescope();
  AlpideChip* arrayOfChipsInTelescope[cNumberOfChipsInTelescope];
  for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    arrayOfChipsInTelescope[iChip] = new AlpideChip();
  }

  AlpidePixel *pixel = new AlpidePixel();

  //Reset the ladders and set their geometry according to common.C
  for(Int_t iLadder=0; iLadder<cNumberOfLadders; iLadder++){
    arrayOfLadders[iLadder] = new AlpideLadder();
    arrayOfLadders[iLadder]->ResetLadder();
    arrayOfLadders[iLadder]->SetLadderID(cLadderID[iLadder]);
    for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
      arrayOfChipsInLadders[iLadder][iChip] = new AlpideChip();
      arrayOfChipsInLadders[iLadder][iChip]->ResetChip();
      arrayOfChipsInLadders[iLadder][iChip]->SetNumberOfPixelsX(cNumberOfColumnsInChip);
      arrayOfChipsInLadders[iLadder][iChip]->SetNumberOfPixelsY(cNumberOfRowsInChip);
      arrayOfChipsInLadders[iLadder][iChip]->SetPositionX(cXPositionOfFirstChipInLadder[iLadder]-iChip*cChipXSize);
      arrayOfChipsInLadders[iLadder][iChip]->SetPositionY(cYPositionOfFirstChipInLadder[iLadder]);
      arrayOfChipsInLadders[iLadder][iChip]->SetPositionZ(cZPositionOfFirstChipInLadder[iLadder]);
      arrayOfChipsInLadders[iLadder][iChip]->SetSizeX(cChipXSize);
      arrayOfChipsInLadders[iLadder][iChip]->SetSizeY(cChipYSize);
      arrayOfChipsInLadders[iLadder][iChip]->SetInitResoX(cInitXReso);
      arrayOfChipsInLadders[iLadder][iChip]->SetInitResoY(cInitYReso);
      arrayOfChipsInLadders[iLadder][iChip]->SetChipID(cTopChipId-iChip);//The chip ids are numbered in such way that the top chip of the ladder has always the id 8.
      arrayOfChipsInLadders[iLadder][iChip]->SetRotationX(cXRotationOfChipsInLadder[iLadder]);
      arrayOfChipsInLadders[iLadder][iChip]->SetRotationY(cYRotationOfChipsInLadder[iLadder]);
      arrayOfLadders[iLadder]->AddChip(arrayOfChipsInLadders[iLadder][iChip]);
    }

  }

  //Reset the telescope and set its chips
  telescope->ResetTelescope();
  telescope->SetTelescopeID(cTelescopeID);
  for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    arrayOfChipsInTelescope[iChip] = new AlpideChip();
    arrayOfChipsInTelescope[iChip]->ResetChip();
    arrayOfChipsInTelescope[iChip]->SetNumberOfPixelsX(cNumberOfColumnsInChip);
    arrayOfChipsInTelescope[iChip]->SetNumberOfPixelsY(cNumberOfRowsInChip);
    arrayOfChipsInTelescope[iChip]->SetPositionX(cXPositionOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetPositionY(cYPositionOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetPositionZ(cZPositionOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetSizeX(cChipXSize);
    arrayOfChipsInTelescope[iChip]->SetSizeY(cChipYSize);
    arrayOfChipsInTelescope[iChip]->SetMisAlignX(0);
    arrayOfChipsInTelescope[iChip]->SetMisAlignY(0);
    arrayOfChipsInTelescope[iChip]->SetInitResoX(cInitXReso);
    arrayOfChipsInTelescope[iChip]->SetInitResoY(cInitYReso);
    arrayOfChipsInTelescope[iChip]->SetRotationX(cXRotationOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetRotationY(cYRotationOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetChipID(cTelescopeChipID[iChip]);
    telescope->AddChip(arrayOfChipsInTelescope[iChip]);
  }
  //-----------------------------------------------------------------------------------------------------------------//


  //-----------------------------------------------------------------------------------------------------------------//
  //Get the merged tree (somehow TChain when used mix everything)
  TFile *inputFile = new TFile(Form("Data/multinoiseScan_%s.root",runName.Data()));
  TTree *pixelsChain = ((TTree*) inputFile->Get("pixTree"));

  Int_t maxTriggerN = (Int_t)pixelsChain->GetMaximum("trgNum");
  Long_t numberOfHitPixels = pixelsChain->GetEntries();

  //Get the branch where the leaf are stored
  TBranch *dataBranch = 0;
  dataBranch = pixelsChain->GetBranch("fData");

  //Set the addresses for the different leaves:TODO: This is not professional. Define a pointer for the different leaves.
  TLeaf *boardIDLeaf = dataBranch->GetLeaf("boardIndex");
  TLeaf *deviceIDLeaf = dataBranch->GetLeaf("deviceId");
  TLeaf *colLeaf  = dataBranch->GetLeaf("col");
  TLeaf *rowLeaf  = dataBranch->GetLeaf("row");
  TLeaf *chipIDLeaf  = dataBranch->GetLeaf("chipId");
  TLeaf *triggerNumLeaf  = dataBranch->GetLeaf("trgNum");
  TLeaf *bunchNumLeaf  = dataBranch->GetLeaf("bunchNum");
  TLeaf *trgTimeLeaf  = dataBranch->GetLeaf("trgTime");
  //-----------------------------------------------------------------------------------------------------------------//


  //-----------------------------------------------------------------------------------------------------------------//
  //Arrange the events in vector of vectors. Each vector with a given index contains the indices of the different hits coresponding to the trigger number. This will make the reading below faster
  std::vector<std::vector<double> > vectorEvents;
  for(Int_t iEvent=0;iEvent<maxTriggerN;iEvent++){
    std::vector<double> tempoVector;
    vectorEvents.push_back(tempoVector);
  }
  Int_t triggerIndex=0;
  for(Int_t iHitPixel=0;iHitPixel<numberOfHitPixels;iHitPixel++){
    dataBranch->GetEntry(iHitPixel);
    triggerIndex = (Int_t)triggerNumLeaf->GetValue(0);
    vectorEvents[triggerIndex].push_back(iHitPixel);
  }
  //-----------------------------------------------------------------------------------------------------------------//


  //-----------------------------------------------------------------------------------------------------------------//
  //Define some variables to be got in the loop
  //In the raw tree, the hits data are stored and for each hit there is the trigger and the bunch number. a new event will be defined when the trigger changes.
  Int_t triggerN = 0;
  Int_t eventCounter=0;
  Int_t pixelRow;
  Int_t pixelCol;
  Int_t chipID;
  Int_t deviceID;
  Int_t numberOfGoodHits =0;
  //-----------------------------------------------------------------------------------------------------------------//



  //-----------------------------------------------------------------------------------------------------------------//
  //The output tree, containing a main branch of type TestBeamEvent.
  TestBeamEvent* event = new TestBeamEvent();
  TFile *outputFile = new TFile(Form("%s/%s",outputDirectory.Data(),outputFileName.Data()), "RECREATE");
  TTree *outputTree = new TTree("TestBeamEventTree", "TestBeamEventTree");
  outputTree->Branch("event", "TestBeamEvent", &event);
  //-----------------------------------------------------------------------------------------------------------------//



  //-----------------------------------------------------------------------------------------------------------------//
  //Start looping over the trigger numbers. For each trigger, get the vector containing the indices of the different hits, if not all bad hits, update the chips and fill the event.
  Double_t numberOfTriggers = cFractionOfEvents*maxTriggerN;
  for(Int_t iTrigger=1;iTrigger<numberOfTriggers;iTrigger++){

    printf("Reading rwa hit pixels ... %.0f%%%s", 100.*iTrigger/numberOfTriggers, (iTrigger < numberOfTriggers) ? "\r" : "\n");
    // cout<<endl;

    //This will clear the event.
    event->ResetEvent();
    numberOfGoodHits=0;

    //Reset the ladders and set their geometry according to common.C
    for(Int_t iLadder=0; iLadder<cNumberOfLadders; iLadder++){
      arrayOfLadders[iLadder]->ResetLadder();
      arrayOfLadders[iLadder]->SetLadderID(cLadderID[iLadder]);
      for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
        arrayOfChipsInLadders[iLadder][iChip]->ResetChip();
        arrayOfLadders[iLadder]->AddChip(arrayOfChipsInLadders[iLadder][iChip]);
      }
    }


    //Reset the telescope and set its chips
    telescope->ResetTelescope();
    telescope->SetTelescopeID(cTelescopeID);
    for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
      arrayOfChipsInTelescope[iChip]->ResetChip();
      telescope->AddChip(arrayOfChipsInTelescope[iChip]);
    }

    //Loop over the indices in the input tree of the hits associated with a trigger
    for(Int_t iEntry=0;iEntry<vectorEvents[iTrigger].size();iEntry++){

      dataBranch->GetEntry(vectorEvents[iTrigger][iEntry]);

      triggerN = (Int_t)triggerNumLeaf->GetValue(0);
      pixelRow = (Int_t)rowLeaf->GetValue(0);
      pixelCol = (Int_t)colLeaf->GetValue(0);
      chipID   = (Int_t)chipIDLeaf->GetValue(0);
      deviceID = (Int_t)deviceIDLeaf->GetValue(0);

      if(IsThisABadPixel(deviceID,chipID,pixelRow,pixelCol)){
        continue;
      }

      pixel->SetCol(pixelCol);
      pixel->SetRow(pixelRow);
      pixel->SetNumberOfHits(1);
      numberOfGoodHits++;


      if(deviceID == 0){
        //This is the case of the telescope
        telescope->GetChipByID(chipID)->AddHitPixel(pixel);
      }
      else{
        //Get which ladder is hit
        for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
          if(deviceID == cLadderID[iLadder]){
            arrayOfLadders[iLadder]->GetChipByID(chipID)->AddHitPixel(pixel);
          }
        }
      }


    } // End while ReadEvent()



    if(numberOfGoodHits >0){
      //Here you fill the event.//TODO: Make an event reader class that can read TTree (like the .dat)
      //add the ladders to the event:
      for(Int_t iLadder=0; iLadder<cNumberOfLadders; iLadder++){
        event->AddLadder(arrayOfLadders[iLadder]);
      }

      event->SetTelescope(telescope);
      outputTree->Fill();
      eventCounter++;
    }

    //End loop over trigger numbers
  }



  cout<<Form("Reading the pixel chain %s is done",runName.Data())<<endl;

  //Write the output file and save the tree.
  outputFile->Write();
  outputTree->ResetBranchAddresses();
  outputFile->Close();


}
