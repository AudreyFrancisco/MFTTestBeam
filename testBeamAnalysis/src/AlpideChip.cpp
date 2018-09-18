//Check The .hpp for the description of the class
#include "AlpideChip.hpp"
#include "Riostream.h"
using namespace std;

ClassImp(AlpideChip)

// default constructor
//______________________________________________________________________
AlpideChip::AlpideChip()
:TObject(),
fChipID(-1),
fNumberOfClusters(0),
fNumberOfPixelsX(-1),
fNumberOfPixelsY(-1),
fChipPositionX(-1.),
fChipPositionY(-1.),
fChipPositionZ(-1.),
fTheta(0),
fChipInitResoX(0),
fChipInitResoY(0),
fOriginCol(512),
fOriginRow(256),
fPixelSizeX(0.02924),
fPixelSizeY(0.02688),
fRotationX(1.),
fRotationY(1.),
fNumberOfHitPixels(-1),
fArrayHitPixels(0),
fArrayClusters(0)
{
  fArrayClusters=new TClonesArray(AlpideCluster::Class());
  fArrayHitPixels=new TClonesArray(AlpidePixel::Class());
}

//constructor
//______________________________________________________________________
AlpideChip::AlpideChip(Int_t chipID, int nPixelsX, int nPixelsY, Float_t xPosition, Float_t yPosition, Float_t zPosition)
:TObject(),
fChipID(chipID),
fNumberOfClusters(0),
fNumberOfPixelsX(nPixelsX),
fNumberOfPixelsY(nPixelsY),
fChipPositionX(xPosition),
fChipPositionY(yPosition),
fChipPositionZ(zPosition),
fTheta(0),
fChipInitResoX(0),
fChipInitResoY(0),
fOriginCol(512),
fOriginRow(256),
fPixelSizeX(0.0294),
fPixelSizeY(0.02688),
fRotationX(1.),
fRotationY(1.),
fNumberOfHitPixels(-1),
fArrayHitPixels(0),
fArrayClusters(0)
{
  fArrayClusters=new TClonesArray(AlpideCluster::Class());
  fArrayHitPixels=new TClonesArray(AlpidePixel::Class());
}

// add Hit pixel to the chip
//______________________________________________________________________
void AlpideChip::AddHitPixel(AlpidePixel *pixel) {
  //Check if similar pixel is already added to the chip. If yes, do not add it
  for(Int_t iPresentPixel=0;iPresentPixel<fNumberOfHitPixels;iPresentPixel++){
    AlpidePixel *presentPixel = (AlpidePixel*)GetHitPixels()->ConstructedAt(iPresentPixel);
    if( (presentPixel->GetCol() == pixel->GetCol()) && (presentPixel->GetRow() == pixel->GetRow()) ){
      return;
    }
  }

  Double_t x = fChipPositionX+fRotationX*(fOriginCol - pixel->GetCol() -0.5)*fPixelSizeX;
  Double_t y = fChipPositionY+fRotationY*(fOriginRow - pixel->GetRow() -0.5)*fPixelSizeY;

  pixel->SetX(x*TMath::Cos(fTheta) - y*TMath::Sin(fTheta));
  pixel->SetY(x*TMath::Sin(fTheta) + y*TMath::Cos(fTheta));

  AlpidePixel *pixelToBeAdded = (AlpidePixel*)fArrayHitPixels->ConstructedAt(fNumberOfHitPixels++);
  *pixelToBeAdded = *pixel;
}

// add cluster to the chip
//______________________________________________________________________
void AlpideChip::AddCluster(AlpideCluster *cluster) {

  AlpideCluster *clusterToBeAdded = (AlpideCluster*)fArrayClusters->ConstructedAt(fNumberOfClusters++);
  *clusterToBeAdded = *cluster;
}

Bool_t AlpideChip::IsAPointInACluster(Float_t xCoordinate, Float_t yCoordinate, Int_t &clusterIndex){
  //Convert the spacial coordiantes into pixels point (x,y)>>>(col,row)
  //TODO: Maybe add options to handle this check in different ways. MAYBE the track extrapolation point must be a hit in the cluster and not simply in the rectangke entourage. Maybe the netouage is a 2-d gauss not a rectangle.
  for(int iCluster=0;iCluster<fNumberOfClusters;iCluster++){
    AlpideCluster *cluster = GetCluster(iCluster);
    // cout<<Form("in chip %d: %3.3f, %3.3f",fChipID,cluster->GetCenterOfMassX(),cluster->GetCenterOfMassY())<<endl;
    if(  TMath::Abs(cluster->GetCenterOfMassX() - xCoordinate) < fChipInitResoX)
    {
      if(  TMath::Abs(cluster->GetCenterOfMassY() - yCoordinate) < fChipInitResoY){
        clusterIndex = iCluster;
        return kTRUE;
      }

    }
  }
  return kFALSE;
}
void AlpideChip::PrintRepeatedPixels(){
  for(int iHitPixel = 0;iHitPixel<fNumberOfHitPixels;iHitPixel++){
    AlpidePixel *mainPixel = (AlpidePixel*)GetHitPixels()->ConstructedAt(iHitPixel);
    for(int jHitPixel=0;jHitPixel<fNumberOfHitPixels;jHitPixel++){
      if(jHitPixel == iHitPixel) continue;
      AlpidePixel *secondaryPixel = (AlpidePixel*)GetHitPixels()->ConstructedAt(jHitPixel);
      if( (secondaryPixel->GetCol() == mainPixel->GetCol()) && (secondaryPixel->GetRow() == mainPixel->GetRow()) ){
        cout<<Form("Repeated pixel in chip %d: col %d, row %d",fChipID,mainPixel->GetCol(),mainPixel->GetRow())<<endl;
      }
    }
  }
}

void AlpideChip::ClustersRecInChip(){
  AlpidePixel *mainPixel;
  // AlpideCluster *cluster;
  AlpidePixel *secondaryPixel;

  for(int iHitPixel = 0;iHitPixel<fNumberOfHitPixels;iHitPixel++){
    mainPixel = (AlpidePixel*)GetHitPixels()->ConstructedAt(iHitPixel);

    //No cluster here yet, just the hit mpas for each chip

    if(mainPixel->IsPixelAddedToCluster()){
      continue;
    }

    AlpideCluster *cluster = new AlpideCluster();
    // cluster->ResetCluster();
    cluster->AddPixel(mainPixel);
    Bool_t clusterIsUpdated = kTRUE;

    //keep re-looping over the hit pixels until no new pixels are added to the cluster
    while(clusterIsUpdated){
      clusterIsUpdated = kFALSE;
      for(int jHitPixel = 0;jHitPixel<fNumberOfHitPixels;jHitPixel++){
        if(jHitPixel == iHitPixel) continue;

        secondaryPixel = (AlpidePixel*)GetHitPixels()->ConstructedAt(jHitPixel);
        if(secondaryPixel->IsPixelAddedToCluster()) continue;
        if(cluster->IsPixelTouchTheCluster(secondaryPixel)){
          cluster->AddPixel(secondaryPixel);
          clusterIsUpdated = kTRUE;
        }

      }//end loop over the hit pixels
    }//end while

    AddCluster(cluster);//this for the tree
  }
}
// reset the chip
//______________________________________________________________________
void AlpideChip::ResetChip() {
  fNumberOfClusters=0;
  fNumberOfHitPixels=0;
  fArrayHitPixels->Clear();
  fArrayClusters->Clear();
}
