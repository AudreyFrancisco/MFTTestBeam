/**
A simple class for a pixel. A pixel is represented by its row and column in the single sensor (chip). Currently, a row is in the X direction while Col is in the Y direction.
*/
#ifndef ALPIDEPIXEL_HPP
#define ALPIDEPIXEL_HPP

//needed root classes
#include <TObject.h>
#include "TMath.h"

class AlpidePixel: public TObject {

private:

  //Pixel row and col number.
    Int_t    fRow;
    Int_t    fCol;
    Int_t    fNumberOfHits;

    Double_t fXCoordinate; /**< X coordinate of the pixel in the global coordinate system*/
    Double_t fYCoordinate; /**< Y coordinate of the pixel in the global coordinate system*/

    Bool_t   fAddedToCluster;/**<This flag is used during the clustering, a pixel will not considerd anymore of it is already added to a cluster*/

public:
    //Constructors
    AlpidePixel();
    AlpidePixel(Int_t row, Int_t col, Int_t nHits);
    virtual ~AlpidePixel(){}

    //Getters
    Int_t    GetCol  () const {return fCol;}
    Int_t    GetRow  () const {return fRow;}
    Int_t    GetNumberOfHits  () const {return fNumberOfHits;}

    //Getters
    Double_t    GetXCoordinate  () const {return fXCoordinate;}
    Double_t    GetYCoordinate  () const {return fYCoordinate;}

    //Setters
    void SetColAndRow(Int_t col, Int_t row) {fCol = col; fRow = row;}
    void SetCol(Int_t col) {fCol = col;}
    void SetRow(Int_t row) {fRow = row;}
    void SetNumberOfHits(Int_t nHits) {fNumberOfHits = nHits;}

    void SetXAndY(Double_t x, Double_t y) {fXCoordinate = x; fYCoordinate = y;}
    void SetX(Double_t x) {fXCoordinate = x;}
    void SetY(Double_t y) {fYCoordinate = y;}

    Bool_t   IsTwoPixelsTouched(AlpidePixel *pixel);/**< Check if two pixels touch each other. A function used in the clustering.*/
    Bool_t   IsPixelAddedToCluster() const {return fAddedToCluster;}/**< Check if a pixel is already added to a cluster*/
    void FlagPixelAsAddedToCluster() {fAddedToCluster = kTRUE;}

    void Reset();
    ClassDef(AlpidePixel,1);
};

#endif
