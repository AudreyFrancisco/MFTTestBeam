/**
 * This class handle the reading of the .txt (or .dat) file and so that the hit info can be stored.
 //This class is obsolete for the analysis of real data since hits are now stored in a TTree instead of .dat. This class however is needed to read generated files for tests purpose.
 */

#ifndef EVENTREADER_HPP
#define EVENTREADER_HPP

//needed root classes
#include "Riostream.h"
#include "TObject.h"

class EventReader: public TObject {
private:
  ifstream fInputFile; /**< input text file. it can be .txt, or other extensions (e.g .dat)*/
  Bool_t   fFirstEvent;/**<This is true when creating the eventReader object and is toogled to false when the first event is read.*/
  Bool_t   fLastEvent;
  Int_t    fHitPixelsIter;
  Int_t    fEventCounter;    /**< internal event counter (= event ID of next event)*/
  Int_t    fCurrentEvent;   /**<current event ID*/

  std::vector<Int_t> fVectorHitPixelsDetectorID;
  std::vector<Int_t> fVectorHitPixelsChipID;
  std::vector<Int_t> fVectorHitPixelsFirstAddress;
  std::vector<Int_t> fVectorHitPixelsSecondAddress;
  std::vector<Int_t> fVectorHitPixelsNHits;

public:
  EventReader();
  ~EventReader();

  Bool_t SetInputFile(const char *fileName);
  Bool_t ReadEvent();

  Bool_t IsLastEvent()     { return fLastEvent; }
  Int_t  GetEventCounter() { return fCurrentEvent; }
  Int_t  GetNumberOfHitPixels()      { return fVectorHitPixelsFirstAddress.size(); }//The size of the 5 vectors is similar
  Bool_t GetNextHitPixel(Int_t *arrayDetectorID, Int_t *arrayChipID, Int_t *arrayFirstAddress, Int_t* arraySecondAddress, Int_t* arrayNHits);
  void   ResetHitPixelsIter()    { fHitPixelsIter = 0; }
  Bool_t CheckDoubleHitPixels(Bool_t remove);




  ClassDef(EventReader,2)
};

#endif
