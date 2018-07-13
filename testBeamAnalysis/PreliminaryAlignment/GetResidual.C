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

void GetResidual(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{

  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t numberOfEntries = testBeamEventTree_WithClusters->GetEntries();

  TH1F *histoXResidual[cNumberOfChipsInTelescope];
  TH1F *histoYResidual[cNumberOfChipsInTelescope];
  for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    histoXResidual[iChip] = new TH1F(Form("histoXResidual_Chip%d",iChip),"",1000,-5,5);
    histoXResidual[iChip]->Sumw2();

    histoYResidual[iChip] = new TH1F(Form("histoYResidual_Chip%d",iChip),"",1000,-5,5);
    histoYResidual[iChip]->Sumw2();
  }

  Double_t misAlign2 = 0;
  Double_t misAlign3 = 0;

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

    for(Int_t iClusterInFirstChip=0;iClusterInFirstChip<numberOfClustersInFirstChip;iClusterInFirstChip++){
      AlpideCluster *clusterInFirstChip = firstChip->GetCluster(iClusterInFirstChip);

      Float_t x1 = clusterInFirstChip->GetCenterOfMassX();
      Float_t y1 = clusterInFirstChip->GetCenterOfMassY();
      Float_t z1 = cZPositionOfChipsInTelescope[0];


      for(Int_t iClusterInLastChip=0;iClusterInLastChip<numberOfClustersInLastChip;iClusterInLastChip++){
        AlpideCluster *clusterInLastChip = lastChip->GetCluster(iClusterInLastChip);

        Float_t x2 = clusterInLastChip->GetCenterOfMassX()-misAlign3;
        Float_t y2 = clusterInLastChip->GetCenterOfMassY();
        Float_t z2 = cZPositionOfChipsInTelescope[cNumberOfChipsInTelescope-1];


        Float_t trackXConstante = (z1*x2 - z2*x1)/(z1-z2);
        Float_t trackYConstante = (z1*y2 - z2*y1)/(z1-z2);
        Float_t trackXSlope = (x1-x2)/(z1-z2);
        Float_t trackYSlope = (y1-y2)/(z1-z2);
        TelescopeTrack *track = new TelescopeTrack(iClusterInLastChip,trackXConstante,trackXSlope,trackYConstante,trackYSlope);
        // track->AddPoint(0,0,0);

        // TelescopeTrack *track = new TelescopeTrack();


        //Check if the track hit the secondChip in a cluster. If yes fit and update the track and add it to the telescope.
        Float_t trackXCoordinateAtSecondChip, trackYCoordinateAtSecondChip;
        track->GetXYCoordinate(trackXCoordinateAtSecondChip, trackYCoordinateAtSecondChip,cZPositionOfChipsInTelescope[1]);
        Int_t clusterInSecondChipIndex;
        if(secondChip->IsAPointInACluster(trackXCoordinateAtSecondChip,trackYCoordinateAtSecondChip,clusterInSecondChipIndex)){
          AlpideCluster *clusterInSecondChip = secondChip->GetCluster(clusterInSecondChipIndex);

          track->AddPoint(x1,y1,z1);
          track->AddPoint(clusterInSecondChip->GetCenterOfMassX()-misAlign2,clusterInSecondChip->GetCenterOfMassY(),cZPositionOfChipsInTelescope[1]);
          track->AddPoint(x2,y2,z2);
          track->FitTrack();
          //Fill the residual plots:
          for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){

            Float_t trackXCoordinateAtChip, trackYCoordinateAtChip;
            track->GetXYCoordinate(trackXCoordinateAtChip, trackYCoordinateAtChip,cZPositionOfChipsInTelescope[iChip]);

            Double_t xResidual = track->GetVectorPointsX()[iChip] - trackXCoordinateAtChip;
            histoXResidual[iChip]->Fill(xResidual);

            Double_t yResidual = track->GetVectorPointsY()[iChip] - trackYCoordinateAtChip;
            histoYResidual[iChip]->Fill(yResidual);
          }

          break;
        }

      }

    }

  }

  //
  TCanvas* canResidualX = new TCanvas("canResidualX","canResidualX",450,750);
  canResidualX->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canResidualX);
  canResidualX->SetFillColor(kYellow);
  //
  // TCanvas* canResidualY = new TCanvas("canResidualY","canResidualY",450,750);
  // canResidualY->Divide(1,cNumberOfChipsInTelescope);
  // SetCanvasStyle(canResidualY);
  // canResidualY->SetFillColor(kRed);

  Double_t maxXMisAlign = -99;
  for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    canResidualX->cd(iChip+1);
    TF1 *fGaussX= new TF1("fGaussX","gaus",-5,5);
    fGaussX->SetParameter(0,100);
    fGaussX->SetParameter(1,0);
    fGaussX->SetParameter(2,3);
    histoXResidual[iChip]->Fit(fGaussX,"RS","",-5,5);
    histoXResidual[iChip]->GetXaxis()->SetRangeUser(fGaussX->GetParameter(1)-5*fGaussX->GetParameter(2),fGaussX->GetParameter(1)+5*fGaussX->GetParameter(2));
    if(TMath::Abs(fGaussX->GetParameter(1)) > maxXMisAlign) maxXMisAlign = fGaussX->GetParameter(1);

    TLegend* legResidualX = new TLegend(0.11, 0.77, 0.83, 0.88);
    legResidualX->SetFillStyle(0);
    legResidualX->SetLineColorAlpha(0,0);
    legResidualX->SetTextColor(kBlack);
    legResidualX->SetMargin(0.1);
    legResidualX->AddEntry("NULL",Form("mean = %3.3f",fGaussX->GetParameter(1)),"");
    legResidualX->Draw();

    // canResidualY->cd(iChip+1);
    // TF1 *fGaussY= new TF1("fGaussY","gaus",-5,5);
    // fGaussY->SetParameter(0,100);
    // fGaussY->SetParameter(1,0);
    // fGaussY->SetParameter(2,3);
    // histoYResidual[iChip]->Fit(fGaussY,"RS","",-5,5);
  }


}
