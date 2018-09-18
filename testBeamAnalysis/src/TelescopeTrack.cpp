//Check The .hpp for the description of the class
#include "TelescopeTrack.hpp"

using namespace std;

ClassImp(TelescopeTrack)

// default constructor
//______________________________________________________________________
TelescopeTrack::TelescopeTrack()
:TObject(),
fTrackID(-1),
fXConstant(0.),
fYConstant(0.),
fXSlope(0.),
fYSlope(0.),
fNumberOfPoints(0),
fPrintResults(kFALSE),
fChiSquare(-1.)
{
  fVectorPointsX.reserve(100);
  fVectorPointsY.reserve(100);
  fVectorPointsZ.reserve(100);
}

//constructor
//______________________________________________________________________
TelescopeTrack::TelescopeTrack(Int_t trackID, Float_t xConstant, Float_t xSlope, Float_t yConstant, Float_t ySlope)
:TObject(),
fTrackID(trackID),
fXConstant(xConstant),
fYConstant(yConstant),
fXSlope(xSlope),
fYSlope(ySlope),
fNumberOfPoints(0),
fPrintResults(kFALSE),
fChiSquare(-1.)
{
  fVectorPointsX.reserve(100);
  fVectorPointsY.reserve(100);
  fVectorPointsZ.reserve(100);
}

// define the parameteric line equation
void TelescopeTrack::GetXYCoordinate(Float_t &xCoordinate, Float_t &yCoordinate, Float_t zCoordiante) {
  // a parameteric line is define from 6 parameters but 4 are independent
  // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
  //If the track passes through the intercation point (0,0,0), fXConstant=fYConstant =0
  xCoordinate = fXConstant + fXSlope*zCoordiante;
  yCoordinate = fYConstant + fYSlope*zCoordiante;
}
void TelescopeTrack::AddPoint(Float_t x, Float_t y, Float_t z){
  fVectorPointsX.push_back(x);
  fVectorPointsY.push_back(y);
  fVectorPointsZ.push_back(z);
  fNumberOfPoints++;
}

void TelescopeTrack::Reset(){
  fVectorPointsX.clear();
  fVectorPointsY.clear();
  fVectorPointsZ.clear();
  fNumberOfPoints=0;
}

void TelescopeTrack::FitTrack(){

  //To fit points, they must be in an object, TH or graph
  TGraph2D * lineGraph = new TGraph2D();
  //Add the points from the track and the new point:
  for(Int_t iPoint=0;iPoint<fNumberOfPoints;iPoint++){
    lineGraph->SetPoint(iPoint,fVectorPointsZ[iPoint],fVectorPointsX[iPoint],fVectorPointsY[iPoint]);
  }

  ROOT::Fit::Fitter  lineFitter;

  // make the functor objet
  SumDistance2Object pointsToLineDistance(lineGraph);
  ROOT::Math::Functor lineFunction(pointsToLineDistance,4);
  // set the function and the initial parameter values
  Double_t pStart[4] = {1,1,1,1};
  lineFitter.SetFCN(lineFunction,pStart);
  // set step sizes different than default ones (0.3 times parameter values)
  for (Int_t iPar = 0; iPar < 4; iPar++){
    lineFitter.Config().ParSettings(iPar).SetStepSize(0.01);
  }

  Bool_t fitIsOK = lineFitter.FitFCN();
  if (!fitIsOK) {
     Error("line3Dfit","Line3D Fit failed");
     return;
  }

  const ROOT::Fit::FitResult & fitResult = lineFitter.Result();
  if(fPrintResults){
    fitResult.Print(std::cout);
  }
  SetParams(fitResult.Parameter(0),fitResult.Parameter(1),fitResult.Parameter(2),fitResult.Parameter(3));
}
