/*
The macro responsible for clustering. The macro takes the output of RawTreeToTree.C and make clusters for each chip. It will create a clone of the input tree and add the clusters per event to it. Some assumptions are made here which need to be checked like the way pixels are considered neighbours.
Note that the clustering algorithm is not in this macro, it is already implemented in the AlpideChip class
*/
//needed root classes
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TString.h"
#include "TMath.h"
#include "TDirectory.h"
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "AlpideTelescope.hpp"
#include "AlpideLadder.hpp"
#include "AlpideChip.hpp"
#include "AlpidePixel.hpp"
#include "AlpideCluster.hpp"

//needed macros
#include "Common.C"

//The main function
void MakeHitsAndClusters(TString inputFileName = "TestBeamEventTree.root", TString outputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{

  //The output of RawFileToTree.C. This tree will be cloned and upadted below
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* event = new TestBeamEvent();
  testBeamEventTree->SetBranchAddress("event", &event);
  Long_t numberOfEntries = testBeamEventTree->GetEntries();


  //Create a new file to save the new tree which is a copy of the original but with adding clusters.
  TFile *outputFile = new TFile(Form("%s/%s",outputDirectory.Data(),outputFileName.Data()),"recreate");
  TTree *treeEventsWithClusters = testBeamEventTree->CloneTree(0);
  AlpideLadder *ladder;
  AlpideChip *chip;
  AlpideTelescope *telescope;

  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
    if(iEvent%100 == 0) printf("Making hits and clusters  ... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");
    // if(iEvent > 420000) continue;
    // if(iEvent%10000 == 0) cout<<iEvent<<"  ";

    testBeamEventTree->GetEntry(iEvent);

    //Loop over the chips in the ladders and reconstruct their clusters
    for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
      ladder = event->GetLadder(iLadder);
      Int_t numberOfChipsInLadder = ladder->GetNumberOfChips();
      for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
        chip = ladder->GetChip(iChip);
        chip->ClustersRecInChip();
      }
    }

    //Loop over the chips in the telescop and reconstruct their clusters
    telescope = event->GetTelescope();
    for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
      chip = telescope->GetChip(iChip);
      if(cDoClustering) chip->ClustersRecInChip();
    }

    treeEventsWithClusters->Fill();

  }


  outputFile->Write();
  delete outputFile;

}
