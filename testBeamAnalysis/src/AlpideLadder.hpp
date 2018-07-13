/**
A class for a MFT ladder. The ladder contains many AlpideChips (2,3,or 4). In addition to the array of chips and their number, the ladder class has an a variable that indicates whether the ladder is on the face or the back side of the disk so that the numbering of the columns can be set.
TODO: This class was developped before knowing that the telescope used in the test beam is also based on alpide chips, one cab think of making a detector class that can be either ladder or a telescope.
*/
#ifndef ALPIDELADDER_HPP
#define ALPIDELADDER_HPP

//needed root classes
#include "TObject.h"
#include "TClonesArray.h"
#include "TRef.h"
//needed user defined classes
#include "AlpideChip.hpp"


class AlpideLadder:public TObject {
private:
  Int_t   fLadderID; /**< This id must match the id system in the raw file*/
  Int_t		fNumberOfChips;	                /**<  number of chips in the ladder.*/
  TClonesArray  *fArrayChips;             /**<  A vector of chips can be used but tobjarray may be more flexible in the further analysis.*/

public:
  //Constructors. For the moment, No need to add more complicated constructors since the default one is enough for the purpose of the analysis.
  AlpideLadder();
  AlpideLadder(Int_t ladderID);
  virtual ~AlpideLadder() {}

  //Getters
  Int_t GetLadderID() const {return fLadderID;}
  Int_t GetNumberOfChips() const {return fNumberOfChips;}
  AlpideChip	 *GetChip(Int_t index);
  AlpideChip	 *GetChipByID(Int_t chipID);
  TClonesArray *GetArrayChips	() const {return fArrayChips;}

  //Setters
  void SetLadderID(Int_t ladderID) {fLadderID=ladderID;}

  void AddChip(AlpideChip *chip);

  //implemented in .cpp but not used so far.
  void ResetLadder();

  ClassDef(AlpideLadder,1); // Event structure
};

#endif
