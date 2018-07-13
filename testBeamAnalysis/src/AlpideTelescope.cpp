//Check The .hpp for the description of the class
#include "AlpideTelescope.hpp"

using namespace std;

ClassImp(AlpideTelescope);

// default constructor
//______________________________________________________________________
AlpideTelescope::AlpideTelescope()
 :TObject(),
  fTelescopeID(-1),
  fNumberOfChips(0),
  fArrayChips(0),
  fNumberOfTelescopeTracks(0),
  fArrayTelescopeTracks(0)
{
  fArrayChips =new TClonesArray(AlpideChip::Class());
  fArrayTelescopeTracks =new TClonesArray(TelescopeTrack::Class());
}

// add plane to telescope
//______________________________________________________________________
void AlpideTelescope::AddChip(AlpideChip *chip) {
  AlpideChip *chipToBeAdded = (AlpideChip*)fArrayChips->ConstructedAt(fNumberOfChips++);
  *chipToBeAdded = *chip;
}

// add track to telescope
//______________________________________________________________________
void AlpideTelescope::AddTelescopeTrack(TelescopeTrack *track) {
  TelescopeTrack *trackToBeAdded = (TelescopeTrack*)fArrayTelescopeTracks->ConstructedAt(fNumberOfTelescopeTracks++);
  *trackToBeAdded = *track;
}

// get specific plane with index idx from
//______________________________________________________________________
AlpideChip* AlpideTelescope::GetChip(Int_t index) {
  return static_cast<AlpideChip*>(fArrayChips->UncheckedAt(index));
}
AlpideChip* AlpideTelescope::GetChipByID(Int_t chipID) {
  for(int iChip=0;iChip<fNumberOfChips;iChip++){
    if((GetChip(iChip)->GetChipID())==chipID){
      return static_cast<AlpideChip*>(fArrayChips->UncheckedAt(iChip));
    }
  }
  return 0x0;
}

TelescopeTrack* AlpideTelescope::GetTelescopeTrack(Int_t index) {
  return static_cast<TelescopeTrack*>(fArrayTelescopeTracks->UncheckedAt(index));
}
void AlpideTelescope::ResetTelescope(){
  fNumberOfChips = 0;
  fNumberOfTelescopeTracks=0;
  fArrayChips->Clear();
  fArrayTelescopeTracks->Clear();
}
