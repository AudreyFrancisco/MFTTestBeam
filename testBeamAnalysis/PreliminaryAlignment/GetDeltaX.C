/*
events and tracks randomly generated using GenerateRandomEvents.C are added to the tree that contains events and clusters (output of ClustersRec.C). This action could not be done directly in GenerateRandomEvents.C due to the way the event are read from the .txt file which is also generated simultaneously by  GenerateRandomEvents.C. This macro might be omitted once the reading format of the telescope tracks is known.
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
#include "TH1.h"
#include "TF1.h"
#include "TLegend.h"
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "TelescopeTrack.hpp"
//needed macros
#include "Common.C"

using namespace ROOT::Math;

void GetDeltaX(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{

  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t numberOfEntries = testBeamEventTree_WithClusters->GetEntries();

  TH1F *histoDeltaX = new TH1F("histoDeltaX","",1000,-5,5);
  histoDeltaX->Sumw2();



  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
    printf("Generating random events... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");

    testBeamEventTree_WithClusters->GetEntry(iEvent);
    AlpideTelescope *telescope = eventWithClusters->GetTelescope();

    //The first and last chips:
    AlpideChip *firstChip = telescope->GetChip(0);
    Int_t numberOfClustersInFirstChip = firstChip->GetNumberOfClusters();

    AlpideChip *lastChip = telescope->GetChip(cNumberOfChipsInTelescope-1);
    Int_t numberOfClustersInLastChip = lastChip->GetNumberOfClusters();

    AlpideChip *secondChip = telescope->GetChip(1);
    Int_t numberOfClustersInSecondChip = secondChip->GetNumberOfClusters();

    cout<<numberOfClustersInSecondChip<<"  "<<numberOfClustersInLastChip<<endl;

    for(Int_t iClusterInSecondChip=0;iClusterInSecondChip<numberOfClustersInSecondChip;iClusterInSecondChip++){
      AlpideCluster *clusterInSecondChip = secondChip->GetCluster(iClusterInSecondChip);
      for(Int_t iClusterInLastChip=0;iClusterInLastChip<numberOfClustersInLastChip;iClusterInLastChip++){
        AlpideCluster *clusterInLastChip = lastChip->GetCluster(iClusterInLastChip);

        histoDeltaX->Fill(clusterInLastChip->GetCenterOfMassX()-clusterInSecondChip->GetCenterOfMassX());

      }
    }

  }



  histoDeltaX->Draw();

}
