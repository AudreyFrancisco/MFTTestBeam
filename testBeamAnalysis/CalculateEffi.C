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
#include "TLine.h"
//needed user-defined classes
#include "TestBeamEvent.hpp"
#include "AlpideChip.hpp"
#include "AlpidePixel.hpp"
#include "AlpideCluster.hpp"

Int_t MaxNumberOfTracks = 1;
void CalculateEffi(TString inputFileName = "TestBeamEventTree_WithTelescopeTracks.root", TString outputDirectory = "TestRun")
{
  //-----------------------------------------------------------------------------------------------------------------//
  //Get the alignment file with the bin corresponding to the run name
  TFile *inputAlignmentFile = new TFile("LaddersAlignment.root");
  TH2F *histoMisAlignment = ((TH2F*) inputAlignmentFile->Get("histoMisAlignment"));
  Int_t binNumberInAlignmentHisto = histoMisAlignment->GetXaxis()->FindBin(outputDirectory);

  //The output file of AddTelescopeTracks.C
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_TracksAndClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* event = new TestBeamEvent();
  testBeamEventTree_TracksAndClusters->SetBranchAddress("event", &event);
  Long_t numberOfEntries = testBeamEventTree_TracksAndClusters->GetEntries();


  TH1F *histoRecTracksInLadders = new TH1F("histoRecTracksInLadders","",6,0,6);
  histoRecTracksInLadders->Sumw2();


  TH1F *histoRecTracksInTelescope = new TH1F("histoRecTracksInTelescope","",6,0,6);
  histoRecTracksInTelescope->Sumw2();


  TH2F ***histo2DRecTracksInChips = new TH2F**[3];
  TH1F ***histoXRecTracksInChips = new TH1F**[3];
  TH1F ***histoYRecTracksInChips = new TH1F**[3];


  TH2F ***histo2DRecTracksInTelescope = new TH2F**[3];
  TH1F ***histoXRecTracksInTelescope = new TH1F**[3];
  TH1F ***histoYRecTracksInTelescope = new TH1F**[3];

  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    histoXRecTracksInChips[iLadder] = new TH1F*[3];
    histoXRecTracksInTelescope[iLadder] = new TH1F*[3];

    histoYRecTracksInChips[iLadder] = new TH1F*[3];
    histoYRecTracksInTelescope[iLadder] = new TH1F*[3];

    histo2DRecTracksInChips[iLadder] = new TH2F*[3];
    histo2DRecTracksInTelescope[iLadder] = new TH2F*[3];
    for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){

      histoXRecTracksInChips[iLadder][iChip] = new TH1F(Form("histoXRecTracksInChips_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",30,-0.5*cChipXSize-1,0.5*cChipXSize+1);
      histoXRecTracksInChips[iLadder][iChip]->Sumw2();

      histoYRecTracksInChips[iLadder][iChip] = new TH1F(Form("histoYRecTracksInChips_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",20,-0.5*cChipYSize-1,0.5*cChipYSize+1);
      histoYRecTracksInChips[iLadder][iChip]->Sumw2();

      histo2DRecTracksInChips[iLadder][iChip] = new TH2F(Form("histo2DRecTracksInChips_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",15,-0.5*cChipXSize-1,0.5*cChipXSize+1,10,-0.5*cChipYSize-1,0.5*cChipYSize+1);
      histo2DRecTracksInChips[iLadder][iChip]->Sumw2();

      histoXRecTracksInTelescope[iLadder][iChip] = new TH1F(Form("histoXRecTracksInTelescope_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",30,-0.5*cChipXSize-1,0.5*cChipXSize+1);
      histoXRecTracksInTelescope[iLadder][iChip]->Sumw2();

      histoYRecTracksInTelescope[iLadder][iChip] = new TH1F(Form("histoYRecTracksInTelescope_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",20,-0.5*cChipYSize-1,0.5*cChipYSize+1);
      histoYRecTracksInTelescope[iLadder][iChip]->Sumw2();

      histo2DRecTracksInTelescope[iLadder][iChip] = new TH2F(Form("histo2DRecTracksInTelescope_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",15,-0.5*cChipXSize-1,0.5*cChipXSize+1,10,-0.5*cChipYSize-1,0.5*cChipYSize+1);
      histo2DRecTracksInTelescope[iLadder][iChip]->Sumw2();


    }

  }


  Int_t totalEntries =0;
  for(int iEvent=0;iEvent<0.2*numberOfEntries;iEvent++){
    if(iEvent%10 == 0) printf("Filling Effi histos %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");
    testBeamEventTree_TracksAndClusters->GetEntry(iEvent);
    //loop over all the tracks, for each track, if the track extrapolation i supposed to hit a chip; check if the chip has a cluster around the track-chip intersection point. If the track is not supposed to meet the chip continue and dont store the track.
    AlpideTelescope *telescope = event->GetTelescope();
    Int_t numberOfTelescopeTracks = telescope->GetNumberOfTelescopeTracks();
    if(numberOfTelescopeTracks > MaxNumberOfTracks) continue;

    for(int iTrack=0;iTrack<numberOfTelescopeTracks;iTrack++){
      TelescopeTrack *track = telescope->GetTelescopeTrack(iTrack);

      for(Int_t iLadder =0;iLadder<cNumberOfLadders;iLadder++){
        Double_t misAlignX = histoMisAlignment->GetBinContent(binNumberInAlignmentHisto,2*iLadder+1);
        Double_t misAlignY = histoMisAlignment->GetBinContent(binNumberInAlignmentHisto,2*iLadder+2);
        // cout<<misAlignX<<endl;
        // cout<<misAlignY<<endl;
        for(Int_t iChip =0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
          AlpideLadder *ladder = event->GetLadder(iLadder);
          AlpideChip *chip = ladder->GetChip(iChip);
          chip->SetInitResoX(0.5);
          chip->SetInitResoY(0.5);
          // cout<<chip->GetPositionX()<<endl;
          // chip->SetPositionX(chip->GetPositionX()+10);
          // chip->SetPositionY(chip->GetPositionY()+10);
          // cout<<chip->GetPositionX()<<endl;
          Int_t numberOfClustersInChip = chip->GetNumberOfClusters();
          if(numberOfClustersInChip < 1) continue;

          Float_t trackXCoordinateAtLadder, trackYCoordinateAtLadder;
          track->GetXYCoordinate(trackXCoordinateAtLadder,trackYCoordinateAtLadder,cZPositionOfFirstChipInLadder[iLadder]);
          if( (trackXCoordinateAtLadder > (chip->GetPositionX()+cChipXSize)) || (trackXCoordinateAtLadder < (chip->GetPositionX()-cChipXSize)) ) continue;
          if( (trackYCoordinateAtLadder > (chip->GetPositionY()+cChipYSize)) || (trackYCoordinateAtLadder < (chip->GetPositionY()-cChipYSize)) ) continue;
          // cout<<Form("event %d: extrapolated %d: %3.3f, %3.3f",iEvent,chip->GetChipID(),trackXCoordinateAtLadder,trackYCoordinateAtLadder)<<endl;
          histoXRecTracksInTelescope[iLadder][iChip]->Fill(trackXCoordinateAtLadder-chip->GetPositionX());
          histoYRecTracksInTelescope[iLadder][iChip]->Fill(trackYCoordinateAtLadder-chip->GetPositionY());
          histo2DRecTracksInTelescope[iLadder][iChip]->Fill(trackXCoordinateAtLadder-chip->GetPositionX(),trackYCoordinateAtLadder-chip->GetPositionY());
          Int_t clusterIntex;
          if(chip->IsAPointInACluster(trackXCoordinateAtLadder,trackYCoordinateAtLadder,clusterIntex)){
            histo2DRecTracksInChips[iLadder][iChip]->Fill(trackXCoordinateAtLadder-chip->GetPositionX(),trackYCoordinateAtLadder-chip->GetPositionY());
            histoXRecTracksInChips[iLadder][iChip]->Fill(trackXCoordinateAtLadder-chip->GetPositionX());
            histoYRecTracksInChips[iLadder][iChip]->Fill(trackYCoordinateAtLadder-chip->GetPositionY());
            totalEntries++;
            // cout<<"Accepted"<<endl;
          }
          // cout<<"----------------------------------------------------------"<<endl;

        }
      }

    }////////////////////////////////////////////////////////////////////////


  }
  // gStyle->SetPalette(1);
  Int_t participatedChipsCounter=0;
  for(Int_t iLadder =0;iLadder<cNumberOfLadders;iLadder++){
    for(Int_t iChip =0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
      if(histo2DRecTracksInChips[iLadder][iChip]->GetEntries() < 0.02*totalEntries) continue;
      TCanvas* canEfficiency2D = new TCanvas(Form("canEfficiency2D_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",1000,800);
      SetCanvasStyle(canEfficiency2D);
      TH2F *histoEffi2D = new TH2F(*histo2DRecTracksInChips[iLadder][iChip]);
      histoEffi2D->Divide(histo2DRecTracksInTelescope[iLadder][iChip]);
      histoEffi2D->GetXaxis()->SetTitle("x (mm)");
      histoEffi2D->GetYaxis()->SetTitle("y (mm)");
      histoEffi2D->GetYaxis()->SetTitleOffset(1.2);
      histoEffi2D->SetTitle(Form("Efficiency: Run %s, Ladder%d, Chip%d",outputDirectory.Data(),cLadderID[iLadder],cTopChipId-iChip));
      histoEffi2D->Draw("colz");
      canEfficiency2D->SaveAs(Form("%s/effi2D_%d_chip%d.pdf",outputDirectory.Data(),cLadderID[iLadder],cTopChipId-iChip));

      TCanvas* canEffiXY = new TCanvas(Form("canEffiXY_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",1100,600);
      SetCanvasStyle(canEffiXY);
      canEffiXY->Divide(2,1);
      canEffiXY->cd(1);
      TH1F *histoEffiX = new TH1F(*histoXRecTracksInChips[iLadder][iChip]);
      histoEffiX->Divide(histoXRecTracksInTelescope[iLadder][iChip]);
      histoEffiX->SetLineColor(kBlack);
      histoEffiX->SetMarkerStyle(kFullCircle);
      histoEffiX->SetMarkerSize(0.8);
      histoEffiX->SetMarkerColor(kRed);
      histoEffiX->GetXaxis()->SetTitle("x (mm)");
      histoEffiX->GetYaxis()->SetTitle("effi");
      histoEffiX->GetYaxis()->SetTitleOffset(1.2);
      histoEffiX->SetTitle(Form("Efficiency: Run %s, Ladder%d, Chip%d",outputDirectory.Data(),cLadderID[iLadder],cTopChipId-iChip));
      histoEffiX->Draw("");
      TLine *lienAtOneX = new TLine(-0.5*cChipXSize-1,1,0.5*cChipXSize+1,1);
      lienAtOneX->SetLineColor(kGreen+8);
      lienAtOneX->SetLineWidth(1);
      lienAtOneX->SetLineStyle(7);
      lienAtOneX->Draw("same");
      histoEffiX->Draw("same");

      canEffiXY->cd(2);
      TH1F *histoEffiY = new TH1F(*histoYRecTracksInChips[iLadder][iChip]);
      histoEffiY->Divide(histoYRecTracksInTelescope[iLadder][iChip]);
      histoEffiY->SetLineColor(kBlack);
      histoEffiY->SetMarkerStyle(kFullCircle);
      histoEffiY->SetMarkerSize(0.8);
      histoEffiY->SetMarkerColor(kRed);
      histoEffiY->GetXaxis()->SetTitle("y (mm)");
      histoEffiY->GetYaxis()->SetTitle("effi");
      histoEffiY->GetYaxis()->SetTitleOffset(1.2);
      histoEffiY->SetTitle(Form("Efficiency: Run %s, Ladder%d, Chip%d",outputDirectory.Data(),cLadderID[iLadder],cTopChipId-iChip));
      histoEffiY->Draw("");
      TLine *lienAtOneY = new TLine(-0.5*cChipYSize-1,1,0.5*cChipYSize+1,1);
      lienAtOneY->SetLineColor(kGreen+8);
      lienAtOneY->SetLineWidth(1);
      lienAtOneY->SetLineStyle(7);
      lienAtOneY->Draw("same");
      histoEffiY->Draw("same");
      canEffiXY->SaveAs(Form("%s/effiXY_%d_chip%d.pdf",outputDirectory.Data(),cLadderID[iLadder],cTopChipId-iChip));

      participatedChipsCounter++;
      histoRecTracksInLadders->SetBinContent(participatedChipsCounter,histo2DRecTracksInChips[iLadder][iChip]->GetEntries());
      histoRecTracksInLadders->SetBinError(participatedChipsCounter,TMath::Sqrt(histo2DRecTracksInChips[iLadder][iChip]->GetEntries()));
      histoRecTracksInTelescope->SetBinContent(participatedChipsCounter,histo2DRecTracksInTelescope[iLadder][iChip]->GetEntries());
      histoRecTracksInTelescope->SetBinError(participatedChipsCounter,TMath::Sqrt(histo2DRecTracksInTelescope[iLadder][iChip]->GetEntries()));
      histoRecTracksInLadders->GetXaxis()->SetBinLabel(participatedChipsCounter,Form("%d_%d",cLadderID[iLadder],cTopChipId-iChip));

    }
  }



  TCanvas* canEfficiency = new TCanvas("canEfficiency","",1000,800);
  SetCanvasStyle(canEfficiency);

  TPad *padMain, *padRatio;
  padMain = new TPad("padMain", "padMain",0,0.3,1,1,0);
  padMain->SetBottomMargin(0.);

  padRatio = new TPad("padRatio", "padRatio",0,0,1,0.3,0);
  padRatio->SetBottomMargin(0.4);
  padRatio->SetTopMargin(0.);
  padRatio->Draw();
  padMain->Draw();
  padMain->cd();


  histoRecTracksInTelescope->GetXaxis()->SetLabelSize(0.05);
  histoRecTracksInTelescope->GetXaxis()->SetTitleSize(0.05);
  histoRecTracksInTelescope->GetXaxis()->SetTitleOffset(1.02);
  histoRecTracksInTelescope->GetYaxis()->SetLabelSize(0.05);
  histoRecTracksInTelescope->GetYaxis()->SetTitleSize(0.05);
  histoRecTracksInTelescope->GetYaxis()->SetTitleOffset(0.8);
  histoRecTracksInTelescope->SetMarkerStyle(kOpenCircle);
  histoRecTracksInTelescope->SetMarkerColor(kBlack);
  histoRecTracksInTelescope->SetLineColor(kBlack);
  histoRecTracksInTelescope->Draw("");

  histoRecTracksInLadders->SetMarkerStyle(kFullCircle);
  histoRecTracksInLadders->SetMarkerColor(kRed);
  histoRecTracksInLadders->SetLineColor(kRed);
  histoRecTracksInLadders->GetXaxis()->SetTitle("Ladder_Chip");
  histoRecTracksInLadders->Draw("same");

  TLegend* legEffi = new TLegend(0.64, 0.71, 0.96, 0.87);
  legEffi->SetFillStyle(0);
  legEffi->SetLineColorAlpha(0,0);
  legEffi->SetTextColor(kBlack);
  legEffi->SetMargin(0.1);
  legEffi->AddEntry("NULL",Form("Run %s",outputDirectory.Data()),"");
  legEffi->AddEntry(histoRecTracksInTelescope,"Reconstructed tracks","pl");
  legEffi->AddEntry(histoRecTracksInLadders,"With clusters in the chip","pl");
  legEffi->Draw();

  TH1F *histoEffi = new TH1F(*histoRecTracksInLadders);
  histoEffi->Divide(histoRecTracksInTelescope);
  histoEffi->SetMarkerStyle(kFullSquare);
  histoEffi->SetMarkerColor(kBlue);
  histoEffi->SetLineColor(kBlue);
  histoEffi->GetYaxis()->SetTitle("Efficiency");
  histoEffi->GetXaxis()->SetLabelSize(0.15);
  histoEffi->GetXaxis()->SetTitleSize(0.1);
  histoEffi->GetXaxis()->SetTitleOffset(1);
  histoEffi->GetYaxis()->SetLabelSize(0.12);
  histoEffi->GetYaxis()->SetTitleSize(0.11);
  histoEffi->GetYaxis()->CenterTitle();
  histoEffi->GetYaxis()->SetTitleOffset(0.35);

  canEfficiency->cd();
  padRatio->cd();
  histoEffi->Draw();
  canEfficiency->SaveAs(Form("%s/effiGloabl.pdf",outputDirectory.Data()));

}
