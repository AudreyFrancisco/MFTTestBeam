/*
This macro is for testing purpose only, it will not be used during the real data analysis. The goal of this macro is simply to genertae random tracks that are suposed to be in the acceptnace of the two ladders and store them in a tree that contains events with array of TelescopeTrack objects. The macro will also produce the .txt raw file with information on the hits per events.
*/
//needed root classes
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TMath.h"
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "TelescopeTrack.hpp"


Int_t maxNumberOfTracksPerEvent = 1;
Int_t maxNumberOfHitsPerTrack = 6;


//These two arrays are to be used when generating tracks for the alignments. Each chip of the telescope will be shifted by the amounts.
Double_t artDeltaX[cNumberOfChipsInTelescope] = {0,0,0};
Double_t artDeltaY[cNumberOfChipsInTelescope] = {0,0,0};

//This function is obsolete. Was used when the addresses of the hits were given in terms of double column and address. Now they are already stored in a TTree by their col,row coordinates.
void ColRowToDoubleColAddress(Int_t col, Int_t row, Int_t &doubleCol, Int_t &address) {
  address = row*2;
  if(address%4 == 1){
    col+1;
  }
  doubleCol = col/2;
}
//-------------------------------------------------------------------------------------------------------------------------------------//
//dummy function to check if a value is already in  a vector.
//TODO: Make it more general so that it can take different type of variables (int, float, string...)
Bool_t IsValueAlreadyInVector(int value, std::vector<int> vector){
  for(int iElement =0;iElement<vector.size();iElement++){
    if(value == vector[iElement]){
      return kTRUE;
    }
  }
  return kFALSE;
}
//-------------------------------------------------------------------------------------------------------------------------------------//

