/**
A cluster class. In the current work, a cluster is an ensemble of adjacent hit pixels in one chip.
TODO: Is there any adjacemet parameter ? (two pixels must be touching to be considered neighbours ?)
TODO: What about pixels at the border of two chips ?
TODO: For consistency, rename X,Y to col,row in order not to mix between numbering and space coordinates.
*/
#ifndef ALPIDECLUSTER_HPP
#define ALPIDECLUSTER_HPP

//needed root classes
#include <TObject.h>
#include "TClonesArray.h"
//needed user defined classes
#include "AlpidePixel.hpp"

using namespace std;

class AlpideCluster: public TObject {

private:
  Int_t   fNumberOfPixels;       /**< number of hit pixels in the cluster. TODO: Is there a maximum number fot the pixels in one cluster ?*/
  TClonesArray *fArrayPixels;    //A clone array of the pixels
  Double_t fCenterOfMassX;
  Double_t fCenterOfMassY;
  Double_t fSpreadX;
  Double_t fSpreadY;

public:
  //Constructors
    AlpideCluster();
    AlpideCluster(Int_t nPixels);
    virtual ~AlpideCluster();

    //GETTERS
    Int_t         GetNumberOfPixels() const {return fNumberOfPixels;}
    TClonesArray* GetPixelArray	() const {return fArrayPixels;}
    AlpidePixel	   *GetPixel(Int_t pixelIndex) {return static_cast<AlpidePixel*>(fArrayPixels->UncheckedAt(pixelIndex));}
    Int_t       GetCenterOfMassCol();	/**< X position of c.m. of cluster signal. For the moment, the weight of each hit is 1 . (right ?)*/
    Int_t       GetCenterOfMassRow();	/**< Y position of c.m. of cluster signal*/
    Int_t         GetColSpread(); /**< the max x-distance between the pixels*/
    Int_t         GetRowSpread(); /**< the max y-distance between the pixels*/
    Int_t       GetTotalSpread(); /**< the max xy-distance between the pixels*/
    Double_t         GetCenterOfMassX();
    Double_t         GetCenterOfMassY();
    Double_t         GetSpreadX();
    Double_t         GetSpreadY();

    //setters
    void            SetCenterOfMassX(Double_t x) {fCenterOfMassX = x;}
    void            SetCenterOfMassY(Double_t y) {fCenterOfMassY = y;}
    void            SetSpreadX(Double_t x) {fSpreadX = x;}
    void            SetSpreadY(Double_t y) {fSpreadY = y;}



    void AddPixel	 (AlpidePixel *pixel);	// add hit pixel to the cluster. The number of the hit pixels (fNumberOfPixels) will increase upon the call.*/
    Bool_t IsPixelTouchTheCluster(AlpidePixel *pixel);// Check if a pixel touch pixels in the clusters.

    //No need so far to implement setters since the AddPixel() function will do the job.

    void ResetCluster();

    ClassDef(AlpideCluster,2);
};

#endif
