/*
This is the first macro to call in the analysis process. It will take as an argument the name of the .txt file where the hits per events are stored and using the EventReader will read them and store them in an event tree where the different chips are added with their hit pixels. The output tree of this macro will not have any clusters. The ClusterRec.C macro takes the output of this macro and make the clustering and update the tree with the cluster information. The name of the output tree is given as a paramater. This macro do not produced drawable objects.
//the constant parameters like the number of chipas nad their geometry are defined in the Common.C macro which must be compiled first.
To run the macro from a root session: .x RawFileToTree("inputFileName","outputFileName")
//This macro is replaced by RawTreeToTree.C. The actual macro is only used for reading generated events.
*/

//needed root classes
#include "Riostream.h"
#include <TTree.h>
#include <TFile.h>
//needed user defined classes
#include "EventReader.hpp"
#include "TestBeamEvent.hpp"
#include "AlpideLadder.hpp"
#include "AlpideChip.hpp"
#include "AlpideTelescope.hpp"

using namespace std;

void DoubleColAddressToColRow(Int_t doubleCol, Int_t address, Int_t &col, Int_t &row) {
  col = doubleCol*2;
  col += ((((address%4)==1) || ((address%4)==2)) ? 1:0);
  row = address/2;
}
//_______________________________________________________________________________________________
void RawFileToTree(TString rawFileName = "rawEvents.txt", TString outputFileName = "TestBeamEventTree.root", TString outputDirectory = "TestRun")
{

  cout<<Form("I: Converting the raw file %s to tree of events",rawFileName.Data())<<endl;
  gROOT->ProcessLineSync(".L Common.C") ;

  // raw file reader
  EventReader *reader = new EventReader();
  if( !reader->SetInputFile(Form("Data/%s",rawFileName.Data()) ) ) {
    cerr<< "Error: Cannot open raw file!" << endl;
    return;
  }


  //
  // This is the main output of the function, a tree containing the events from the raw file. Each event contains information of the chips, and the hit pixels in them. Clustering is not done in this function.
  TFile* outputFile = new TFile(Form("%s/%s",outputDirectory.Data(),outputFileName.Data()), "RECREATE");
  TestBeamEvent* event = new TestBeamEvent();
  TTree* outputTree = new TTree("TestBeamEventTree", "TestBeamEventTree");
  outputTree->Branch("event", "TestBeamEvent", &event);


  ////// the main detector in the event are the ladders and the telescope. Ladders are always there. Telescope can be or not depending on the flag in Common.C

  AlpideTelescope *telescope = new AlpideTelescope();
  AlpideLadder* arrayOfLadders[cNumberOfLadders];
  for(Int_t iLadder=0; iLadder<cNumberOfLadders; iLadder++){
    arrayOfLadders[iLadder] = new AlpideLadder();
  }


  Int_t eventCounter = 0;//the evnt counter which will be used as the evnt id. This might be changed to set the event id as it mentioned in the raw file.
  while(reader->ReadEvent()) {

    //printing options
    if(cVerboseOption){
      Int_t   numberOfHitsInEvent = reader->GetNumberOfHitPixels();
      cout<<"Event: "<<eventCounter<<" hit pixels"<<numberOfHitsInEvent<<endl;
    }
    else{
      if( (eventCounter+1)%1000 == 0 ) cout << "Processed events: " << eventCounter+1 << endl;
    }

    //Clear the event and define new chips for each event. The constants parameters are taken form Common.C
    event->ResetEvent();
    event->SetEventID(eventCounter);


    //Reset the ladders and set their geometry according to common.C
    for(Int_t iLadder=0; iLadder<cNumberOfLadders; iLadder++){
      arrayOfLadders[iLadder]->ResetLadder();
      arrayOfLadders[iLadder]->SetLadderID(cLadderID[iLadder]);
      Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
      for(Int_t iChip=0;iChip<numberOfChipsInLadder;iChip++){
        AlpideChip *chip = new AlpideChip();
        chip->ResetChip();
        chip->SetNumberOfPixelsX(cNumberOfColumnsInChip);
        chip->SetNumberOfPixelsY(cNumberOfRowsInChip);
        chip->SetPositionX(cXPositionOfFirstChipInLadder[iLadder]+iChip*cChipXSize);//First chip is the one at bottom of the ladder (above the peripherly)
        chip->SetPositionY(cYPositionOfFirstChipInLadder[iLadder]);
        chip->SetPositionZ(cZPositionOfFirstChipInLadder[iLadder]);
        chip->SetSizeX(cChipXSize);
        chip->SetSizeY(cChipYSize);
        chip->SetInitResoX(cInitXReso);
        chip->SetInitResoY(cInitYReso);
        chip->SetChipID(cTopChipId-numberOfChipsInLadder+iChip+1);//The chip ids are numbered in such way that the top chip of the ladder has always the id 8.
        chip->SetOriginCol(cLadderChipsOriginCol[iLadder]);
        chip->SetOriginRow(cLadderChipsOriginRow[iLadder]);
        arrayOfLadders[iLadder]->AddChip(chip);
      }

    }



    //Reset the telescope and set its chips
    telescope->ResetTelescope();
    telescope->SetTelescopeID(cTelescopeID);
    for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
      AlpideChip *chip = new AlpideChip();
      chip->ResetChip();
      chip->SetNumberOfPixelsX(cNumberOfColumnsInChip);
      chip->SetNumberOfPixelsY(cNumberOfRowsInChip);
      chip->SetPositionX(cXPositionOfChipsInTelescope[iChip]);
      chip->SetPositionY(cYPositionOfChipsInTelescope[iChip]);
      chip->SetPositionZ(cZPositionOfChipsInTelescope[iChip]);
      chip->SetSizeX(cChipXSize);
      chip->SetSizeY(cChipYSize);
      chip->SetMisAlignX(0);
      chip->SetMisAlignY(0);
      chip->SetInitResoX(cInitXReso);
      chip->SetInitResoY(cInitYReso);
      chip->SetOriginCol(cTelescopeChipsOriginCol[iChip]);
      chip->SetOriginRow(cTelescopeChipsOriginRow[iChip]);
      chip->SetChipID(cTelescopeChipID[iChip]);
      telescope->AddChip(chip);
    }



    //Start reading the hits and stored them in temporarly varibales before converting them into (row,col) adresses. For the moment nothing is done with number of hits per pixel.
    Int_t detectorID,chipID, pixelFirstAddress, pixelSecondAddress, pixelNHits;
    while(reader->GetNextHitPixel(&detectorID, &chipID, &pixelFirstAddress, &pixelSecondAddress, &pixelNHits)) {
      if(pixelNHits > cMaxNHits) continue;
      //build the hit pixel
      Int_t col, row;
      if(cRowColStructure){
        row = pixelFirstAddress;
        col = pixelSecondAddress;
      }
      else{
        DoubleColAddressToColRow(pixelFirstAddress, pixelSecondAddress, col, row);
      }

      AlpidePixel *pixel = new AlpidePixel(row,col,pixelNHits);
      //Find the associated detector, then find the chip in the detector with its id, then add the pixel to the chip.
      if(detectorID == cTelescopeID){
        telescope->GetChipByID(chipID)->AddHitPixel(pixel);
      }
      else{
        for(Int_t iLadder=0; iLadder<cNumberOfLadders; iLadder++){
          if(cLadderID[iLadder] == detectorID){
            arrayOfLadders[iLadder]->GetChipByID(chipID)->AddHitPixel(pixel);
            break;
          }
        }
      }
    }

    //add the ladders to the event:
    for(Int_t iLadder=0; iLadder<cNumberOfLadders; iLadder++){
      event->AddLadder(arrayOfLadders[iLadder]);
    }
    //add the telescope to the event:
    event->SetTelescope(telescope);

    outputTree->Fill();
    eventCounter++;
  } // End while ReadEvent()

  cout<<Form("Reading the raw file %s is done",rawFileName.Data())<<endl;

  //Write the output file and save the tree.
  outputFile->Write();
  outputTree->ResetBranchAddresses();
  outputFile->Close();

  return;
}
