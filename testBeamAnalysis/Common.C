/*
This macro contains some parameters that are constant for all the other macros. It also contains some functions that can be used by other macros. To be loaded (.L Common.C) before running the other macros.
*/
#include "TCanvas.h"
#include "TMath.h"
#include "TStyle.h"
#include "TROOT.h"

//-------------------------------------------------------------------------------------------------------------------------------------//
//The positions in the lab frame of the chips: //Todo: Make a class for the whole detector that can take care of this.
//For each chip, {x,y,z}
Double_t cChipXSize = 30.15;//width of the chip along the x axis in mm
Double_t cChipYSize = 13.76256;//height of the chip along the y axis in mm
//-------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------//
Short_t cNumberOfRowsInChip = 512;
Short_t cNumberOfColumnsInChip = 1024;
Bool_t  cRowColStructure = kTRUE;
//-------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------//
//The distance in x and y by which a cluster can be separated from a track and still be accepted. These values must be larger before performing the alognment.
Double_t cInitXReso = 0.5;
Double_t cInitYReso = 0.5;
//-------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------//
//Do not modify this block. These are the properties of the different available ladders in the test beam. The values are taken from:
// https://twiki.cern.ch/twiki/pub/ALICE/MftFirstTestBeamDiskPositionning/Ladders_on_New_Disk.pdf


// Disk positions (in the lab frame, and corresponds to the old positioning. For new runs, the position will be shifted in ReadRawTree.C)
Double_t cXDiskPosition = 74.075;
Double_t cYDiskPosition = 34;


enum cLadderIDEnum {k3025,k3028,k3014,k3010};
const Double_t cGeomXSizeOfChip = 30.15;
const Double_t cGeomYSizeOfChip = 17.0;
const Int_t cNumberOfLadders = 4;
const Int_t cLadderID[cNumberOfLadders] = {3025,3028,3014,3010};
const Int_t cNumberOfChipsInLadder[cNumberOfLadders] = {3,3,3,3};
// const Double_t cXPositionOfFirstChipInLadder[cNumberOfLadders] = {-22.06+cXDiskPosition-0.5*cGeomXSizeOfChip, -2.0+cXDiskPosition-0.5*cGeomXSizeOfChip-0.228963, -23.0+cXDiskPosition-0.5*cGeomXSizeOfChip, -20.46+cXDiskPosition-0.5*cGeomXSizeOfChip};
// const Double_t cYPositionOfFirstChipInLadder[cNumberOfLadders] = {-6.5+cYDiskPosition-0.5*cGeomYSizeOfChip, -23.5+cYDiskPosition-0.5*cGeomYSizeOfChip-0.370387, 3.5+cYDiskPosition-0.5*cGeomYSizeOfChip, -13.5+cYDiskPosition-0.5*cGeomYSizeOfChip};
const Double_t cXPositionOfFirstChipInLadder[cNumberOfLadders] = {-37.49+cXDiskPosition, -17.425+cXDiskPosition, -38.32+cXDiskPosition, -35.73+cXDiskPosition};
const Double_t cYPositionOfFirstChipInLadder[cNumberOfLadders] = {-49.55+cYDiskPosition, -32.43+cYDiskPosition, -60.1+cYDiskPosition, -43.15+cYDiskPosition};
const Double_t cZPositionOfFirstChipInLadder[cNumberOfLadders] = {230.,230.,247.,247.};

const Double_t cXRotationOfChipsInLadder[cNumberOfLadders] = {-1,-1,-1,-1};
const Double_t cYRotationOfChipsInLadder[cNumberOfLadders] = {-1,-1,1,1};
Int_t cTopChipId = 8;
//-------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------//
//Declare here the geometry of the telescope; positions in the lab frame, dimensions, number of chips and pixels....
const Int_t cNumberOfChipsInTelescope = 3;
Double_t cXPositionOfChipsInTelescope[cNumberOfChipsInTelescope] = {0.67,0.13,0};
Double_t cYPositionOfChipsInTelescope[cNumberOfChipsInTelescope] = {-0.06,-0.35,0};
Double_t cZPositionOfChipsInTelescope[cNumberOfChipsInTelescope] = {10,60,80};
Double_t cXRotationOfChipsInTelescope[cNumberOfChipsInTelescope] = {1,1,1};
Double_t cYRotationOfChipsInTelescope[cNumberOfChipsInTelescope] = {1,1,1};
Int_t cTelescopeID = 0;
Int_t cTelescopeChipID[cNumberOfChipsInTelescope] = {8,2,0};
//-------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------//
//These two parameters will only be used when generating tracks for the alignment.
Int_t cScintillatorZPosition = -1000;
Bool_t cGenerateForAlignment = kTRUE;
//-------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------//
//Number of maximum number per clusters. In the current implementation, nothing is said about it.
Int_t cMaxNumberOfHitsPerCluster = 300;
Int_t cMaxNHits = 1000;
//Sometimes you only want to check the hitmap. If so, turn this off.
Bool_t cDoClustering = kTRUE;
Bool_t cDrawLargeClusters = kTRUE;
Int_t  cLimitOfLargeCluster = 30;
Int_t cMaxNumberOfClusterPerEvent = 20;
//-------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------//
//Useful for test purpose. It will only analyse a fraction of the run event
Double_t cFractionOfEvents = 0.1;
//-------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------//
//This option if true will print event-by-event details when running the analysis macros
Bool_t cVerboseOption = kFALSE;
//Set the style of the canvas. Some repetitions are to be cleared.
void SetCanvasStyle(TCanvas *can){
  gStyle->SetOptStat(0);
  int font = 42;
  // gROOT->SetStyle("Plain");
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetLegendBorderSize(1);
  gStyle->SetTextFont(font);
  gStyle->SetTickLength(0.02,"y");
  gStyle->SetTickLength(0.02,"x");
  can->SetTickx();
  can->SetTicky();
  gStyle->SetLabelSize(0.04,"xyz");
  gStyle->SetLabelFont(font,"xyz");
  gStyle->SetLabelOffset(0.01,"xyz");
  gStyle->SetTitleFont(font,"xyz");
  gStyle->SetTitleOffset(1.1,"xy");
  gStyle->SetTitleSize(0.04,"xyz");
  gStyle->SetMarkerSize(1.1);
  gStyle->SetLineWidth(1);
  gStyle->SetLegendFont(42);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(10);
  gStyle->SetEndErrorSize(0);
  can->SetFillColor(0);
  can->SetBorderMode(0);
  can->SetBorderSize(0);
  can->SetLeftMargin(0.18);
  can->SetRightMargin(0.1);
  can->SetBottomMargin(0.1518219);
  // can->SetTopMargin(0.);
  can->SetFrameBorderMode(0);
}
//-------------------------------------------------------------------------------------------------------------------------------------//
void Common(){
}
//-------------------------------------------------------------------------------------------------------------------------------------//
