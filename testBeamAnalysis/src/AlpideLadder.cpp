//Check The .hpp for the description of the class
#include "AlpideLadder.hpp"

using namespace std;

ClassImp(AlpideLadder);

// default constructor
//______________________________________________________________________
AlpideLadder::AlpideLadder()
:TObject(),
fLadderID(-1),
fNumberOfChips(0),
fArrayChips(0)
{
  fArrayChips =new TClonesArray(AlpideChip::Class());
}

//constructor
//______________________________________________________________________
AlpideLadder::AlpideLadder(Int_t ladderID)
:TObject(),
fLadderID(ladderID),
fNumberOfChips(0),
fArrayChips(0)
{
  fArrayChips =new TClonesArray(AlpideChip::Class());
}

// add plane to event
//______________________________________________________________________
void AlpideLadder::AddChip(AlpideChip *chip) {
  AlpideChip *chipToBeAdded = (AlpideChip*)fArrayChips->ConstructedAt(fNumberOfChips++);
  *chipToBeAdded = *chip;
}

// get specific chip with index idx from the ladder
//______________________________________________________________________
AlpideChip* AlpideLadder::GetChip(Int_t index) {
  return static_cast<AlpideChip*>(fArrayChips->UncheckedAt(index));
}
// get specific chip using its chip ID
//______________________________________________________________________
AlpideChip* AlpideLadder::GetChipByID(Int_t chipID) {
  for(int iChip=0;iChip<fNumberOfChips;iChip++){
    if((GetChip(iChip)->GetChipID())==chipID){
      return static_cast<AlpideChip*>(fArrayChips->UncheckedAt(iChip));
    }
  }
  return 0x0;
}

void AlpideLadder::ResetLadder(){
  fNumberOfChips = 0;
  fArrayChips->Clear();
}
