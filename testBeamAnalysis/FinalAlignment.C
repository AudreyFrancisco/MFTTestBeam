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

void FinalAlignment(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{

  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t numberOfEntries = testBeamEventTree_WithClusters->GetEntries();


  //**************************************************************************************************************************************//
  //First step: Alignment of the last two chips benefeting from the fact that they are very close in Z. The difference between the clsters in x, is expected to have a gaussian distrubution centered at the misalignment value (0 if ideal).
  TH1F *histoLastTwoChipsDeltaX = new TH1F("histoLastTwoChipsDeltaX","",1000,-5,5);//in mm. TODO: genralize it.
  histoLastTwoChipsDeltaX->Sumw2();

  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
  
    testBeamEventTree_WithClusters->GetEntry(iEvent);
    AlpideTelescope *telescope = eventWithClusters->GetTelescope();

    //The last two chips:
    AlpideChip *lastChip = telescope->GetChip(cNumberOfChipsInTelescope-1);
    Int_t numberOfClustersInLastChip = lastChip->GetNumberOfClusters();

    AlpideChip *secondChip = telescope->GetChip(cNumberOfChipsInTelescope-2);
    Int_t numberOfClustersInSecondChip = secondChip->GetNumberOfClusters();


    for(Int_t iClusterInSecondChip=0;iClusterInSecondChip<numberOfClustersInSecondChip;iClusterInSecondChip++){
      AlpideCluster *clusterInSecondChip = secondChip->GetCluster(iClusterInSecondChip);
      for(Int_t iClusterInLastChip=0;iClusterInLastChip<numberOfClustersInLastChip;iClusterInLastChip++){
        AlpideCluster *clusterInLastChip = lastChip->GetCluster(iClusterInLastChip);

        histoLastTwoChipsDeltaX->Fill(clusterInSecondChip->GetCenterOfMassX()-clusterInLastChip->GetCenterOfMassX());

      }
    }

  }
  //Draw The results of the first step, fit the gaussian distribution and use it in the next step:
  TCanvas* canLastTwoChipsDeltaX = new TCanvas("canLastTwoChipsDeltaX","canLastTwoChipsDeltaX",450,750);
  SetCanvasStyle(canLastTwoChipsDeltaX);
  histoLastTwoChipsDeltaX->Draw();
  TF1 *fGaussLastTwoChipsDeltaX = new TF1("fGaussLastTwoChipsDeltaX","gaus",-5,5);
  fGaussLastTwoChipsDeltaX->SetParameter(0,100);
  fGaussLastTwoChipsDeltaX->SetParameter(1,0);
  fGaussLastTwoChipsDeltaX->SetParameter(2,3);
  histoLastTwoChipsDeltaX->Fit(fGaussLastTwoChipsDeltaX,"RS","",-5,5);
  Double_t misAlignXChip2 = fGaussLastTwoChipsDeltaX->GetParameter(1);

  TLegend* legLastTwoChipsDeltaX = new TLegend(0.11, 0.77, 0.83, 0.88);
  legLastTwoChipsDeltaX->SetFillStyle(0);
  legLastTwoChipsDeltaX->SetLineColorAlpha(0,0);
  legLastTwoChipsDeltaX->SetTextColor(kBlack);
  legLastTwoChipsDeltaX->SetMargin(0.1);
  legLastTwoChipsDeltaX->AddEntry("NULL",Form("mean = %3.3f",fGaussLastTwoChipsDeltaX->GetParameter(1)),"");
  legLastTwoChipsDeltaX->Draw();

  //****************************************************************************************************************************************//

  //Second Step: Correct the chip 2 using its misalignment from step 1 with respect to the last chip. Fit tracks in the last two chips and get the residual in the first one.
  TH1F *histoXResidualInFirstChip = new TH1F("histoXResidualInFirstChip","",1000,-5,5);
  histoXResidualInFirstChip->Sumw2();

  //First Step is to get an estimate of the misalignement by plotting the residual:
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

    for(Int_t iClusterInSecondChip=0;iClusterInSecondChip<numberOfClustersInSecondChip;iClusterInSecondChip++){
      AlpideCluster *clusterInSecondChip = secondChip->GetCluster(iClusterInSecondChip);

      Float_t x1 = clusterInSecondChip->GetCenterOfMassX()-misAlignXChip2;
      Float_t y1 = clusterInSecondChip->GetCenterOfMassY();
      Float_t z1 = cZPositionOfChipsInTelescope[1];


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


        //Check if the track hit the secondChip in a cluster. If yes fit and update the track and add it to the telescope.
        Float_t trackXCoordinateAtFirstChip, trackYCoordinateAtFirstChip;
        track->GetXYCoordinate(trackXCoordinateAtFirstChip, trackYCoordinateAtFirstChip,cZPositionOfChipsInTelescope[0]);
        Int_t clusterInFirstChipIndex;
        if(firstChip->IsAPointInACluster(trackXCoordinateAtFirstChip, trackYCoordinateAtFirstChip,clusterInFirstChipIndex)){
          AlpideCluster *clusterInFirstChip = firstChip->GetCluster(clusterInFirstChipIndex);

          Double_t xResidualInFirstChip = clusterInFirstChip->GetCenterOfMassX() - trackXCoordinateAtFirstChip;
          histoXResidualInFirstChip->Fill(xResidualInFirstChip);

          break;
        }

      }

    }

  }



  TCanvas* canXResidualInFirstChip = new TCanvas("canXResidualInFirstChip","canXResidualInFirstChip",450,750);
  SetCanvasStyle(canXResidualInFirstChip);

  TF1 *fGaussXResidualInFirstChip= new TF1("fGaussXResidualInFirstChip","gaus",-5,5);
  fGaussXResidualInFirstChip->SetParameter(0,100);
  fGaussXResidualInFirstChip->SetParameter(1,0);
  fGaussXResidualInFirstChip->SetParameter(2,3);
  histoXResidualInFirstChip->Fit(fGaussXResidualInFirstChip,"RS","",-5,5);


  TLegend* legXResidualInFirstChip = new TLegend(0.11, 0.77, 0.83, 0.88);
  legXResidualInFirstChip->SetFillStyle(0);
  legXResidualInFirstChip->SetLineColorAlpha(0,0);
  legXResidualInFirstChip->SetTextColor(kBlack);
  legXResidualInFirstChip->SetMargin(0.1);
  legXResidualInFirstChip->AddEntry("NULL",Form("mean = %3.3f",fGaussXResidualInFirstChip->GetParameter(1)),"");
  legXResidualInFirstChip->Draw();






}