Bool_t CreateTrackHitsAtZ(TelescopeTrack *track, Float_t zCoordiante, Int_t detectorID, Int_t whichChip, ofstream& outputTxtFile){
  /////////////////////////////////////////////////////
  //Set the random object seed to the actual time so that different values will be generated every time the macro is launched.
  TDatime actualTime;
  static UInt_t seed = actualTime.Get();
  seed += ((Int_t)zCoordiante)*((Int_t)detectorID+3);

  TRandom3 *randomObject = new TRandom3();
  randomObject->SetSeed(seed);
  //////////////////////////////////////////////////////

  //Also the number of hit per track is random
  int numberOfHitPixels = randomObject->Integer(maxNumberOfHitsPerTrack)+1;

  Float_t trackXCoordinateAtLadder, trackYCoordinateAtLadder;
  track->GetXYCoordinate(trackXCoordinateAtLadder,trackYCoordinateAtLadder,zCoordiante);
  //Get to which chip does this y correspond:

  Int_t pixelCol, pixelRow, chipID;
  if(detectorID < 2){//case of ladders
    whichChip = (trackXCoordinateAtLadder + 0.5*cNumberOfChipsInLadder[cNumberOfLadders-1]*cChipXSize)/cChipXSize;
    pixelCol = ((cXPositionOfFirstChipInLadder[cNumberOfLadders-1]+whichChip*cChipXSize) - trackXCoordinateAtLadder)*(cNumberOfColumnsInChip/cChipXSize);
    if(cLadderChipsOriginCol[detectorID] > 0){
      pixelCol = cLadderChipsOriginCol[detectorID]-pixelCol;
      // cout<<pixelCol<<endl;
    }
    pixelRow = (cYPositionOfFirstChipInLadder[cNumberOfLadders-1]-trackYCoordinateAtLadder)*(cNumberOfRowsInChip/cChipYSize);
    if(cLadderChipsOriginRow[detectorID] > 0){
      pixelRow = cLadderChipsOriginRow[detectorID]-pixelRow;
    }
    chipID = cTopChipId-cNumberOfChipsInLadder[0]+whichChip+1;
  }
  else if(detectorID == 2){//case of telescope
    // trackXCoordinateAtLadder = trackXCoordinateAtLadder;

    //Add artificual misalignments
    trackXCoordinateAtLadder = trackXCoordinateAtLadder + artDeltaX[whichChip];
    trackYCoordinateAtLadder = trackYCoordinateAtLadder + artDeltaY[whichChip];

    pixelCol = (cXPositionOfChipsInTelescope[0] - trackXCoordinateAtLadder)*(cNumberOfColumnsInChip/cChipXSize);
    pixelRow = (cYPositionOfChipsInTelescope[0] - trackYCoordinateAtLadder)*(cNumberOfRowsInChip/cChipYSize);
    chipID = cTelescopeChipID[whichChip];
  }


  int pixelNHits = 1;//This one is present in the real readout file format but not used in the analysis so take it 1

  if(pixelCol<0 || pixelCol>=cNumberOfColumnsInChip || pixelRow<0 || pixelRow>=cNumberOfRowsInChip) return kFALSE;

  //write the first hit pixel of the cluster. Below the other pixels in the cluster are generated
  //Convert the col,row to doubleCol address
  Int_t doubleCol, address;
  ColRowToDoubleColAddress(pixelCol,pixelRow,doubleCol,address);
  if(cRowColStructure){
    outputTxtFile<<detectorID<<" "<<chipID<<" "<<pixelRow<<" "<<pixelCol<<" "<<pixelNHits<<endl;
  }
  else{
    outputTxtFile<<detectorID<<" "<<chipID<<" "<<doubleCol<<" "<<address<<" "<<pixelNHits<<endl;
  }


  //Store each generated hit pixel so no double pixels will be present
  std::vector<int> vectorCol;
  vectorCol.push_back(pixelCol);

  std::vector<int> vectorRow;
  vectorRow.push_back(pixelRow);

  //the other pixels in the cluster
  for(int iHitPixel = 1;iHitPixel<numberOfHitPixels;iHitPixel++){

    Int_t deltaCol = 0;
    Int_t deltaRow = 0;
    while(deltaCol==0 && deltaRow==0){
      deltaCol = randomObject->Integer(3)-1;
      deltaRow = randomObject->Integer(3)-1;
    }

    Int_t dummyCounter =0;//sometimes (when the main pixel is on border, the pixels finding will stuck, the loop below will stop after some times)

    do{
      int pixelToBeNeighboured = randomObject->Integer(vectorRow.size());
      pixelCol = vectorCol[pixelToBeNeighboured]+deltaCol;
      pixelRow = vectorRow[pixelToBeNeighboured]+deltaRow;
      dummyCounter++;
      if(dummyCounter==1000) break;
    } while( (IsValueAlreadyInVector(pixelCol,vectorCol) && IsValueAlreadyInVector(pixelRow,vectorRow)) || pixelCol<0 || pixelRow<0 || pixelCol>=cNumberOfColumnsInChip || pixelRow>=cNumberOfRowsInChip );


    if(dummyCounter==1000) continue;
    ColRowToDoubleColAddress(pixelCol,pixelRow,doubleCol,address);
    if(cRowColStructure){
      outputTxtFile<<detectorID<<" "<<chipID<<" "<<pixelRow<<" "<<pixelCol<<" "<<pixelNHits<<endl;
    }
    else{
      outputTxtFile<<detectorID<<" "<<chipID<<" "<<doubleCol<<" "<<address<<" "<<pixelNHits<<endl;
    }
    vectorCol.push_back(pixelCol);
    vectorRow.push_back(pixelRow);

  }
  return kTRUE;

}
///
void GenerateRandomEvents(Int_t numberOfRandomEvents =10, TString txtFileName = "rawEvents.txt", TString outputDirectory = "TestRun")
{
  /////////////////////////////////////////////////////
  //Set the random object seed to the actual time so that different values will be generated every time the macro is launched.
  TDatime actualTime;
  static UInt_t seed = actualTime.Get();

  TRandom3 *randomObject = new TRandom3();
  randomObject->SetSeed(seed);
  //////////////////////////////////////////////////////


  // This is the first output of the macro, an ascii file containing the events and the corresponding hits. This is made to look like the output of the ladders in the real beam test.
  ofstream outputTxtFile(Form("Data/%s",txtFileName.Data()));

  for(int iEvent=0;iEvent<numberOfRandomEvents;iEvent++){

    printf("Generating events ... %.0f%%%s", 100.*iEvent/numberOfRandomEvents, (iEvent < numberOfRandomEvents) ? "\r" : "\n");

    outputTxtFile<<iEvent<<endl;

    //The number of tracks per event is random ? How this is true in the reality ? Is it one track ?
    int numberOfTracks = randomObject->Integer(maxNumberOfTracksPerEvent)+1;
    // int numberOfTracks = 10;

    for(int iTrack =0;iTrack<numberOfTracks;iTrack++){

      //Start randomising the track parameters (line equation). In this macro, all the tracks are passing through the first chip of the telescope and the last ladder. so by construction the track is generated to be in the acceptance of the telescope.

      Float_t x1, y1, z1, x2, y2, z2;

      if(cGenerateForAlignment){
        //Get the fisrt point on the last chip of the telescope:
        x1 = cChipXSize*randomObject->Rndm()-0.5*cChipXSize;
        y1 = cChipYSize*randomObject->Rndm()-0.5*cChipYSize;
        z1 = cZPositionOfChipsInTelescope[cNumberOfChipsInTelescope-1];

        //Get the point on the scintillator
        x2 = cChipXSize*randomObject->Rndm()-0.5*cChipXSize;
        y2 = cChipXSize*randomObject->Rndm()-0.5*cChipXSize;
        z2 = cScintillatorZPosition;
      }
      else{
        x1 = cChipXSize*randomObject->Rndm()-0.5*cChipXSize;
        y1 = cChipYSize*randomObject->Rndm()-0.5*cChipYSize;
        z1 = cZPositionOfChipsInTelescope[0];

        //Get the second point which is a random point on the last ladder
        x2 = cNumberOfChipsInLadder[cNumberOfLadders-1]*(cChipXSize*randomObject->Rndm()-0.5*cChipXSize);
        y2 = cChipYSize*randomObject->Rndm()-0.5*cChipYSize;
        z2 = cZPositionOfFirstChipInLadder[cNumberOfLadders-1];
      }


      //Define the slope and constantes of the line using the two points:
      Float_t trackXConstante = (z1*x2 - z2*x1)/(z1-z2);
      Float_t trackYConstante = (z1*y2 - z2*y1)/(z1-z2);
      Float_t trackXSlope = (x1-x2)/(z1-z2);
      Float_t trackYSlope = (y1-y2)/(z1-z2);

      TelescopeTrack *track = new TelescopeTrack(iTrack,trackXConstante,trackXSlope,trackYConstante,trackYSlope);//Do not add this track to the telescope since the ones will be added are the obtained from fit of the telescoe clusters
      track->SetChiSquare(1);

      //For each track, get the intersection point with the two ladders, get to which chip this (x,y) point corresponds and generate cluster with an efficiency according to the one of the chip.
      for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
        if(!CreateTrackHitsAtZ(track,cZPositionOfFirstChipInLadder[iLadder],iLadder,-1,outputTxtFile)) continue;
      }

      //create the hits in the telescope
      for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
        if(!CreateTrackHitsAtZ(track,cZPositionOfChipsInTelescope[iChip],2,iChip,outputTxtFile)) continue;
      }

    }

  }

  outputTxtFile.close();

}
