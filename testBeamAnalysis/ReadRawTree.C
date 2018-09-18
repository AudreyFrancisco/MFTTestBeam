
//needed root classes
#include "Riostream.h"
#include <TTree.h>
#include <TFile.h>
#include "TLeaf.h"
#include "TBranch.h"
#include "TChain.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
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

//-----------------------------------------------------------------------------------------------------------------//
//Open the file containing the different mask histograms:
TFile *inputMasksFile = new TFile("Masks/Masks.root");
//Define a function to check if a pixel is masked or not:
Bool_t IsThisABadPixel(Int_t deviceID, Int_t chipID, Int_t row, Int_t col){
  TH2D *histoChip;
  if(deviceID == 0){
    histoChip = ((TH2D*) inputMasksFile->Get(Form("hMask_ibhic0_chip%d",chipID)));
  }
  else{
    histoChip = ((TH2D*) inputMasksFile->Get(Form("hMask_ladder%d_chip%d",deviceID,chipID)));
  }
  if(histoChip->GetBinContent(col+1,row+1) != 0) return kTRUE;
  return kFALSE;
}



//-----------------------------------------------------------------------------------------------------------------//

using namespace std;
//_______________________________________________________________________________________________
void ReadRawTree(TString runName, TString outputFileName, TString outputDirectory)
{

  //-----------------------------------------------------------------------------------------------------------------//
  //Get the position of the disk based on the time of the run
  //Get the alignment folder (there are two disk positions) corresponding to this run:
  Double_t diskPositionDeltaX = 0;
  Double_t diskPositionDeltaY = 0;
  TObjArray *objRunName = outputDirectory.Tokenize("_");
  Int_t runDate = ((TObjString*)objRunName->UncheckedAt(0))->String().Atoi();
  Int_t runTime = ((TObjString*)objRunName->UncheckedAt(1))->String().Atoi();
  if(runDate > 180706){
    diskPositionDeltaX = -34.705;
    diskPositionDeltaY = 15;
  }
  if(runDate == 180706){
    if(runTime > 83820){
      diskPositionDeltaX = -34.705;
      diskPositionDeltaY = 15;
    }
  }

  //-----------------------------------------------------------------------------------------------------------------//
  //Get the alignment file with the bin corresponding to the run name
  TFile *inputAlignmentFile = new TFile("LaddersAlignment.root");
  TH2F *histoMisAlignment = ((TH2F*) inputAlignmentFile->Get("histoMisAlignment"));
  Int_t binNumberInAlignmentHisto = histoMisAlignment->GetXaxis()->FindBin(outputDirectory);

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
    Double_t misAlignX = histoMisAlignment->GetBinContent(binNumberInAlignmentHisto,2*iLadder+1);
    Double_t misAlignY = histoMisAlignment->GetBinContent(binNumberInAlignmentHisto,2*iLadder+2);
    for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
      arrayOfChipsInLadders[iLadder][iChip] = new AlpideChip();
      arrayOfChipsInLadders[iLadder][iChip]->ResetChip();
      arrayOfChipsInLadders[iLadder][iChip]->SetNumberOfPixelsX(cNumberOfColumnsInChip);
      arrayOfChipsInLadders[iLadder][iChip]->SetNumberOfPixelsY(cNumberOfRowsInChip);
      arrayOfChipsInLadders[iLadder][iChip]->SetPositionX(diskPositionDeltaX+cXPositionOfFirstChipInLadder[iLadder]-iChip*cChipXSize-misAlignX);
      arrayOfChipsInLadders[iLadder][iChip]->SetPositionY(diskPositionDeltaY+cYPositionOfFirstChipInLadder[iLadder]-misAlignY);
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
    arrayOfChipsInTelescope[iChip]->SetSizeX(cChipXSize);
    arrayOfChipsInTelescope[iChip]->SetSizeY(cChipYSize);
    arrayOfChipsInTelescope[iChip]->SetInitResoX(cInitXReso);
    arrayOfChipsInTelescope[iChip]->SetInitResoY(cInitYReso);
    arrayOfChipsInTelescope[iChip]->SetRotationX(cXRotationOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetRotationY(cYRotationOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetChipID(cTelescopeChipID[iChip]);
    //Get the misalignment values per each chip
    arrayOfChipsInTelescope[iChip]->SetPositionX(cXPositionOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetPositionY(cYPositionOfChipsInTelescope[iChip]);
    arrayOfChipsInTelescope[iChip]->SetPositionZ(cZPositionOfChipsInTelescope[iChip]);
    telescope->AddChip(arrayOfChipsInTelescope[iChip]);
  }
  //-----------------------------------------------------------------------------------------------------------------//


  //-----------------------------------------------------------------------------------------------------------------//
  //Get the merged tree (somehow TChain when used mix everything)
  TFile *inputFile = new TFile(Form("Data/Test_Beam_0618/Master-slave_data_taking/multinoiseScan_%s.root",runName.Data()));
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
  // std::vector<std::vector<double> > vectorEvents;
  // for(Int_t iEvent=0;iEvent<maxTriggerN;iEvent++){
  //   std::vector<double> tempoVector;
  //   vectorEvents.push_back(tempoVector);
  // }
  // Int_t triggerIndex=0;
  // for(Int_t iHitPixel=0;iHitPixel<numberOfHitPixels;iHitPixel++){
  //   dataBranch->GetEntry(iHitPixel);
  //   triggerIndex = (Int_t)triggerNumLeaf->GetValue(0);
  //   vectorEvents[triggerIndex].push_back(iHitPixel);
  // }
  //-----------------------------------------------------------------------------------------------------------------//


  map<long, vector<int>> mapEvents;
  map<long, vector<int>> mapEventsTest;
  long triggerTime;
  long triggerNumber;
  // Long_t test2;
  for(Int_t iHitPixel=0;iHitPixel<numberOfHitPixels;iHitPixel++){
    dataBranch->GetEntry(iHitPixel);
    triggerTime = (long)trgTimeLeaf->GetValue(0);
    triggerNumber = (int)triggerNumLeaf->GetValue(0);
    triggerTime = triggerTime/10000000000;
    if(triggerTime == 0) continue;
    mapEvents[triggerTime].push_back(iHitPixel);
    mapEventsTest[triggerTime].push_back(triggerNumber);
  }
  map<long, vector<int>>::iterator mapIterator;
  map<long, vector<int>>::iterator mapIteratorTest;
  // cout<<mapEvents.size()<<endl;



  vector<int> tempoVectorIndex;
  vector<int> tempoVectorTrigger;
  vector<int> vectorToBeDeleted;
  map<int, vector<int>> mapEventsFinal;

  for ( mapIterator = mapEvents.begin(), mapIteratorTest = mapEventsTest.begin(); mapIterator != mapEvents.end(); mapIterator++,mapIteratorTest++ ){

    vectorToBeDeleted.clear();

    tempoVectorIndex = mapIterator->second;
    tempoVectorTrigger = mapIteratorTest->second;
    vectorToBeDeleted.push_back(tempoVectorTrigger[0]);
    Int_t jEntry;
    for(Int_t iEntry=0;iEntry<tempoVectorTrigger.size();iEntry++){
      for(jEntry=0;jEntry<vectorToBeDeleted.size();jEntry++){
        if(TMath::Abs(tempoVectorTrigger[iEntry]-vectorToBeDeleted[jEntry])<10){
          mapEventsFinal[vectorToBeDeleted[jEntry]].push_back(tempoVectorIndex[iEntry]);
          break;
        }
      }
      if(jEntry == vectorToBeDeleted.size()){
        vectorToBeDeleted.push_back(tempoVectorTrigger[iEntry]);
        mapEventsFinal[vectorToBeDeleted[jEntry]].push_back(tempoVectorIndex[iEntry]);
      }
    }

  }





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
  Double_t numberOfTriggers = 10000;

  map<int, vector<int>>::iterator mapIteratorFinal;

  vector<int> tempoVector;

  Int_t tempo=0;
  for ( mapIteratorFinal = mapEventsFinal.begin(); mapIteratorFinal != mapEventsFinal.end(); mapIteratorFinal++ ){
    // tempo++;
    // if(tempo < 140000) continue;
    tempoVector = mapIteratorFinal->second;
    if(eventCounter == numberOfTriggers) break;
    // for(Int_t iTrigger=1;iTrigger<maxTriggerN;iTrigger++){

    printf("Reading raw hit pixels ... %.0f%%%s", 100.*eventCounter/numberOfTriggers, (eventCounter < numberOfTriggers) ? "\r" : "\n");
    if(eventCounter%10000 == 0) cout<<eventCounter<<endl;
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
    Long_t triggerTimeTester;
    //Loop over the indices in the input tree of the hits associated with a trigger

    for(Int_t iEntry=0;iEntry<tempoVector.size();iEntry++){


      dataBranch->GetEntry(tempoVector[iEntry]);

      triggerN = (Int_t)triggerNumLeaf->GetValue(0);
      pixelRow = (Int_t)rowLeaf->GetValue(0);
      pixelCol = (Int_t)colLeaf->GetValue(0);
      chipID   = (Int_t)chipIDLeaf->GetValue(0);
      deviceID = (Int_t)deviceIDLeaf->GetValue(0);
      triggerTime = (Long_t)trgTimeLeaf->GetValue(0);

      // cout<<triggerN<<endl;

      if(iEntry == 0) triggerTimeTester = triggerTime;

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


  }



  cout<<Form("Reading the pixel chain %s is done",runName.Data())<<endl;

  //Write the output file and save the tree.
  outputFile->Write();
  outputTree->ResetBranchAddresses();
  outputFile->Close();


}
