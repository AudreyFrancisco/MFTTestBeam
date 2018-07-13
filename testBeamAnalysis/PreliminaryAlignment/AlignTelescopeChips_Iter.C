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


void AlignTelescopeChips_Iter(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{

  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t numberOfEntries = testBeamEventTree_WithClusters->GetEntries();


Double_t arrayMisAlign[cNumberOfChipsInTelescope] = {0,0,0};


  for(Int_t iTrial=0;iTrial<20;iTrial++){
    Int_t movingChipIndex = 1;
    Int_t firstFixedChipIndex = 0;
    Int_t secondFixedChipIndex = 2;


    Double_t arraySubMisAlign[cNumberOfChipsInTelescope] = {0,0,0};

    for(Int_t iSubTrial=0;iSubTrial<2;iSubTrial++){

      TH1F *histoXResidualInMovingChip = new TH1F("histoXResidualInMovingChip","",1000,-5,5);
      histoXResidualInMovingChip->Sumw2();


      for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
        printf("Generating random events... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");

        testBeamEventTree_WithClusters->GetEntry(iEvent);
        AlpideTelescope *telescope = eventWithClusters->GetTelescope();


        AlpideChip *arrayOfChips[cNumberOfChipsInTelescope];
        Int_t numberOfClusterInChip[cNumberOfChipsInTelescope];
        for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
          arrayOfChips[iChip] = telescope->GetChip(iChip);
          numberOfClusterInChip[iChip] = arrayOfChips[iChip]->GetNumberOfClusters();
        }


        for(Int_t iClusterInFirstFixedChip=0;iClusterInFirstFixedChip<numberOfClusterInChip[firstFixedChipIndex];iClusterInFirstFixedChip++){
          AlpideCluster *clusterInFirstFixedChip = arrayOfChips[firstFixedChipIndex]->GetCluster(iClusterInFirstFixedChip);

          Float_t x1 = clusterInFirstFixedChip->GetCenterOfMassX()-arrayMisAlign[firstFixedChipIndex];
          Float_t y1 = clusterInFirstFixedChip->GetCenterOfMassY();
          Float_t z1 = cZPositionOfChipsInTelescope[firstFixedChipIndex];


          for(Int_t iClusterInSecondFixedChip=0;iClusterInSecondFixedChip<numberOfClusterInChip[secondFixedChipIndex];iClusterInSecondFixedChip++){
            AlpideCluster *clusterInSecondFixedChip = arrayOfChips[secondFixedChipIndex]->GetCluster(iClusterInSecondFixedChip);

            Float_t x2 = clusterInSecondFixedChip->GetCenterOfMassX()-arrayMisAlign[secondFixedChipIndex];
            Float_t y2 = clusterInSecondFixedChip->GetCenterOfMassY();
            Float_t z2 = cZPositionOfChipsInTelescope[secondFixedChipIndex];


            Float_t trackXConstante = (z1*x2 - z2*x1)/(z1-z2);
            Float_t trackYConstante = (z1*y2 - z2*y1)/(z1-z2);
            Float_t trackXSlope = (x1-x2)/(z1-z2);
            Float_t trackYSlope = (y1-y2)/(z1-z2);
            TelescopeTrack *track = new TelescopeTrack(iClusterInSecondFixedChip,trackXConstante,trackXSlope,trackYConstante,trackYSlope);
            // track->AddPoint(0,0,0);

            // TelescopeTrack *track = new TelescopeTrack();


            //Check if the track hit the secondChip in a cluster. If yes fit and update the track and add it to the telescope.
            Float_t trackXCoordinateAtMovingChip, trackYCoordinateAtMovingChip;
            track->GetXYCoordinate(trackXCoordinateAtMovingChip, trackYCoordinateAtMovingChip,cZPositionOfChipsInTelescope[movingChipIndex]);
            Int_t clusterInMovingChipIndex;
            if(arrayOfChips[movingChipIndex]->IsAPointInACluster(trackXCoordinateAtMovingChip,trackYCoordinateAtMovingChip,clusterInMovingChipIndex)){
              AlpideCluster *clusterInMovingChip = arrayOfChips[movingChipIndex]->GetCluster(clusterInMovingChipIndex);

              //Fill the three residual histos
              Double_t xResidualInMovingChip  = clusterInMovingChip->GetCenterOfMassX()-trackXCoordinateAtMovingChip;
              histoXResidualInMovingChip->Fill(xResidualInMovingChip);

              break;
            }

          }

        }

      }


      TCanvas* canResidualX = new TCanvas(Form("canResidualX_%d_%d",iTrial,iSubTrial),"canResidualX",450,750);
      SetCanvasStyle(canResidualX);

      TF1 *fGaussX= new TF1("fGaussX","gaus",-5,5);
      fGaussX->SetParameter(0,100);
      fGaussX->SetParameter(1,0);
      fGaussX->SetParameter(2,3);
      histoXResidualInMovingChip->Fit(fGaussX,"RS","",-5,5);
      histoXResidualInMovingChip->GetXaxis()->SetRangeUser(fGaussX->GetParameter(1)-5*fGaussX->GetParameter(2),fGaussX->GetParameter(1)+5*fGaussX->GetParameter(2));

      TLegend* legResidualX = new TLegend(0.14, 0.81, 0.85, 0.92);
      legResidualX->SetFillStyle(0);
      legResidualX->SetLineColorAlpha(0,0);
      legResidualX->SetTextColor(kBlack);
      legResidualX->SetMargin(0.1);
      legResidualX->AddEntry("NULL",Form("Moving chip: %d",movingChipIndex),"");
      legResidualX->AddEntry("NULL",Form("%2.2f, %2.2f",arrayMisAlign[1],arrayMisAlign[2]),"");
      legResidualX->AddEntry("NULL",Form("mean = %3.3f",fGaussX->GetParameter(1)),"");
      legResidualX->Draw();

      // canResidualX->SaveAs(Form("AlignTests/canResidualX_%d.pdf",iTrial));
      arraySubMisAlign[movingChipIndex] = fGaussX->GetParameter(1);

      if(movingChipIndex == 1){
        movingChipIndex = 2;
        secondFixedChipIndex = 1;
      }
      else{
        movingChipIndex = 1;
        secondFixedChipIndex = 2;
      }


    }
    arrayMisAlign[0] += arraySubMisAlign[0];
    arrayMisAlign[1] += arraySubMisAlign[1];
    arrayMisAlign[2] += arraySubMisAlign[2];
  }

}
