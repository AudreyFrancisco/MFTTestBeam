/*
In this macro, a loop over the clusters in the 3 telescope chips is done and tracks are associated with the clusters. The fitting procedure is implemented in the TelescopeTrack class. It simply fits the data with a straight line. Since only tree chips are used so far in the telescope, a tracks must be detected in the three chips in order to be accepted.
*/
//needed root classes
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TMath.h"
#include <TGraph2D.h>
#include <TVirtualFitter.h>
#include <Math/Vector3D.h>
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "TelescopeTrack.hpp"

using namespace ROOT::Math;
Int_t iFirstFixedChip = 0;
Int_t iSecondFixedChip = 2;
Int_t iFreeChip=1;
void FitTelescopeTracks(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputFileName = "TestBeamEventTree_WithTelescopeTracks.root", TString outputDirectory = "TestRun")
{

  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t numberOfEntries = testBeamEventTree_WithClusters->GetEntries();


  //Create a new file to save the new tree which is a copy of the one with clusters. For each event, the tracks will be copied one by one from the second tree. Is there easier way ?
  TFile *outputFile = new TFile(Form("%s/%s",outputDirectory.Data(),outputFileName.Data()),"recreate");
  TTree *testBeamEventTree_TracksAndClusters = testBeamEventTree_WithClusters->CloneTree(0);


  for(int iEvent=0;iEvent<0.5*numberOfEntries;iEvent++){
    printf("Fititng Tracks  ... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");
    Int_t numberOfTracksPerEvent = 0;
    testBeamEventTree_WithClusters->GetEntry(iEvent);
    AlpideTelescope *telescope = eventWithClusters->GetTelescope();
    //Make a loop over the clusters of the last and first  chip in the telescope, for each cluster associate a preliminary track using the center of mass of the cluster and the origin, then loop over the remaining chips of the telescope and search for clusters in the vicinty of the track, if you find on, add it to the track and refit its points.


    AlpideChip *firstFixedChip = telescope->GetChip(iFirstFixedChip);
    Int_t numberOfClustersInFirstFixedChip = firstFixedChip->GetNumberOfClusters();

    AlpideChip *secondFixedChip = telescope->GetChip(iSecondFixedChip);
    Int_t numberOfClustersInSecondFixedChip = secondFixedChip->GetNumberOfClusters();

    AlpideChip *freeChip = telescope->GetChip(iFreeChip);
    Int_t numberOfClustersInFreeChip = freeChip->GetNumberOfClusters();
    freeChip->SetInitResoX(1);
    freeChip->SetInitResoY(1);


    for(Int_t iClusterInFirstFixedChip=0;iClusterInFirstFixedChip<numberOfClustersInFirstFixedChip;iClusterInFirstFixedChip++){
      AlpideCluster *clusterInFirstFixedChip = firstFixedChip->GetCluster(iClusterInFirstFixedChip);

      Float_t x1 = clusterInFirstFixedChip->GetCenterOfMassX();
      Float_t y1 = clusterInFirstFixedChip->GetCenterOfMassY();
      Float_t z1 = cZPositionOfChipsInTelescope[iFirstFixedChip];


      for(Int_t iClusterInSecondChipChip=0;iClusterInSecondChipChip<numberOfClustersInSecondFixedChip;iClusterInSecondChipChip++){
        AlpideCluster *clusterInSecondFixedChip = secondFixedChip->GetCluster(iClusterInSecondChipChip);

        Float_t x2 = clusterInSecondFixedChip->GetCenterOfMassX();
        Float_t y2 = clusterInSecondFixedChip->GetCenterOfMassY();
        Float_t z2 = cZPositionOfChipsInTelescope[iSecondFixedChip];


        Float_t trackXConstante = (z1*x2 - z2*x1)/(z1-z2);
        Float_t trackYConstante = (z1*y2 - z2*y1)/(z1-z2);
        Float_t trackXSlope = (x1-x2)/(z1-z2);
        Float_t trackYSlope = (y1-y2)/(z1-z2);
        TelescopeTrack *track = new TelescopeTrack(iClusterInSecondChipChip,trackXConstante,trackXSlope,trackYConstante,trackYSlope);
        track->AddPoint(x1,y1,z1);
        track->AddPoint(x2,y2,z2);

        //Check if the track hit the secondChip in a cluster. If yes fit and update the track and add it to the telescope.
        Float_t trackXCoordinateAtFreeChip, trackYCoordinateAtFreeChip;
        track->GetXYCoordinate(trackXCoordinateAtFreeChip, trackYCoordinateAtFreeChip,cZPositionOfChipsInTelescope[iFreeChip]);
        Int_t clusterInFreeChipIndex;
        if(freeChip->IsAPointInACluster(trackXCoordinateAtFreeChip,trackYCoordinateAtFreeChip,clusterInFreeChipIndex)){
          AlpideCluster *clusterInFreeChip = freeChip->GetCluster(clusterInFreeChipIndex);
          track->AddPoint(clusterInFreeChip->GetCenterOfMassX(),clusterInFreeChip->GetCenterOfMassY(),cZPositionOfChipsInTelescope[iFreeChip]);
          track->FitTrack();
          // track->FitTrack();
          telescope->AddTelescopeTrack(track);
          numberOfTracksPerEvent++;
          // break;
        }

      }

    }

    testBeamEventTree_TracksAndClusters->Fill();
  }


  outputFile->Write();
  delete outputFile;

}
