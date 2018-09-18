/*
This macro takes a tree that contains information on the evnts, chips, hits, clusters, and tracks from telesopes to calculate the efficiency of each chip. (The macro need to be running after running, RawFileToTree.C, ClustersRec.C, AddTelescopeTracks.C). The efficiency if a chip is defined as a the number of the tracks that have clusters around their intersection points with the chip and the number of tracks in the acceptance of the chip. The function draws one canvas with the efficiency per chip.
*/
//needed root classes
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TString.h"
#include "TMath.h"
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "AlpideChip.hpp"
#include "AlpidePixel.hpp"
#include "AlpideCluster.hpp"

Int_t MaxNumberOfTracks = 1;
void GetResidualInLadders(TString inputFileName = "TestBeamEventTree_WithTelescopeTracks.root", TString outputDirectory = "TestRun")
{
  //The output file of AddTelescopeTracks.C
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_TracksAndClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* event = new TestBeamEvent();
  testBeamEventTree_TracksAndClusters->SetBranchAddress("event", &event);
  Long_t numberOfEntries = testBeamEventTree_TracksAndClusters->GetEntries();


  TH1F ***histoXResidualInLadderChip = new TH1F**[3];
  TH1F ***histoYResidualInLadderChip = new TH1F**[3];
  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    histoXResidualInLadderChip[iLadder] = new TH1F*[3];
    histoYResidualInLadderChip[iLadder] = new TH1F*[3];

    for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
      histoXResidualInLadderChip[iLadder][iChip] = new TH1F(Form("histoXResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",500,-0.5,0.5);
      histoXResidualInLadderChip[iLadder][iChip]->Sumw2();
      histoXResidualInLadderChip[iLadder][iChip]->SetTitle(Form("histoXResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip));

      histoYResidualInLadderChip[iLadder][iChip] = new TH1F(Form("histoYResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",500,-0.5,0.5);
      histoYResidualInLadderChip[iLadder][iChip]->Sumw2();
      histoYResidualInLadderChip[iLadder][iChip]->SetTitle(Form("histoYResidualInLadder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip));
    }

  }



  Double_t xResidualInLadderChip,yResidualInLadderChip;
  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
    printf("Filling Effi histos %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");
    testBeamEventTree_TracksAndClusters->GetEntry(iEvent);
    //loop over all the tracks, for each track, if the track extrapolation i supposed to hit a chip; check if the chip has a cluster around the track-chip intersection point. If the track is not supposed to meet the chip continue and dont store the track.
    AlpideTelescope *telescope = event->GetTelescope();
    Int_t numberOfTelescopeTracks = telescope->GetNumberOfTelescopeTracks();
    // if(numberOfTelescopeTracks > MaxNumberOfTracks) continue;

    for(int iTrack=0;iTrack<numberOfTelescopeTracks;iTrack++){
      TelescopeTrack *track = telescope->GetTelescopeTrack(iTrack);
      // cout<<track->GetNumberOfPoints()<<endl;
      if(track->GetNumberOfPoints() < 4) continue;

      for(Int_t iLadder =0;iLadder<cNumberOfLadders;iLadder++){
        for(Int_t iChip =0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
          AlpideLadder *ladder = event->GetLadder(iLadder);
          AlpideChip *chip = ladder->GetChip(iChip);
          chip->SetInitResoX(1);
          chip->SetInitResoY(1);
          Int_t numberOfClustersInChip = chip->GetNumberOfClusters();
          if(numberOfClustersInChip < 1) continue;

          Float_t trackXCoordinateAtLadder, trackYCoordinateAtLadder;
          track->GetXYCoordinate(trackXCoordinateAtLadder,trackYCoordinateAtLadder,cZPositionOfFirstChipInLadder[iLadder]);
          if( (trackXCoordinateAtLadder > (chip->GetPositionX()+cChipXSize)) || (trackXCoordinateAtLadder < (chip->GetPositionX()-cChipXSize)) ) continue;
          if( (trackYCoordinateAtLadder > (chip->GetPositionY()+cChipYSize)) || (trackYCoordinateAtLadder < (chip->GetPositionY()-cChipYSize)) ) continue;

          Int_t clusterIntex;
          if(chip->IsAPointInACluster(trackXCoordinateAtLadder,trackYCoordinateAtLadder,clusterIntex)){
            AlpideCluster *clusterInLadder = chip->GetCluster(clusterIntex);

            xResidualInLadderChip = clusterInLadder->GetCenterOfMassX() - trackXCoordinateAtLadder;
            histoXResidualInLadderChip[iLadder][iChip]->Fill(xResidualInLadderChip);

            yResidualInLadderChip = clusterInLadder->GetCenterOfMassY() - trackYCoordinateAtLadder;
            histoYResidualInLadderChip[iLadder][iChip]->Fill(yResidualInLadderChip);

          }

        }
      }

    }////////////////////////////////////////////////////////////////////////


  }

  for(Int_t iLadder =0;iLadder<cNumberOfLadders;iLadder++){
    for(Int_t iChip =0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){

      if(histoXResidualInLadderChip[iLadder][iChip]->GetEntries() < 100) continue;

      TCanvas* canResidualInLadder = new TCanvas(Form("canResidualInLadder%d_chip%d",cLadderID[iLadder],iChip),"",1100,600);
      SetCanvasStyle(canResidualInLadder);
      canResidualInLadder->Divide(2,1);
      canResidualInLadder->cd(1);
      histoXResidualInLadderChip[iLadder][iChip]->SetLineColor(kBlack);
      histoXResidualInLadderChip[iLadder][iChip]->Draw();
      TF1 *fGaussXResidualInLadderChip= new TF1(Form("fGaussXResidualInLadder%d_chip%d",cLadderID[iLadder],iChip),"gaus",-5,5);
      fGaussXResidualInLadderChip->SetParameter(0,100);
      fGaussXResidualInLadderChip->SetParameter(1,0);
      fGaussXResidualInLadderChip->SetParameter(2,3);
      histoXResidualInLadderChip[iLadder][iChip]->Fit(fGaussXResidualInLadderChip,"RS","",-5,5);
      histoXResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetRangeUser(-1,1);
      histoXResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetTitle("#delta_{x} (mm)");
      histoXResidualInLadderChip[iLadder][iChip]->GetYaxis()->SetTitle("Counts");

      TLegend* legXResidualInLadderChip = new TLegend(0.10, 0.68, 0.47, 0.87);
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
      fGaussYResidualInLadderChip->SetParameter(0,100);
      fGaussYResidualInLadderChip->SetParameter(1,0);
      fGaussYResidualInLadderChip->SetParameter(2,3);
      histoYResidualInLadderChip[iLadder][iChip]->Fit(fGaussYResidualInLadderChip,"RS","",-5,5);
      histoYResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetRangeUser(-1,1);
      histoYResidualInLadderChip[iLadder][iChip]->GetXaxis()->SetTitle("#delta_{y} (mm)");
      histoYResidualInLadderChip[iLadder][iChip]->GetYaxis()->SetTitle("Counts");

      TLegend* legYResidualInLadderChip = new TLegend(0.10, 0.68, 0.47, 0.87);
      legYResidualInLadderChip->SetFillStyle(0);
      legYResidualInLadderChip->SetLineColorAlpha(0,0);
      legYResidualInLadderChip->SetTextColor(kBlack);
      legYResidualInLadderChip->SetMargin(0.1);
      legYResidualInLadderChip->AddEntry("NULL",Form("Run %s",outputDirectory.Data()),"");
      legYResidualInLadderChip->AddEntry("NULL",Form("Ladder %d, Chip %d",cLadderID[iLadder],cTopChipId-iChip),"");
      legYResidualInLadderChip->AddEntry("NULL",Form("mean = %2.2f #pm %2.2f #mum",1000*fGaussYResidualInLadderChip->GetParameter(1),1000*fGaussYResidualInLadderChip->GetParError(1)),"");
      legYResidualInLadderChip->AddEntry("NULL",Form("#sigma = %2.2f #pm %2.2f #mum",1000*fGaussYResidualInLadderChip->GetParameter(2),1000*fGaussYResidualInLadderChip->GetParError(2)),"");
      legYResidualInLadderChip->Draw();

      canResidualInLadder->SaveAs(Form("%s/canResidualAfterAlignmentInLadder%d_chip%d.pdf",outputDirectory.Data(),cLadderID[iLadder],iChip));

    }
  }

}
