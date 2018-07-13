/**
A class for an event of MFT test beam. An event has an id, an array of ladders, and a telescope.
*/
#ifndef TESTBEAMEVENT_HPP
#define TESTBEAMEVENT_HPP

//needed root classes
#include "TObject.h"
#include "TClonesArray.h"
#include "TRef.h"
//needed user defined classes
#include "AlpideLadder.hpp"
#include "AlpideTelescope.hpp"



class TestBeamEvent:public TObject {
private:

  Int_t		fEventID;		                    /**<  event ID number*/
  Int_t		fNumberOfLadders;	                /**<  number of ladders in the event. Probably this will be common for all the events.*/
  TClonesArray  *fArrayLadders;             /**<  A vector of AlpideLadders can be used but tobjarray may be more flexible in the further analysis.*/
  AlpideTelescope *fTelescope; /**< A class member where the telescope tracks and chips are stored. Only w-one telescope is supposed to be in the setup*/


public:
  //Constructors. For the moment, No need to add more complicated constructors since the default one is enough for the purpose of the analysis.
  TestBeamEvent();
  TestBeamEvent(int eventID);
  virtual ~TestBeamEvent() {}

  //Getters
  Int_t GetEventID()               const {return fEventID;}
  Int_t GetNumberOfLadders() const {return fNumberOfLadders;}
  AlpideTelescope *GetTelescope() const {return fTelescope;}
  AlpideLadder	 *GetLadder(Int_t index);
  AlpideLadder	 *GetLadderById(Int_t ladderID);
  TClonesArray *GetArrayLadders	() const {return fArrayLadders;}


  //Two functions to add ladders for the event. The description of the two function is in the .cpp, the number of laddersof the evnt will increase upon the call these functions.  TODO maybe define a functions (SETTERS) that add the objectarray at once.
  void AddLadder(AlpideLadder *ladder);

  //Setter
  void SetEventID(Int_t eventID)		 {fEventID=eventID;}
  void SetTelescope(AlpideTelescope *telescope) {fTelescope=telescope;}

  //implemented in .cpp but not used so far.
  void ResetEvent();


  ClassDef(TestBeamEvent,1); // Event structure
};

#endif
