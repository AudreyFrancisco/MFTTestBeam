//Check The .hpp for the description of the class
#include "EventReader.hpp"
#include "Riostream.h"
#include "TObjString.h"
#include "TObjArray.h"

using namespace std;
//__________________________________________________________
EventReader::EventReader():TObject(),
fEventCounter(0),
fFirstEvent(1),
fLastEvent(0),
fHitPixelsIter(0)
{
  // Construct. Reserve do not mean that other hits will not be saved. It is only for the memory allocation ?
  fVectorHitPixelsDetectorID.reserve(1000);
  fVectorHitPixelsChipID.reserve(1000);
  fVectorHitPixelsFirstAddress.reserve(1000);
  fVectorHitPixelsSecondAddress.reserve(1000);
  fVectorHitPixelsNHits.reserve(1000);
}

//__________________________________________________________
EventReader::~EventReader()
{
  fInputFile.close();
  fVectorHitPixelsDetectorID.clear();
  fVectorHitPixelsChipID.clear();
  fVectorHitPixelsFirstAddress.clear();
  fVectorHitPixelsSecondAddress.clear();
  fVectorHitPixelsNHits.clear();
}
//__________________________________________________________
Bool_t EventReader::SetInputFile(const char *filename)
{
  fInputFile.open(filename);
  return fInputFile.is_open();
}

//__________________________________________________________
Bool_t EventReader::ReadEvent()
{
  //This is the main function of the class. Actually the whole class can be replaced by a macro. But a class might be more flexible in case of future modification
  Int_t   event;
  fHitPixelsIter = 0;
  fVectorHitPixelsDetectorID.clear();
  fVectorHitPixelsChipID.clear();
  fVectorHitPixelsFirstAddress.clear();
  fVectorHitPixelsSecondAddress.clear();
  fVectorHitPixelsNHits.clear();

  if(!fInputFile.good()) {
    if(!IsLastEvent())
    cout << "EventReader::ReadEvent() : Error Input File : 1" << endl;
    return kFALSE;
  }


  if(fFirstEvent) {
    fInputFile >> event;
    fEventCounter = event;
    fFirstEvent = kFALSE;
  }
  else {
    event = fEventCounter;
  }
  fCurrentEvent = fEventCounter;

  TString inputLine;
  while(fInputFile.good() && event == fEventCounter) {

    inputLine.ReadLine(fInputFile,kTRUE);
    TObjArray *lineContent = inputLine.Tokenize(" ");
    Int_t lineSize = lineContent->GetEntries();

    //? The readout output is supposed to have events separated by the event id and below the different hit SecondAddresses. This is the part that might be changed if the format of the input file vary.
    if(lineSize == 1){
      event = ((TObjString*)lineContent->UncheckedAt(0))->String().Atoi();
      delete lineContent;
    }
    else if(lineSize == 5){
      fVectorHitPixelsDetectorID.push_back(((TObjString*)lineContent->UncheckedAt(0))->String().Atoi());
      fVectorHitPixelsChipID.push_back(((TObjString*)lineContent->UncheckedAt(1))->String().Atoi());
      fVectorHitPixelsFirstAddress.push_back(((TObjString*)lineContent->UncheckedAt(2))->String().Atoi());
      fVectorHitPixelsSecondAddress.push_back(((TObjString*)lineContent->UncheckedAt(3))->String().Atoi());
      fVectorHitPixelsNHits.push_back(((TObjString*)lineContent->UncheckedAt(4))->String().Atoi());
      delete lineContent;
    }
    else{
      continue;
    }

  }

  if(!fInputFile.good()) fLastEvent = kTRUE;

  fEventCounter = event;

  return kTRUE;
}


//__________________________________________________________
Bool_t EventReader::GetNextHitPixel(Int_t *arrayDetectorID, Int_t *arrayChipID, Int_t *arrayFirstAddress, Int_t* arraySecondAddress, Int_t* arrayNHits) {
  if( fHitPixelsIter < GetNumberOfHitPixels() ) {
    *arrayDetectorID = fVectorHitPixelsDetectorID.at(fHitPixelsIter);
    *arrayChipID = fVectorHitPixelsChipID.at(fHitPixelsIter);
    *arrayFirstAddress = fVectorHitPixelsFirstAddress.at(fHitPixelsIter);
    *arraySecondAddress = fVectorHitPixelsSecondAddress.at(fHitPixelsIter);
    *arrayNHits = fVectorHitPixelsNHits.at(fHitPixelsIter);
    ++fHitPixelsIter;
    return kTRUE;
  }
  else {
    *arrayDetectorID = -99;
    *arrayChipID = -99;
    *arraySecondAddress = -99;
    *arrayFirstAddress = -99;
    *arrayNHits = -99;
    return kFALSE;
  }
}

//__________________________________________________________
Bool_t EventReader::CheckDoubleHitPixels(Bool_t andRemoveIt) {
  // check if there are duplicated pixels (same row and cols) that exist different times in the same event. Remove them if asked.
  //TODO: This function is not used so far in the analysis macro.
  Bool_t doubleHitsFound = kFALSE;
  for(Int_t iHit=0; iHit < GetNumberOfHitPixels(); iHit++){
    for(Int_t jHit=iHit+1; jHit < GetNumberOfHitPixels(); jHit++){
      if(fVectorHitPixelsDetectorID.at(iHit) == fVectorHitPixelsDetectorID.at(jHit))
      if(fVectorHitPixelsChipID.at(iHit) == fVectorHitPixelsChipID.at(jHit))
      if(fVectorHitPixelsFirstAddress.at(iHit) == fVectorHitPixelsFirstAddress.at(jHit))
      if(fVectorHitPixelsSecondAddress.at(iHit) == fVectorHitPixelsSecondAddress.at(jHit)) {
        doubleHitsFound = kTRUE;
        if(andRemoveIt) {
          fVectorHitPixelsDetectorID.erase(fVectorHitPixelsDetectorID.begin()+jHit);
          fVectorHitPixelsChipID.erase(fVectorHitPixelsChipID.begin()+jHit);
          fVectorHitPixelsFirstAddress.erase(fVectorHitPixelsFirstAddress.begin()+jHit);
          fVectorHitPixelsSecondAddress.erase(fVectorHitPixelsSecondAddress.begin()+jHit);
          fVectorHitPixelsNHits.erase(fVectorHitPixelsNHits.begin()+jHit);
          jHit--;
        }
      }
    }
  }
  return doubleHitsFound;


}
