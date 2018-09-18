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
#include "TH2.h"
#include "TF1.h"
#include "TLegend.h"
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "TelescopeTrack.hpp"
//needed macros
#include "Common.C"

using namespace ROOT::Math;

void SetHistogramBinContent(TH2F *histoMisAlignment, TString strBin,Int_t ladderNumber, Double_t xMisAlign, Double_t yMisAlign){
  int lastXbin = histoMisAlignment->FindLastBinAbove(0,1);
  int xBinInHistoMisAlignment = histoMisAlignment->GetXaxis()->FindBin(strBin);
  if(!xBinInHistoMisAlignment)
  {
    histoMisAlignment->GetXaxis()->SetBinLabel(lastXbin+1,strBin);
    xBinInHistoMisAlignment = lastXbin+1;
  }

  histoMisAlignment->SetBinContent(xBinInHistoMisAlignment,2*ladderNumber+1,xMisAlign);
  histoMisAlignment->SetBinContent(xBinInHistoMisAlignment,2*ladderNumber+2,yMisAlign);

}

void FinalAlignment(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{


  //Original tree that contains clusterized events (output of ClustersRec.C)
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_WithClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* eventWithClusters = new TestBeamEvent();
  testBeamEventTree_WithClusters->SetBranchAddress("event", &eventWithClusters);
  Long_t totalNumberOfEntries = testBeamEventTree_WithClusters->GetEntries();
  Long_t numberOfEntries = 1*totalNumberOfEntries;



  TH1F ***histoXResidualInLadderChip = new TH1F**[3];
  TH1F ***histoYResidualInLadderChip = new TH1F**[3];
  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    histoXResidualInLadderChip[iLadder] = new TH1F*[3];
    histoYResidualInLadderChip[iLadder] = new TH1F*[3];

    for(Int_t iChip=0;iChip<3;iChip++){
      histoXResidualInLadderChip[iLadder][iChip] = new TH1F(Form("histoXResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",500,-5,5);
      histoXResidualInLadderChip[iLadder][iChip]->Sumw2();
      histoXResidualInLadderChip[iLadder][iChip]->SetTitle(Form("histoXResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip));

      histoYResidualInLadderChip[iLadder][iChip] = new TH1F(Form("histoYResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",500,-5,5);
      histoYResidualInLadderChip[iLadder][iChip]->Sumw2();
      histoYResidualInLadderChip[iLadder][iChip]->SetTitle(Form("histoYResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip));
    }

  }

  Double_t xResidualInLadderChip,yResidualInLadderChip;

  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
    printf("Alignment step 3  ... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");
    // cout<<"----------------------------------------------------------------"<<endl;
    Int_t numberOfTracksPerEvent = 0;
    testBeamEventTree_WithClusters->GetEntry(iEvent);
    AlpideTelescope *telescope = eventWithClusters->GetTelescope();
    //Make a loop over the clusters of the last and first  chip in the telescope, for each cluster associate a preliminary track using the center of mass of the cluster and the origin, then loop over the remaining chips of the telescope and search for clusters in the vicinty of the track, if you find on, add it to the track and refit its points.


    AlpideChip *firstChip = telescope->GetChip(0);
    firstChip->SetInitResoX(1);
    firstChip->SetInitResoY(1);
    Int_t numberOfClustersInFirstChip = firstChip->GetNumberOfClusters();
    if(numberOfClustersInFirstChip != 1) continue;

    AlpideChip *secondChip = telescope->GetChip(1);
    Int_t numberOfClustersInSecondChip = secondChip->GetNumberOfClusters();
    if(numberOfClustersInSecondChip != 1) continue;

    AlpideChip *lastChip = telescope->GetChip(2);
    Int_t numberOfClustersInLastChip = lastChip->GetNumberOfClusters();
    if(numberOfClustersInLastChip != 1) continue;


    AlpideCluster *clusterInFirstChip = firstChip->GetCluster(0);
    Float_t x1 = clusterInFirstChip->GetCenterOfMassX();
    Float_t y1 = clusterInFirstChip->GetCenterOfMassY();
    Float_t z1 = cZPositionOfChipsInTelescope[0];

    AlpideCluster *clusterInLastChip = lastChip->GetCluster(0);
    Float_t x2 = clusterInLastChip->GetCenterOfMassX();
    Float_t y2 = clusterInLastChip->GetCenterOfMassY();
    Float_t z2 = cZPositionOfChipsInTelescope[2];

    Float_t trackXConstante = (z1*x2 - z2*x1)/(z1-z2);
    Float_t trackYConstante = (z1*y2 - z2*y1)/(z1-z2);
    Float_t trackXSlope = (x1-x2)/(z1-z2);
    Float_t trackYSlope = (y1-y2)/(z1-z2);
    TelescopeTrack *track = new TelescopeTrack(0,trackXConstante,trackXSlope,trackYConstante,trackYSlope);
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


      for(Int_t iLadder =0;iLadder<cNumberOfLadders;iLadder++){
        for(Int_t iChip =0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
          AlpideLadder *ladder = eventWithClusters->GetLadder(iLadder);
          AlpideChip *chip = ladder->GetChip(iChip);
          chip->SetInitResoX(3);
          chip->SetInitResoY(3);
          Int_t numberOfClustersInChip = chip->GetNumberOfClusters();
          if(numberOfClustersInChip < 1) continue;

          Float_t trackXCoordinateAtLadder, trackYCoordinateAtLadder;
          track->GetXYCoordinate(trackXCoordinateAtLadder,trackYCoordinateAtLadder,cZPositionOfFirstChipInLadder[iLadder]);

          // if( (trackXCoordinateAtLadder > 10) || (trackXCoordinateAtLadder < 8) ) continue;
          // if( (trackYCoordinateAtLadder > 6) || (trackYCoordinateAtLadder < 4) ) continue;

          if( (trackXCoordinateAtLadder > (chip->GetPositionX()+cChipXSize)) || (trackXCoordinateAtLadder < (chip->GetPositionX()-cChipXSize)) ) continue;
          if( (trackYCoordinateAtLadder > (chip->GetPositionY()+cChipYSize)) || (trackYCoordinateAtLadder < (chip->GetPositionY()-cChipYSize)) ) continue;

          Int_t clusterIntex;
          if(chip->IsAPointInACluster(trackXCoordinateAtLadder,trackYCoordinateAtLadder,clusterIntex)){
            AlpideCluster *clusterInLadder = chip->GetCluster(clusterIntex);

            xResidualInLadderChip = clusterInLadder->GetCenterOfMassX() - trackXCoordinateAtLadder+chip->GetPixelSizeX();
            histoXResidualInLadderChip[iLadder][iChip]->Fill(xResidualInLadderChip);

            yResidualInLadderChip = clusterInLadder->GetCenterOfMassY() - trackYCoordinateAtLadder+chip->GetPixelSizeY();
            histoYResidualInLadderChip[iLadder][iChip]->Fill(yResidualInLadderChip);

          }

        }
      }

    }


  }

  //Get the alignment folder (there are two disk positions) corresponding to this run:
  //Create histogram containing the misalignment values for all the chips.
  TFile *outputFile = new TFile("LaddersAlignment.root","update");
  TH2F *histoMisAlignment;
  histoMisAlignment = ((TH2F*) outputFile->Get("histoMisAlignment"));

  if(!histoMisAlignment){
    outputFile = new TFile("LaddersAlignment.root","RECREATE");
    histoMisAlignment = new TH2F("histoMisAlignment","",100,0,100,9,0,9);
  }



  for(Int_t iLadder =0;iLadder<1;iLadder++){
    for(Int_t iChip =0;iChip<1;iChip++){

      TCanvas* canResidualInLadder = new TCanvas(Form("canResidualInLadder%d_chip%d",cLadderID[iLadder],iChip),"",1100,600);
      SetCanvasStyle(canResidualInLadder);
      canResidualInLadder->Divide(2,1);
      canResidualInLadder->cd(1);
      histoXResidualInLadderChip[iLadder][iChip]->SetLineColor(kBlack);
      histoXResidualInLadderChip[iLadder][iChip]->Draw();
      TF1 *fGaussXResidualInLadderChip= new TF1(Form("fGaussXResidualInLadder%d_chip%d",cLadderID[iLadder],iChip),"gaus",-5,5);
      fGaussXResidualInLadderChip->SetParameter(0,histoYResidualInLadderChip[iLadder][iChip]->GetMaximum());
      fGaussXResidualInLadderChip->SetParameter(1,histoYResidualInLadderChip[iLadder][iChip]->GetMean());
      fGaussXResidualInLadderChip->SetParameter(2,3);
      histoXResidualInLadderChip[iLadder][iChip]->Fit(fGaussXResidualInLadderChip,"RS","",-5,5);
      // histoXResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetRangeUser(-1,1);
      histoXResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetTitle("#delta_{x} (mm)");
      histoXResidualInLadderChip[iLadder][iChip]->GetYaxis()->SetTitle("Counts");

      TLegend* legXResidualInLadderChip = new TLegend(0.139, 0.664, 0.86, 0.848);
      legXResidualInLadderChip->SetFillStyle(0);
      legXResidualInLadderChip->SetLineColorAlpha(0,0);
      legXResidualInLadderChip->SetTextColor(kBlack);
      legXResidualInLadderChip->SetMargin(0.1);
      legXResidualInLadderChip->AddEntry("NULL",Form("Run %s",outputDirectory.Data()),"");
      legXResidualInLadderChip->AddEntry("NULL",Form("Ladder %d, Chip %d",cLadderID[iLadder],cTopChipId-iChip),"");
      legXResidualInLadderChip->AddEntry("NULL",Form("mean = %2.2f #pm %2.2f #mum",1000*fGaussXResidualInLadderChip->GetParameter(1),1000*fGaussXResidualInLadderChip->GetParError(1)),"");
      legXResidualInLadderChip->AddEntry("NULL",Form("#sigma = %2.2f #pm %2.2f #mum",1000*fGaussXResidualInLadderChip->GetParameter(2),1000*fGaussXResidualInLadderChip->GetParError(2)),"");
      legXResidualInLadderChip->Draw();


      canResidualInLadder->cd(2);
      histoYResidualInLadderChip[iLadder][iChip]->SetLineColor(kBlack);
      histoYResidualInLadderChip[iLadder][iChip]->Draw();
      TF1 *fGaussYResidualInLadderChip= new TF1(Form("fGaussYResidualInLadder%d_chip%d",cLadderID[iLadder],iChip),"gaus",-5,5);
      fGaussYResidualInLadderChip->SetParameter(0,200);
      fGaussYResidualInLadderChip->SetParameter(1,0);
      fGaussYResidualInLadderChip->SetParameter(2,0.1);
      histoYResidualInLadderChip[iLadder][iChip]->Fit(fGaussYResidualInLadderChip,"RS","",-5,5);
      // histoYResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetRangeUser(-1,1);
      histoYResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetTitle("#delta_{y} (mm)");
      histoYResidualInLadderChip[iLadder][iChip]->GetYaxis()->SetTitle("Counts");

      TLegend* legYResidualInLadderChip = new TLegend(0.139, 0.664, 0.86, 0.848);
      legYResidualInLadderChip->SetFillStyle(0);
      legYResidualInLadderChip->SetLineColorAlpha(0,0);
      legYResidualInLadderChip->SetTextColor(kBlack);
      legYResidualInLadderChip->SetMargin(0.1);
      legYResidualInLadderChip->AddEntry("NULL",Form("Run %s",outputDirectory.Data()),"");
      legYResidualInLadderChip->AddEntry("NULL",Form("Ladder %d, Chip %d",cLadderID[iLadder],cTopChipId-iChip),"");
      legYResidualInLadderChip->AddEntry("NULL",Form("mean = %2.2f #pm %2.2f #mum",1000*fGaussYResidualInLadderChip->GetParameter(1),1000*fGaussYResidualInLadderChip->GetParError(1)),"");
      legYResidualInLadderChip->AddEntry("NULL",Form("#sigma = %2.2f #pm %2.2f #mum",1000*fGaussYResidualInLadderChip->GetParameter(2),1000*fGaussYResidualInLadderChip->GetParError(2)),"");
      legYResidualInLadderChip->Draw();

      if(histoXResidualInLadderChip[iLadder][iChip]->GetEntries() > 100){
        canResidualInLadder->SaveAs(Form("%s/canResidualInLadder%d_chip%d.pdf",outputDirectory.Data(),cLadderID[iLadder],iChip));
        //Fill manually the misalignment values. //TODO: Better to be automatic
        SetHistogramBinContent(histoMisAlignment,outputDirectory,iLadder,fGaussXResidualInLadderChip->GetParameter(1),fGaussYResidualInLadderChip->GetParameter(1));
      }


    }
  }


  histoMisAlignment->Write("",TObject::kOverwrite);
  outputFile->Close();
}
