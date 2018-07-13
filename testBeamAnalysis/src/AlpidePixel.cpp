//Check The .hpp for the description of the class
#include "AlpidePixel.hpp"

using namespace std;

ClassImp(AlpidePixel)

//______________________________________________________________________
AlpidePixel::AlpidePixel()
:TObject(),
fCol(-1),
fRow(-1),
fNumberOfHits(0),
fAddedToCluster(kFALSE)
{
}

//______________________________________________________________________
AlpidePixel::AlpidePixel(Int_t row, Int_t col, Int_t nHits)
:TObject(),
fCol(col),
fRow(row),
fNumberOfHits(nHits),
fAddedToCluster(kFALSE)
{
}

//Check if two hit pixels are neighbours
Bool_t AlpidePixel::IsTwoPixelsTouched(AlpidePixel *pixel){
  Int_t yDistance = TMath::Abs(fRow-pixel->GetRow());
  Int_t xDistance = TMath::Abs(fCol-pixel->GetCol());
  if( (xDistance <= 1) && (yDistance <= 1) ) return kTRUE;
  return kFALSE;
}

//______________________________________________________________________
void AlpidePixel::Reset() {
  fCol=-1;
  fRow=-1;
  fNumberOfHits=0;
  fAddedToCluster = kFALSE;
}
