//Check The .hpp for the description of the class
#include "TestBeamEvent.hpp"

using namespace std;

ClassImp(TestBeamEvent);

// default constructor
//______________________________________________________________________
TestBeamEvent::TestBeamEvent()
 :TObject(),
  fEventID(-1),
  fNumberOfLadders(0),
  fArrayLadders(0),
  fTelescope(0)
{
  fArrayLadders =new TClonesArray(AlpideLadder::Class());
  fTelescope = new AlpideTelescope();
}

//______________________________________________________________________
TestBeamEvent::TestBeamEvent(int eventID)
 :TObject(),
 fEventID(eventID),
 fNumberOfLadders(0),
 fArrayLadders(0),
 fTelescope(0)
{
  fArrayLadders =new TClonesArray(AlpideLadder::Class());
  fTelescope = new AlpideTelescope();
}
// add plane to event
//______________________________________________________________________
void TestBeamEvent::AddLadder(AlpideLadder *ladder) {
  AlpideLadder *ladderToBeAdded = (AlpideLadder*)fArrayLadders->ConstructedAt(fNumberOfLadders++);
  *ladderToBeAdded = *ladder;
}


// get specific ladder with index idx from event
//______________________________________________________________________
AlpideLadder* TestBeamEvent::GetLadder(Int_t index) {
  return static_cast<AlpideLadder*>(fArrayLadders->UncheckedAt(index));
}

// get specific ladder with id from event
//______________________________________________________________________
AlpideLadder* TestBeamEvent::GetLadderById(Int_t ladderID) {
  for(Int_t iLadder =0;iLadder<fNumberOfLadders;iLadder++){
    if((GetLadder(iLadder)->GetLadderID()) == ladderID ){
      return static_cast<AlpideLadder*>(fArrayLadders->UncheckedAt(iLadder));
    }
  }
  return 0x0;
}


void TestBeamEvent::ResetEvent(){
  fEventID = -1;
  fNumberOfLadders = 0;
  fArrayLadders->Clear();
  fTelescope->ResetTelescope();
}
