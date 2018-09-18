/*
A chip class that consists the elements of an AlpideLadder and AlpideTelescope. The class contains dimensions and positions of the chip, an array of hit pixels, and an array of reconstructed clusters and a function to test if a point in (X,Y) corresponds to a cluster in the chip. In ladders, a chip might be either on the face side or on the back ones, the column numbering in the chip depends on this oreintation.
TODO: So far the function IsAPointInACluster() check if the point (track extrapolation) is in the rectangle domain of the cluster (XSpread-YSpread). This might be changed (Gaussian distribution ?)
*/

#ifndef ALPIDECHIP_HPP
#define ALPIDECHIP_HPP

//needed root classes
#include "TObject.h"
#include "TClonesArray.h"
#include "TRef.h"

//needed user defined classes
#include "AlpideCluster.hpp"
#include "AlpidePixel.hpp"

class AlpideChip:public TObject {

private:
    Int_t		  fChipID;	// Chip id
    Int_t		  fNumberOfPixelsX;	// number of rows
    Int_t		  fNumberOfPixelsY;	// number of columns
    Float_t	  fChipPositionX; 		// x chip position of the bottom left (x_min and y_min of the rectangle) corner in the lab frame in mm
    Float_t	  fChipPositionY;		  // y ////////////////////////////////////////////////////
    Float_t 	fChipPositionZ; 		// z ///////////////////////////////////////////////////
    Float_t   fTheta;             //Chip rotation around the Z-axis
    Float_t	  fChipSizeX;		  // x size of the sensor in mm
    Float_t 	fChipSizeY; 		// y ////////////////////////
    Float_t	  fChipInitResoX;		  // distance to search for a track in mm
    Float_t 	fChipInitResoY; 		// distance to search for a track in mm
    Int_t     fOriginCol;     //The col number of the geometrical origion of the chip, taken as the top left corner
    Int_t     fOriginRow;     //The col number of the geometrical origion of the chip, taken as the top left corner
    Double_t  fPixelSizeX;    //in mm
    Double_t  fPixelSizeY;    //in mm
    Double_t  fRotationX;
    Double_t  fRotationY;


    Int_t   fNumberOfHitPixels;     // number of hit pixels found in the chip
    TClonesArray *fArrayHitPixels;    //array of the hit pixels in the chip

    Int_t		fNumberOfClusters;	// number of clusters reconstructed in the chip
    TClonesArray *fArrayClusters; //-> array containg all clusters in the chip

public:
  //constructors
    AlpideChip   		();
    AlpideChip			(Int_t chipID, int nPixelsX, int nPixelsY, Float_t xPosition, Float_t yPosition, Float_t zPosition);
    virtual ~AlpideChip	() {delete fArrayClusters;}

    //Getters
    Int_t         GetChipID() {return fChipID;}
    TClonesArray  *GetHitPixels	() const {return fArrayHitPixels;}
    AlpidePixel	    *GetHitPixel		(Int_t hitPixelIndex) {return static_cast<AlpidePixel*>(fArrayHitPixels->UncheckedAt(hitPixelIndex));}
    AlpideCluster	  *GetCluster		(Int_t clusterIndex) {return static_cast<AlpideCluster*>(fArrayClusters->UncheckedAt(clusterIndex));}
    TClonesArray 	*GetClusters	() const {return fArrayClusters;}
    Int_t 	      GetNumberOfClusters	() const {return fNumberOfClusters;}
    Int_t 	      GetNumberOfPixelsX   		() const {return fNumberOfPixelsX;}
    Int_t 	      GetNumberOfPixelsY   		() const {return fNumberOfPixelsY;}
    Int_t       	GetNumberOfHitPixels   		() const {return fNumberOfHitPixels;}
    Float_t 	    GetPositionX    		() const {return fChipPositionX;}
    Float_t 	    GetPositionY    		() const {return fChipPositionY;}
    Float_t 	    GetPositionZ	 	  	() const {return fChipPositionZ;}
    Float_t 	    GetTHeta	     	  	() const {return fTheta;}
    Float_t 	    GetSizeX    		() const {return fChipSizeX;}
    Float_t 	    GetSizeY    		() const {return fChipSizeY;}
    Float_t 	    GetInitResoX    		() const {return fChipInitResoX;}
    Float_t 	    GetInitResoY    		() const {return fChipInitResoY;}
    Int_t 	      GetOriginCol   		() const {return fOriginCol;}
    Int_t       	GetOriginRow   		() const {return fOriginRow;}
    Double_t 	      GetPixelSizeX   		() const {return fPixelSizeX;}
    Double_t       	GetPixelSizeY   		() const {return fPixelSizeY;}
    Double_t 	      GetRotationX   		() const {return fRotationX;}
    Double_t       	GetRotationY   		() const {return fRotationY;}


    //Setters
    void SetChipID   	 (Int_t chipID)	 {fChipID=chipID;}
    void SetNumberOfPixelsX   	 (Int_t nPixelsX)	 {fNumberOfPixelsX=nPixelsX;}
    void SetNumberOfPixelsY   	 (Int_t nPixelsY)	 {fNumberOfPixelsY=nPixelsY;}
    void SetNumberOfHitPixels		 (Int_t nHitPixels)     {fNumberOfHitPixels=nHitPixels;}
    void SetPositionX    	 (Float_t xPosition)		 {fChipPositionX=xPosition;}
    void SetPositionY    	 (Float_t yPosition)   	 {fChipPositionY=yPosition;}
    void SetPositionZ    	 (Float_t zPosition)	 	 {fChipPositionZ=zPosition;}
    void SetTheta       	 (Float_t theta)	 	 {fTheta=theta;}
    void SetSizeX    	 (Float_t xSize)		 {fChipSizeX=xSize;}
    void SetSizeY    	 (Float_t ySize)   	 {fChipSizeY=ySize;}
    void SetInitResoX    	 (Float_t xReso)		 {fChipInitResoX=xReso;}
    void SetInitResoY    	 (Float_t yReso)   	 {fChipInitResoY=yReso;}
    void SetOriginCol   	 (Int_t col)	 {fOriginCol=col;}
    void SetOriginRow   	 (Int_t row)	 {fOriginRow=row;}
    void SetPixelSizeX   	 (Int_t xSize)	 {fPixelSizeX=xSize;}
    void SetPixelSizeY   	 (Int_t ySize)	 {fPixelSizeY=ySize;}
    void SetRotationX   	 (Int_t xRotation)	 {fRotationX=xRotation;}
    void SetRotationY   	 (Int_t yRotation)	 {fRotationY=yRotation;}


    void AddCluster	 (AlpideCluster *cluster);	// add reconstructed cluster to chip
    void AddHitPixel	 (AlpidePixel *pixel);	// add hit pixel to chip
    Bool_t IsAPointInACluster(Float_t xCoordinate, Float_t yCoordinate, Int_t &clusterIndex);//See TODO above
    void ClustersRecInChip();
    void PrintRepeatedPixels();
    void ResetChip		 ();


    ClassDef(AlpideChip,1);
};

#endif
