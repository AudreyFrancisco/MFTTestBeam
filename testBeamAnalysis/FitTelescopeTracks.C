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

void FitTelescopeTracks(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputFileName = "TestBeamEventTree_WithTelescopeTracks.root", TString outputDirectory = "TestRun")
{

  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t numberOfEntries = testBeamEventTree_WithClusters->GetEntries();

  //A 1-D histogram to save information about the number of tracks oer event
  TH1I *histoNumberOfTracksPerEvent = new TH1I("histoNumberOfTracksPerEvent","",100,0,100);
  histoNumberOfTracksPerEvent->Sumw2();

  //Create a new file to save the new tree which is a copy of the one with clusters. For each event, the tracks will be copied one by one from the second tree. Is there easier way ?
  TFile *outputFile = new TFile(Form("%s/%s",outputDirectory.Data(),outputFileName.Data()),"recreate");
  TTree *testBeamEventTree_TracksAndClusters = testBeamEventTree_WithClusters->CloneTree(0);




  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
    printf("Fititng Tracks  ... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");
    Int_t numberOfTracksPerEvent = 0;
    testBeamEventTree_WithClusters->GetEntry(iEvent);
    AlpideTelescope *telescope = eventWithClusters->GetTelescope();
    //Make a loop over the clusters of the last and first  chip in the telescope, for each cluster associate a preliminary track using the center of mass of the cluster and the origin, then loop over the remaining chips of the telescope and search for clusters in the vicinty of the track, if you find on, add it to the track and refit its points.


    AlpideChip *firstChip = telescope->GetChip(0);
    Int_t numberOfClustersInFirstChip = firstChip->GetNumberOfClusters();

    AlpideChip *lastChip = telescope->GetChip(cNumberOfChipsInTelescope-1);
    Int_t numberOfClustersInLastChip = lastChip->GetNumberOfClusters();

    AlpideChip *secondChip = telescope->GetChip(1);
    Int_t numberOfClustersInSecondChip = secondChip->GetNumberOfClusters();




    for(Int_t iClusterInFirstChip=0;iClusterInFirstChip<numberOfClustersInFirstChip;iClusterInFirstChip++){
      AlpideCluster *clusterInFirstChip = firstChip->GetCluster(iClusterInFirstChip);

      Float_t x1 = clusterInFirstChip->GetCenterOfMassX();
      Float_t y1 = clusterInFirstChip->GetCenterOfMassY();
      Float_t z1 = cZPositionOfChipsInTelescope[0];


      for(Int_t iClusterInLastChip=0;iClusterInLastChip<numberOfClustersInLastChip;iClusterInLastChip++){
        AlpideCluster *clusterInLastChip = lastChip->GetCluster(iClusterInLastChip);

        Float_t x2 = clusterInLastChip->GetCenterOfMassX();
        Float_t y2 = clusterInLastChip->GetCenterOfMassY();
        Float_t z2 = cZPositionOfChipsInTelescope[cNumberOfChipsInTelescope-1];


        Float_t trackXConstante = (z1*x2 - z2*x1)/(z1-z2);
        Float_t trackYConstante = (z1*y2 - z2*y1)/(z1-z2);
        Float_t trackXSlope = (x1-x2)/(z1-z2);
        Float_t trackYSlope = (y1-y2)/(z1-z2);
        TelescopeTrack *track = new TelescopeTrack(iClusterInLastChip,trackXConstante,trackXSlope,trackYConstante,trackYSlope);
        track->AddPoint(x1,y1,z1);
        track->AddPoint(x2,y2,z2);

        //Check if the track hit the secondChip in a cluster. If yes fit and update the track and add it to the telescope.
        Float_t trackXCoordinateAtSecondChip, trackYCoordinateAtSecondChip;
        track->GetXYCoordinate(trackXCoordinateAtSecondChip, trackYCoordinateAtSecondChip,cZPositionOfChipsInTelescope[1]);
        Int_t clusterInSecondChipIndex;
        if(secondChip->IsAPointInACluster(trackXCoordinateAtSecondChip,trackYCoordinateAtSecondChip,clusterInSecondChipIndex)){
          AlpideCluster *clusterInSecondChip = secondChip->GetCluster(clusterInSecondChipIndex);
          track->AddPoint(clusterInSecondChip->GetCenterOfMassX(),clusterInSecondChip->GetCenterOfMassY(),cZPositionOfChipsInTelescope[1]);
          track->FitTrack();
          telescope->AddTelescopeTrack(track);
          numberOfTracksPerEvent++;
          // break;
        }

      }

    }

    testBeamEventTree_TracksAndClusters->Fill();
    histoNumberOfTracksPerEvent->Fill(numberOfTracksPerEvent);
  }


  outputFile->Write();
  delete outputFile;


  //Add the histogram of number of tracks per event to the AnalysisResults.root file
  TFile *outputHistosFile = new TFile(Form("%s/AnalysisResults.root",outputDirectory.Data()),"UPDATE");
  TDirectory *tracksDirectory = outputHistosFile->mkdir("Tracks");
  tracksDirectory->cd();
  histoNumberOfTracksPerEvent->Write();
  outputHistosFile->Close();



}
