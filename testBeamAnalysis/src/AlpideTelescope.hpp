/**
A class for the alpide telescope. The telescope contains many (single?) alpide chips. It also contains an array of telescope tracks that will be added after fitting the cluster of its chips.
TODO: This class was developped before knowing that the telescope used in the test beam is also based on alpide chips, one cab think of making a detector class that can be either ladder or a telescope.
*/
#ifndef ALPIDETELESCOPE_HPP
#define ALPIDETELESCOPE_HPP

//needed root classes
#include "TObject.h"
#include "TClonesArray.h"
#include "TRef.h"
//needed user defined classes
#include "AlpideChip.hpp"
#include "TelescopeTrack.hpp"


class AlpideTelescope:public TObject {
private:

  Int_t		fTelescopeID;	                /**<  ID of the telescope as it appears in the raw file.*/
  Int_t		fNumberOfChips;	                /**<  number of chips in the telescope.*/
  TClonesArray  *fArrayChips;             /**<  A vector of MTFChips can be used but tobjarray may be more flexible in the further analysis.*/
  Int_t   fNumberOfTelescopeTracks;       /**<  number of telescope tracks. (Do we expect one track per event or what ?)*/
  TClonesArray  *fArrayTelescopeTracks;   /**<  array of tracks from the telescope*/

public:
  //Constructors. For the moment, No need to add more complicated constructors since the default one is enough for the purpose of the analysis.
  AlpideTelescope();
  virtual ~AlpideTelescope() {}

  //Getters
  Int_t GetTelescopeID() const {return fTelescopeID;}
  Int_t GetNumberOfChips() const {return fNumberOfChips;}
  AlpideChip	 *GetChip(Int_t index);
  AlpideChip	 *GetChipByID(Int_t chipID);
  TClonesArray *GetArrayChips	() const {return fArrayChips;}
  Int_t GetNumberOfTelescopeTracks() const {return fNumberOfTelescopeTracks;}
  TelescopeTrack	 *GetTelescopeTrack(Int_t index);
  TClonesArray *GetArrayTelescopeTracks	() const {return fArrayTelescopeTracks;}

  //Setters
  void SetTelescopeID(Int_t telescopeID) {fTelescopeID = telescopeID;}

  void AddChip(AlpideChip *chip);
  void AddTelescopeTrack(TelescopeTrack *track);

  //implemented in .cpp but not used so far.
  void ResetTelescope();


  ClassDef(AlpideTelescope,1); // Event structure
};

#endif
