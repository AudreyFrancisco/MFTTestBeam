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
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "TelescopeTrack.hpp"
//needed macros
#include "Common.C"

using namespace ROOT::Math;


void AlignTelescopeChips(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{

  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t numberOfEntries = testBeamEventTree_WithClusters->GetEntries();


  //Second step after getting an aproximate of the misalignement (the max residual shift). Iterate over the position shift for the second and third chip. The ones getting the smallest absolute residual shift is the true misalignment.

  Double_t maxXMisAlign = 3;
  Double_t trialStep = 0.2;
  Int_t numberOfTrials = ((2*maxXMisAlign)/trialStep)*((2*maxXMisAlign)/trialStep);//The factor 2 is for the negative and positive misalignments

  ofstream logFile("AlignLog.txt");
  //////////////////////////////////////////////////////////////////////////////////

  TH1F* histoGlobalXResidual = new TH1F("histoGlobalXResidual","",numberOfTrials,0,numberOfTrials);
  Int_t trialCounter =1;

  for(Double_t xMisalignChip2 = -1*maxXMisAlign; xMisalignChip2<maxXMisAlign; xMisalignChip2=xMisalignChip2+trialStep){
    for(Double_t xMisalignChip3 = -1*maxXMisAlign; xMisalignChip3<maxXMisAlign; xMisalignChip3=xMisalignChip3+trialStep){


      TH1F *histoTempoXResidual[cNumberOfChipsInTelescope];
      for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
        histoTempoXResidual[iChip] = new TH1F(Form("histoTempoXResidual_Chip%d_%d",iChip,trialCounter),"",1000,-5,5);
        histoTempoXResidual[iChip]->Sumw2();
      }



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

            Float_t x2 = clusterInLastChip->GetCenterOfMassX()-xMisalignChip3;
            Float_t y2 = clusterInLastChip->GetCenterOfMassY();
            Float_t z2 = cZPositionOfChipsInTelescope[cNumberOfChipsInTelescope-1];


            Float_t trackXConstante = (z1*x2 - z2*x1)/(z1-z2);
            Float_t trackYConstante = (z1*y2 - z2*y1)/(z1-z2);
            Float_t trackXSlope = (x1-x2)/(z1-z2);
            Float_t trackYSlope = (y1-y2)/(z1-z2);
            TelescopeTrack *track = new TelescopeTrack(iClusterInLastChip,trackXConstante,trackXSlope,trackYConstante,trackYSlope);



            //Check if the track hit the secondChip in a cluster. If yes fit and update the track and add it to the telescope.
            Float_t trackXCoordinateAtSecondChip, trackYCoordinateAtSecondChip;
            track->GetXYCoordinate(trackXCoordinateAtSecondChip, trackYCoordinateAtSecondChip,cZPositionOfChipsInTelescope[1]);
            Int_t clusterInSecondChipIndex;
            if(secondChip->IsAPointInACluster(trackXCoordinateAtSecondChip,trackYCoordinateAtSecondChip,clusterInSecondChipIndex)){
              AlpideCluster *clusterInSecondChip = secondChip->GetCluster(clusterInSecondChipIndex);

              track->AddPoint(x1,y1,z1);
              track->AddPoint(clusterInSecondChip->GetCenterOfMassX()-xMisalignChip2,clusterInSecondChip->GetCenterOfMassY(),cZPositionOfChipsInTelescope[1]);
              track->AddPoint(x2,y2,z2);
              track->FitTrack();
              //Fill the residual plots:
              for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){

                Float_t trackXCoordinateAtChip, trackYCoordinateAtChip;
                track->GetXYCoordinate(trackXCoordinateAtChip, trackYCoordinateAtChip,cZPositionOfChipsInTelescope[iChip]);

                Double_t xResidual = track->GetVectorPointsX()[iChip] - trackXCoordinateAtChip;
                histoTempoXResidual[iChip]->Fill(xResidual);

                // Double_t yResidual = track->GetVectorPointsY()[iChip] - trackYCoordinateAtChip;
                // histoYResidual[iChip]->Fill(yResidual);
              }

              break;
            }

          }

        }

      }
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      Double_t globalResidual =0;
      for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
        if(histoTempoXResidual[iChip]->GetEntries()==0){
          globalResidual = 9;
          break;
        }
        TF1 *fGauss= new TF1("fGauss","gaus",-5,5);
        fGauss->SetParameter(0,1000);
        fGauss->SetParameter(1,0);
        fGauss->SetParameter(2,3);
        histoTempoXResidual[iChip]->Fit(fGauss,"RS0","",-2,2);
        globalResidual += TMath::Abs(fGauss->GetParameter(1));

      }
      histoGlobalXResidual->SetBinContent(trialCounter,globalResidual);
      histoGlobalXResidual->GetXaxis()->SetBinLabel(trialCounter,Form("%2.2f, %2.2f",xMisalignChip2,xMisalignChip3));
      trialCounter++;
      logFile<<Form("Trial %d: global misAlign = %2.2f. xMisalignChip2 = %3.3f, xMisalignChip3 = %3.3f",trialCounter,globalResidual, xMisalignChip2, xMisalignChip3)<<endl;
    }
  }

  histoGlobalXResidual->Draw();
}
