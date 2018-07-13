
/**
A class for a telescope track. In the current implementation, the track is simply a staright line in 3d space with four parameters (constant and slope for X and Y).
The line equation is: {x = A + B*t},{y = C + D*t}, {z=t}
TODO: Make this class more general (maybe use directly the TTrack from root).
*/
#ifndef TELESCOPETRACK_HPP
#define TELESCOPETRACK_HPP


//needed root classes
#include "TObject.h"
#include "TClonesArray.h"
#include "TGraph2D.h"
#include "TVirtualFitter.h"
#include "TRef.h"
#include "TROOT.h"
#include "TMath.h"
#include "Math/Vector3D.h"
#include "Math/Vector3Dfwd.h"
#include "HFitInterface.h"
#include <Fit/Fitter.h>
#include <Math/Functor.h>


class TelescopeTrack:public TObject {

private:
    Int_t		    fTrackID;	  /**< a track ID. Not used so far since in the evnt the track is identified by its index in the array.*/
    Float_t		  fXConstant;	/**< A in the above line equation*/
    Float_t		  fXSlope;	  /**< B in the above line equation*/
    Float_t		  fYConstant;	/**< C in the above line equation*/
    Float_t		  fYSlope;	  /**< D in the above line equation*/
    Int_t       fNumberOfPoints;
    std::vector<Float_t> fVectorPointsX;/**< The x coordinates of the points added to the track fit*/
    std::vector<Float_t> fVectorPointsY;/**< The y coordinates of the points added to the track fit*/
    std::vector<Float_t> fVectorPointsZ;/**< The z coordinates of the points added to the track fit*/
    Bool_t      fPrintResults; /**< Flag for printing results of the fit. Put to true when testing*/
    Float_t		  fChiSquare;	/**< The track will be fitted using the hits in the telescope. A chisquare cut shall be applied to accept the tracks.*/


public:
    //Constructors
    TelescopeTrack();
    TelescopeTrack(Int_t trackID, Float_t xConstant, Float_t xSlope, Float_t yConstant, Float_t ySlope);
    virtual ~TelescopeTrack	(){}

    //Getters
    Int_t     GetTrackID(){return fTrackID;}
    Float_t 	GetXConstant() const {return fXConstant;}
    Float_t 	GetYConstant() const {return fYConstant;}
    Float_t 	GetXSlope() const {return fXSlope;}
    Float_t 	GetYSlope() const {return fYSlope;}
    Int_t    	GetNumberOfPoints() const {return fNumberOfPoints;}
    std::vector<Float_t> GetVectorPointsX() const {return fVectorPointsX;}
    std::vector<Float_t> GetVectorPointsY() const {return fVectorPointsY;}
    std::vector<Float_t> GetVectorPointsZ() const {return fVectorPointsZ;}
    Float_t 	GetChiSquare() const {return fChiSquare;}


    void GetXYCoordinate(Float_t &xCoordinate, Float_t &yCoordinate, Float_t zCoordiante);/**< A function to get the X,Y coordinate for a given Z coordinate using the line equation.*/

    //Setters
    void SetTrackID(Int_t trackID){fTrackID=trackID;}
    void SetConstantX(Float_t xConstant){fXConstant=xConstant;}
    void SetConstantY(Float_t yConstant){fYConstant=yConstant;}
    void SetSlopeX(Float_t xSlope){fXSlope=xSlope;}
    void SetSlopeY(Float_t ySlope){fYSlope=ySlope;}
    void SetParams(Float_t xConstant,Float_t xSlope, Float_t yConstant , Float_t ySlope){fXConstant=xConstant;fYConstant=yConstant;fXSlope=xSlope;fYSlope=ySlope;}
    void SetChiSquare(Float_t chiSquare){fChiSquare=chiSquare;}
    void AddPoint(Float_t x, Float_t y, Float_t z);/**< Add a point to the ensemble of points to be fitted*/
    void SetPrintResults(Bool_t b){fPrintResults=b;}
    void Reset();

    // function Object to be minimized
    struct SumDistance2Object {
       // the TGraph is a data member of the object
       TGraph2D *lineGraph;

       SumDistance2Object(TGraph2D *g) : lineGraph(g) {}

       Double_t LinePointDistance2(Double_t x,Double_t y,Double_t z, const Double_t *lineParams) {
         // distance line point is D= | (xp-x0) cross  ux |
         // where ux is direction of line and x0 is a point in the line (like t = 0)
         ROOT::Math::XYZVector vectorPoint(x,y,z);
         ROOT::Math::XYZVector vectorPointOnLine1(lineParams[0], lineParams[2], 0. );
         ROOT::Math::XYZVector vectorPointOnLine2(lineParams[0] + lineParams[1], lineParams[2] + lineParams[3], 1. );
         ROOT::Math::XYZVector lineUnitVector = (vectorPointOnLine2-vectorPointOnLine1).Unit();
         Double_t distance2 = ((vectorPoint-vectorPointOnLine1).Cross(lineUnitVector)).Mag2();
         return distance2;
       }

       // implementation of the function to be minimized
       double operator() (const double *lineParams) {
          assert(lineGraph != 0);
          Double_t * xPoints = lineGraph->GetX();
          Double_t * yPoints = lineGraph->GetY();
          Double_t * zPoints = lineGraph->GetZ();
          Int_t numberOfPoints = lineGraph->GetN();
          double sum = 0;
          for (int iPoint=0; iPoint < numberOfPoints; iPoint++) {
            Double_t distance = LinePointDistance2(yPoints[iPoint],zPoints[iPoint],xPoints[iPoint],lineParams);
            sum += distance;
          }
          return sum;
       }

    };


    void FitTrack();
    ClassDef(TelescopeTrack,1);
};

#endif
