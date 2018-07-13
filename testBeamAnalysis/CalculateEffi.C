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

void CalculateEffi(TString inputFileName = "TestBeamEventTree_WithTelescopeTracks.root", TString outputDirectory = "TestRun")
{
  //The output file of AddTelescopeTracks.C
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_TracksAndClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* event = new TestBeamEvent();
  testBeamEventTree_TracksAndClusters->SetBranchAddress("event", &event);
  Long_t numberOfEntries = testBeamEventTree_TracksAndClusters->GetEntries();


  //define the histograms of the chip hitmaps. TODO: Make them look good
  TH1F **histoRecTracksInChips = new TH1F*[cNumberOfLadders];
  TH1F **histoRecTracksInTelescope = new TH1F*[cNumberOfLadders];

  for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
    histoRecTracksInChips[iLadder] = new TH1F(Form("histoRecTracksInChips_Ladder%d",iLadder),"",numberOfChipsInLadder,0,numberOfChipsInLadder);
    histoRecTracksInChips[iLadder]->Sumw2();

    histoRecTracksInTelescope[iLadder] = new TH1F(Form("histoRecTracksInTelescope_Ladder%d",iLadder),"",numberOfChipsInLadder,0,numberOfChipsInLadder);
    histoRecTracksInTelescope[iLadder]->Sumw2();
  }



  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
    // cout<<iEvent<<endl;
    testBeamEventTree_TracksAndClusters->GetEntry(iEvent);
    //loop over all the tracks, for each track, if the track extrapolation i supposed to hit a chip; check if the chip has a cluster around the track-chip intersection point. If the track is not supposed to meet the chip continue and dont store the track.
    AlpideTelescope *telescope = event->GetTelescope();
    Int_t numberOfTelescopeTracks = telescope->GetNumberOfTelescopeTracks();
    for(int iTrack=0;iTrack<numberOfTelescopeTracks;iTrack++){
      TelescopeTrack *track = telescope->GetTelescopeTrack(iTrack);

      //Loop over the ladders and the chips in the ladder
      for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
        AlpideLadder *ladder = event->GetLadder(iLadder);

        Float_t trackXCoordinateAtLadder, trackYCoordinateAtLadder;
        track->GetXYCoordinate(trackXCoordinateAtLadder,trackYCoordinateAtLadder,cZPositionOfFirstChipInLadder[iLadder]);
        //Get the chip in which the pixel is hit using the trackYCoordinateAtLadder:
        Int_t whichChip = (trackXCoordinateAtLadder + 0.5*cNumberOfChipsInLadder[iLadder]*cChipXSize)/cChipXSize;
        AlpideChip *chip = ladder->GetChip(whichChip);

        //get the track is in the (x,y) acceptance of the chip, fill the  histoRecTracksInTelescope
        if( (trackXCoordinateAtLadder > chip->GetPositionX()) || (trackXCoordinateAtLadder < (chip->GetPositionX()-chip->GetSizeX()) ) ) continue;
        if( (trackYCoordinateAtLadder > chip->GetPositionY()) || (trackYCoordinateAtLadder < (chip->GetPositionY()-chip->GetSizeY()) ) ) continue;

        histoRecTracksInTelescope[iLadder]->Fill(whichChip);
        //If A cluster is found around the track-chip intersection point, fill histoRecTracksInChips
        Int_t clusterIntex;
        if(chip->IsAPointInACluster(trackXCoordinateAtLadder,trackYCoordinateAtLadder,clusterIntex)){
          histoRecTracksInChips[iLadder]->Fill(whichChip);
        }

      }


    }


  }

  TCanvas* canEfficiency[cNumberOfLadders];
  for(int iLadder =0;iLadder<cNumberOfLadders;iLadder++){
    const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
    //Create canvas and draw the two histograms and their ratio (efficiency)
    canEfficiency[iLadder] = new TCanvas(Form("canEfficiency_Ladder%d",iLadder),"canEfficiency",1000,800);
    SetCanvasStyle(canEfficiency[iLadder]);

    TPad *padMain, *padRatio;
    padMain = new TPad("padMain", "padMain",0,0.3,1,1,0);
    padMain->SetBottomMargin(0.);

    padRatio = new TPad("padRatio", "padRatio",0,0,1,0.3,0);
    padRatio->SetBottomMargin(0.4);
    padRatio->SetTopMargin(0.);
    padRatio->Draw();
    padMain->Draw();
    padMain->cd();


    histoRecTracksInTelescope[iLadder]->GetXaxis()->SetLabelSize(0.05);
    histoRecTracksInTelescope[iLadder]->GetXaxis()->SetTitleSize(0.05);
    histoRecTracksInTelescope[iLadder]->GetXaxis()->SetTitleOffset(1.02);
    histoRecTracksInTelescope[iLadder]->GetYaxis()->SetLabelSize(0.05);
    histoRecTracksInTelescope[iLadder]->GetYaxis()->SetTitleSize(0.05);
    histoRecTracksInTelescope[iLadder]->GetYaxis()->SetTitleOffset(0.8);
    histoRecTracksInTelescope[iLadder]->SetMarkerStyle(kOpenCircle);
    histoRecTracksInTelescope[iLadder]->SetMarkerColor(kBlack);
    histoRecTracksInTelescope[iLadder]->SetLineColor(kBlack);
    histoRecTracksInTelescope[iLadder]->Draw("");

    histoRecTracksInChips[iLadder]->SetMarkerStyle(kFullCircle);
    histoRecTracksInChips[iLadder]->SetMarkerColor(kRed);
    histoRecTracksInChips[iLadder]->SetLineColor(kRed);
    histoRecTracksInChips[iLadder]->Draw("same");

    TLegend* legEffi = new TLegend(0.11, 0.77, 0.83, 0.88);
    legEffi->SetFillStyle(0);
    legEffi->SetLineColorAlpha(0,0);
    legEffi->SetTextColor(kBlack);
    legEffi->SetMargin(0.1);
    legEffi->AddEntry(histoRecTracksInTelescope[iLadder],"Tracks in the telescope","pl");
    legEffi->AddEntry(histoRecTracksInChips[iLadder],"Tracks with clusters","pl");
    legEffi->Draw();

    TH1F *histoEffi = new TH1F(*histoRecTracksInChips[iLadder]);
    histoEffi->Divide(histoRecTracksInTelescope[iLadder]);
    histoEffi->SetMarkerStyle(kFullSquare);
    histoEffi->SetMarkerColor(kBlue);
    histoEffi->SetLineColor(kBlue);
    histoEffi->GetYaxis()->SetTitle("Efficiency");
    histoEffi->GetXaxis()->SetLabelSize(0.15);
    histoEffi->GetXaxis()->SetTitleSize(0.15);
    histoEffi->GetXaxis()->SetTitleOffset(1);
    histoEffi->GetYaxis()->SetLabelSize(0.12);
    histoEffi->GetYaxis()->SetTitleSize(0.11);
    histoEffi->GetYaxis()->CenterTitle();
    histoEffi->GetYaxis()->SetTitleOffset(0.35);
    for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
      histoEffi->GetXaxis()->SetBinLabel(iChip+1,Form("Chip %d",iChip));
    }
    canEfficiency[iLadder]->cd();
    padRatio->cd();
    histoEffi->Draw();

    canEfficiency[iLadder]->SaveAs(Form("%s/E-Efficiency_Ladder%d.pdf",outputDirectory.Data(),iLadder));
  }

  TFile *outputHistosFile = new TFile(Form("%s/AnalysisResults.root",outputDirectory.Data()),"UPDATE");
  TDirectory *effiDirectory = outputHistosFile->mkdir("Efficiency");
  effiDirectory->cd();

  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    canEfficiency[iLadder]->Write();
  }
  outputHistosFile->Close();

}
