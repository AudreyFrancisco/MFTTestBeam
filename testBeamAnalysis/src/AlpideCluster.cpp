//Check The .hpp for the description of the class
#include "AlpideCluster.hpp"
#include "TMath.h"
#include "Riostream.h"
using namespace std;

ClassImp(AlpideCluster)

// default constructor
AlpideCluster::AlpideCluster()
:TObject(),
fNumberOfPixels(0),
fArrayPixels(0)
{
  fArrayPixels=new TClonesArray(AlpidePixel::Class());
}

AlpideCluster::AlpideCluster(Int_t nPixels)
:TObject(),
fNumberOfPixels(nPixels)
{
  fArrayPixels=new TClonesArray(AlpidePixel::Class());
}

// destructor
//______________________________________________________________________
AlpideCluster::~AlpideCluster() {
  delete fArrayPixels;
}

// get col position of center of mass of cluster
//______________________________________________________________________
Int_t AlpideCluster::GetCenterOfMassCol() {
  //The weight here is one (binary not analogue)
  //This function and the three below might be changed if the X,Y do not correspond for Row,Col
  Float_t sumPositionCol=0.;
  for (Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++)
  sumPositionCol += GetPixel(iPixel)->GetCol();
  return sumPositionCol/fNumberOfPixels;
}

// get y position of center of mass of cluster
//______________________________________________________________________
Int_t AlpideCluster::GetCenterOfMassRow() {
  //The weight here is one (binary not analogue)
  Float_t sumPositionRow=0.;
  for (Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++)
  sumPositionRow += GetPixel(iPixel)->GetRow();
  return sumPositionRow/fNumberOfPixels;
}

// get cluster height
//______________________________________________________________________
Int_t AlpideCluster::GetColSpread() {
  Int_t min = 99999, max = -99999;
  for(Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++) {
    Int_t pixelCol = GetPixel(iPixel)->GetCol();
    if(pixelCol > max) max = pixelCol;
    if(pixelCol < min) min = pixelCol;
  }
  return max-min+1;
}

// get cluster width
//______________________________________________________________________
Int_t AlpideCluster::GetRowSpread() {
  Int_t min = 99999, max = -99999;
  for(Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++) {
    Int_t pixelRow = GetPixel(iPixel)->GetRow();
    if(pixelRow > max) max = pixelRow;
    if(pixelRow < min) min = pixelRow;
  }
  return max-min+1;
}

// get maximum distance between two pixels
//______________________________________________________________________
Int_t AlpideCluster::GetTotalSpread() {
  Float_t max = 0.;
  for(Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++) {
    for(Int_t jPixel=iPixel+1; jPixel<fNumberOfPixels; jPixel++) {
      Float_t distance = TMath::Sqrt(
        (GetPixel(iPixel)->GetCol()-GetPixel(jPixel)->GetCol())*(GetPixel(iPixel)->GetCol()-GetPixel(jPixel)->GetCol()) +
        (GetPixel(iPixel)->GetRow()-GetPixel(jPixel)->GetRow())*(GetPixel(iPixel)->GetRow()-GetPixel(jPixel)->GetRow()) );
        if(distance > max) max = distance;
      }
    }
    return max;
  }

  // get X position of center of mass of cluster
  //______________________________________________________________________
  Double_t AlpideCluster::GetCenterOfMassX() {
    //The weight here is one (binary not analogue)
    //This function and the three below might be changed if the X,Y do not correspond for Row,Col
    Float_t sumPositionX=0.;
    for (Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++)
    sumPositionX += GetPixel(iPixel)->GetXCoordinate();
    return sumPositionX/fNumberOfPixels;
  }

  // get Y position of center of mass of cluster
  //______________________________________________________________________
  Double_t AlpideCluster::GetCenterOfMassY() {
    //The weight here is one (binary not analogue)
    //This function and the three below might be changed if the X,Y do not correspond for Row,Col
    Float_t sumPositionY=0.;
    for (Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++)
    sumPositionY += GetPixel(iPixel)->GetYCoordinate();
    return sumPositionY/fNumberOfPixels;
  }
  // get cluster width
  //______________________________________________________________________
  Double_t AlpideCluster::GetSpreadX() {
    Double_t min = 99999, max = -99999;
    Double_t pixelX;
    for(Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++) {
      pixelX = GetPixel(iPixel)->GetXCoordinate();
      if(pixelX > max) max = pixelX;
      if(pixelX < min) min = pixelX;
    }
    return max-min;
  }
  // get cluster height
  //______________________________________________________________________
  Double_t AlpideCluster::GetSpreadY() {
    Double_t min = 99999, max = -99999;
    Double_t pixelY;
    for(Int_t iPixel=0; iPixel<fNumberOfPixels; iPixel++) {
      pixelY = GetPixel(iPixel)->GetYCoordinate();
      if(pixelY > max) max = pixelY;
      if(pixelY < min) min = pixelY;
    }
    return max-min;
  }

  // add Hit pixel to the chip
  //______________________________________________________________________
  void AlpideCluster::AddPixel(AlpidePixel *pixel) {
    //what abot the max numbers of the hit pixels in the clusters? Should all the clusters be ignored of such a limit exists ? Or stop considering the new pixels while clustering ? If the last is the case, this function should have a condition
    AlpidePixel *pixelToBeAdded = (AlpidePixel*)fArrayPixels->ConstructedAt(fNumberOfPixels++);
    *pixelToBeAdded = *pixel;
    pixel->FlagPixelAsAddedToCluster();
  }

  //Check if the pixel is neighbour with at least one pixel in the cluster
  Bool_t AlpideCluster::IsPixelTouchTheCluster(AlpidePixel *pixel){
    for(int iPixel=0;iPixel<fNumberOfPixels;iPixel++){
      AlpidePixel *pixelInTheCluster = GetPixel(iPixel);
      if(pixelInTheCluster->IsTwoPixelsTouched(pixel)) return kTRUE;
    }
    return kFALSE;
  }
  //______________________________________________________________________
  void AlpideCluster::ResetCluster() {
    fArrayPixels->Clear();
    fNumberOfPixels = 0;
  }
