//This macro is only for drawing of the hitmpas. It take the output of the previous macro (MakeHitsAndClusters.C).
//needed root classes
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TString.h"
#include "TMath.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TGraphErrors.h"
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
void DrawHitMaps(TString inputFileName = "TestBeamEventTree_WithClusters.root", TString outputDirectory = "TestRun")
{
  gStyle->SetOptStat(0);

  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    cLadderID[iLadder] = cLadderID_All[cParticipatedLadders[iLadder]];
    cNumberOfChipsInLadder[iLadder] = cNumberOfChipsInLadder_All[cParticipatedLadders[iLadder]];
    cXPositionOfFirstChipInLadder[iLadder] = cXPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cYPositionOfFirstChipInLadder[iLadder] = cYPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cZPositionOfFirstChipInLadder[iLadder] = cZPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cLadderChipsOriginRow[iLadder] = cLadderChipsOriginRow_All[cParticipatedLadders[iLadder]];
    cLadderChipsOriginCol[iLadder] = cLadderChipsOriginCol_All[cParticipatedLadders[iLadder]];
  }



  //The output of RawFileToTree.C. This tree will be cloned and upadted below
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* event = new TestBeamEvent();
  testBeamEventTree->SetBranchAddress("event", &event);
  Long_t numberOfEntries = testBeamEventTree->GetEntries();


  //define the histograms and create dummy array of chips where the clusters will be cumulated for all the events in order to be drawn on top of the hit mpas.

  TH2I ***histoHitMap = new TH2I**[cNumberOfLadders];
  TH2I ***histoHitMapXY = new TH2I**[cNumberOfLadders];
  TH1D ***histoHitMapX = new TH1D**[cNumberOfLadders];
  TH1D ***histoHitMapY = new TH1D**[cNumberOfLadders];
  TH1D ***histoHitMapCol = new TH1D**[cNumberOfLadders];
  TH1D ***histoHitMapRow = new TH1D**[cNumberOfLadders];
  TH1I ***histoNumberOfHitPerCluster = new TH1I**[cNumberOfLadders];
  TGraphErrors ***grClusters = new TGraphErrors**[cNumberOfLadders];
  TGraphErrors ***grClustersXY = new TGraphErrors**[cNumberOfLadders];

  for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
    histoHitMap[iLadder] = new TH2I*[numberOfChipsInLadder];
    histoHitMapXY[iLadder] = new TH2I*[numberOfChipsInLadder];
    histoHitMapX[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoHitMapY[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoHitMapCol[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoHitMapRow[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoNumberOfHitPerCluster[iLadder] = new TH1I*[numberOfChipsInLadder];
    grClusters[iLadder] = new TGraphErrors*[numberOfChipsInLadder];
    grClustersXY[iLadder] = new TGraphErrors*[numberOfChipsInLadder];

    for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){

      histoHitMap[iLadder][iChip] = new TH2I(Form("histoHitMap_Ladder%d_Chip%d",iLadder,iChip),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
      histoHitMap[iLadder][iChip]->Sumw2();

      histoHitMapXY[iLadder][iChip] = new TH2I(Form("histoHitMapXY_Ladder%d_Chip%d",iLadder,iChip),"",cNumberOfRowsInChip,cYPositionOfFirstChipInLadder[iLadder]-cChipYSize,cYPositionOfFirstChipInLadder[iLadder],cNumberOfColumnsInChip,(iChip-1)*cChipXSize+cXPositionOfFirstChipInLadder[iLadder],iChip*cChipXSize+cXPositionOfFirstChipInLadder[iLadder]);
      histoHitMapXY[iLadder][iChip]->Sumw2();

      histoNumberOfHitPerCluster[iLadder][iChip] = new TH1I(Form("histoNumberOfHitPerCluster_Ladder%d_Chip%d",iLadder,iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
      histoNumberOfHitPerCluster[iLadder][iChip]->Sumw2();

      //also define set of chips to be saved and cumulated in order to be used further for drawing purpose.
      grClusters[iLadder][iChip] = new TGraphErrors(1);
      grClusters[iLadder][iChip]->SetFillColorAlpha(kRed,0.2);
      grClusters[iLadder][iChip]->SetLineColorAlpha(kRed,0.2);
      grClusters[iLadder][iChip]->SetFillStyle(1001);

      grClustersXY[iLadder][iChip] = new TGraphErrors(1);
      grClustersXY[iLadder][iChip]->SetFillColorAlpha(kRed,0.2);
      grClustersXY[iLadder][iChip]->SetLineColorAlpha(kRed,0.2);
      grClustersXY[iLadder][iChip]->SetFillStyle(1001);
    }
  }

  //define the same histograms for the telescope:
  TH2I *histoHitMap_Telescope[cNumberOfChipsInTelescope];
  TH2I *histoHitMapXY_Telescope[cNumberOfChipsInTelescope];
  TH1D *histoHitMapX_Telescope[cNumberOfChipsInTelescope];
  TH1D *histoHitMapY_Telescope[cNumberOfChipsInTelescope];
  TH1D *histoHitMapCol_Telescope[cNumberOfChipsInTelescope];
  TH1D *histoHitMapRow_Telescope[cNumberOfChipsInTelescope];
  TH1I *histoNumberOfHitPerCluster_Telescope[cNumberOfChipsInTelescope];
  TGraphErrors *grClusters_Telescope[cNumberOfChipsInTelescope];
  TGraphErrors *grClustersXY_Telescope[cNumberOfChipsInTelescope];

  for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){

    histoHitMap_Telescope[iChip] = new TH2I(Form("histoHitMap_Telescope_Chip%d",cTelescopeChipID[iChip]),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
    histoHitMap_Telescope[iChip]->Sumw2();

    histoHitMapXY_Telescope[iChip] = new TH2I(Form("histoHitMapXY_Telescope_Chip%d",cTelescopeChipID[iChip]),"",cNumberOfRowsInChip,cYPositionOfChipsInTelescope[iChip]-cChipYSize,cYPositionOfChipsInTelescope[iChip],cNumberOfColumnsInChip,cXPositionOfChipsInTelescope[iChip]-cChipXSize,cXPositionOfChipsInTelescope[iChip]);
    histoHitMapXY_Telescope[iChip]->Sumw2();

    histoNumberOfHitPerCluster_Telescope[iChip] = new TH1I(Form("histoNumberOfHitPerCluster_Telescope_Chip%d",cTelescopeChipID[iChip]),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
    histoNumberOfHitPerCluster_Telescope[iChip]->Sumw2();

    //also define set of chips to be saved and cumulated in order to be used further for drawing purpose.
    grClusters_Telescope[iChip] = new TGraphErrors(1);
    grClusters_Telescope[iChip]->SetFillColorAlpha(kRed,0.2);
    grClusters_Telescope[iChip]->SetLineColorAlpha(kRed,0.2);
    grClusters_Telescope[iChip]->SetFillStyle(1001);

    //also define set of chips to be saved and cumulated in order to be used further for drawing purpose.
    grClustersXY_Telescope[iChip] = new TGraphErrors(1);
    grClustersXY_Telescope[iChip]->SetFillColorAlpha(kRed,0.2);
    grClustersXY_Telescope[iChip]->SetLineColorAlpha(kRed,0.2);
    grClustersXY_Telescope[iChip]->SetFillStyle(1001);
  }



  for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
    printf("Filling histograms  ... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");

    testBeamEventTree->GetEntry(iEvent);

    //Loop over the chips in the ladders and reconstruct their clusters
    for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
      AlpideLadder *ladder = event->GetLadder(iLadder);
      Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
      for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
        AlpideChip *chip = ladder->GetChip(iChip);
        //Fill the hit histograms
        Int_t numberOfHitPixels = chip->GetNumberOfHitPixels();
        for(int iHitPixel=0;iHitPixel<numberOfHitPixels;iHitPixel++){
          AlpidePixel *pixel = chip->GetHitPixel(iHitPixel);
          histoHitMap[iLadder][iChip]->Fill(pixel->GetCol(),pixel->GetRow());
          histoHitMapXY[iLadder][iChip]->Fill(pixel->GetYCoordinate(),pixel->GetXCoordinate());
        }
        Int_t numberOfClusters = chip->GetNumberOfClusters();
        for(int iCluster=0;iCluster<numberOfClusters;iCluster++){
          AlpideCluster *cluster = chip->GetCluster(iCluster);
          histoNumberOfHitPerCluster[iLadder][iChip]->Fill(cluster->GetNumberOfPixels());
          //Add the cluster to the TGraphErrors to be drawn at the end:
          Int_t numberOfPointsInGraph = grClusters[iLadder][iChip]->GetN();
          grClusters[iLadder][iChip]->SetPoint(numberOfPointsInGraph,cluster->GetCenterOfMassCol()-1,cluster->GetCenterOfMassRow());
          grClusters[iLadder][iChip]->SetPointError(numberOfPointsInGraph,0.5*cluster->GetColSpread()+2,0.5*cluster->GetRowSpread()+2);

          grClustersXY[iLadder][iChip]->SetPoint(numberOfPointsInGraph,cluster->GetCenterOfMassY(),cluster->GetCenterOfMassX());
          grClustersXY[iLadder][iChip]->SetPointError(numberOfPointsInGraph,2*cluster->GetSpreadY(),2*cluster->GetSpreadX());

        }
      }
    }

    //Loop over the chips in the telescop
    AlpideTelescope *telescope = event->GetTelescope();
    for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
      AlpideChip *chip = telescope->GetChip(iChip);
      //Fill the hit histograms
      Int_t numberOfHitPixels = chip->GetNumberOfHitPixels();
      for(int iHitPixel=0;iHitPixel<numberOfHitPixels;iHitPixel++){
        AlpidePixel *pixel = chip->GetHitPixel(iHitPixel);
        histoHitMap_Telescope[iChip]->Fill(pixel->GetCol(),pixel->GetRow());
        histoHitMapXY_Telescope[iChip]->Fill(pixel->GetYCoordinate(),pixel->GetXCoordinate());
      }
      Int_t numberOfClusters = chip->GetNumberOfClusters();
      for(int iCluster=0;iCluster<numberOfClusters;iCluster++){
        AlpideCluster *cluster = chip->GetCluster(iCluster);
        histoNumberOfHitPerCluster_Telescope[iChip]->Fill(cluster->GetNumberOfPixels());
        //Add the cluster to the TGraphErrors to be drawn at the end:
        Int_t numberOfPointsInGraph = grClusters_Telescope[iChip]->GetN();
        grClusters_Telescope[iChip]->SetPoint(numberOfPointsInGraph,cluster->GetCenterOfMassCol()-1,cluster->GetCenterOfMassRow());
        grClusters_Telescope[iChip]->SetPointError(numberOfPointsInGraph,0.5*cluster->GetColSpread()+2,0.5*cluster->GetRowSpread()+2);

        grClustersXY_Telescope[iChip]->SetPoint(numberOfPointsInGraph,cluster->GetCenterOfMassY(),cluster->GetCenterOfMassX());
        grClustersXY_Telescope[iChip]->SetPointError(numberOfPointsInGraph,2*cluster->GetSpreadY(),2*cluster->GetSpreadX());

      }
    }


  }


  //Plot the hitmapps and other histograms for the ladders
  TCanvas* canHitMaps[cNumberOfLadders];
  TCanvas* canHitMapsCol[cNumberOfLadders];
  TCanvas* canHitMapsRow[cNumberOfLadders];
  TCanvas* canHitMapsXY[cNumberOfLadders];
  TCanvas* canHitMapsX[cNumberOfLadders];
  TCanvas* canHitMapsY[cNumberOfLadders];
  TCanvas* canNumberOfHitsPerCluster[cNumberOfLadders];

  for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    int numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];

    canHitMaps[iLadder] = new TCanvas(Form("canHitMaps_Ladder%d",iLadder),"canHitMaps",1000,1000);
    canHitMaps[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMaps[iLadder]);
    canHitMaps[iLadder]->SetFillColor(kGray);

    canHitMapsCol[iLadder] = new TCanvas(Form("canHitMapsCol_Ladder%d",iLadder),"canHitMaps",1000,1000);
    canHitMapsCol[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsCol[iLadder]);
    canHitMapsCol[iLadder]->SetFillColor(kGray);

    canHitMapsRow[iLadder] = new TCanvas(Form("canHitMapsRow_Ladder%d",iLadder),"canHitMaps",1000,1000);
    canHitMapsRow[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsRow[iLadder]);
    canHitMapsRow[iLadder]->SetFillColor(kGray);

    canHitMapsXY[iLadder] = new TCanvas(Form("canHitMapsXY_Ladder%d",iLadder),"canHitMaps",500,1000);
    canHitMapsXY[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsXY[iLadder]);
    canHitMapsXY[iLadder]->SetFillColor(kGray);

    canHitMapsX[iLadder] = new TCanvas(Form("canHitMapsX_Ladder%d",iLadder),"canHitMaps",1000,1000);
    canHitMapsX[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsX[iLadder]);
    canHitMapsX[iLadder]->SetFillColor(kGray);

    canHitMapsY[iLadder] = new TCanvas(Form("canHitMapsY_Ladder%d",iLadder),"canHitMaps",1000,1000);
    canHitMapsY[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsY[iLadder]);
    canHitMapsY[iLadder]->SetFillColor(kGray);



    canNumberOfHitsPerCluster[iLadder] = new TCanvas(Form("canNumberOfHitsPerCluster_Ladder%d",iLadder),"canNumberOfHitsPerCluster",1000,1000);
    canNumberOfHitsPerCluster[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canNumberOfHitsPerCluster[iLadder]);
    canNumberOfHitsPerCluster[iLadder]->SetFillColor(kGray);



    for(int iChip=numberOfChipsInLadder-1;iChip>=0;iChip--){
      canHitMaps[iLadder]->cd(numberOfChipsInLadder-iChip);
      histoHitMap[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",iLadder,cTopChipId-numberOfChipsInLadder+iChip+1,histoHitMap[iLadder][iChip]->GetEntries()));
      histoHitMap[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoHitMap[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoHitMap[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoHitMap[iLadder][iChip]->GetXaxis()->SetTitle("Col");
      histoHitMap[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoHitMap[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoHitMap[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoHitMap[iLadder][iChip]->GetYaxis()->SetTitle("Row");
      histoHitMap[iLadder][iChip]->Draw("colz");

      if(cDrawClusters){
        grClusters[iLadder][iChip]->Draw("5same");
        // histoHitMap[iLadder][iChip]->Draw("same");
      }

      canHitMapsCol[iLadder]->cd(numberOfChipsInLadder-iChip);
      histoHitMapCol[iLadder][iChip] =  histoHitMap[iLadder][iChip]->ProjectionX(Form("histoHitMapCol_Ladder%d_Chip%d",iLadder,cTopChipId-numberOfChipsInLadder+iChip+1));
      histoHitMapCol[iLadder][iChip]->Draw();

      canHitMapsRow[iLadder]->cd(numberOfChipsInLadder-iChip);
      histoHitMapRow[iLadder][iChip] =  histoHitMap[iLadder][iChip]->ProjectionY(Form("histoHitMapRow_Ladder%d_Chip%d",iLadder,cTopChipId-numberOfChipsInLadder+iChip+1));
      histoHitMapRow[iLadder][iChip]->Draw();

      canHitMapsXY[iLadder]->cd(numberOfChipsInLadder-iChip);
      histoHitMapXY[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",iLadder,cTopChipId-numberOfChipsInLadder+iChip+1,histoHitMapXY[iLadder][iChip]->GetEntries()));
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetTitle("Y");
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetTitle("X");
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoHitMapXY[iLadder][iChip]->Draw("colz");

      if(cDrawClusters){
        grClustersXY[iLadder][iChip]->Draw("5same");
        // histoHitMapXY[iLadder][iChip]->Draw("same");
      }

      canHitMapsX[iLadder]->cd(numberOfChipsInLadder-iChip);
      histoHitMapX[iLadder][iChip] =  histoHitMapXY[iLadder][iChip]->ProjectionY(Form("histoHitMapX_Ladder%d_Chip%d",iLadder,cTopChipId-numberOfChipsInLadder+iChip+1));
      histoHitMapX[iLadder][iChip]->Draw();

      canHitMapsY[iLadder]->cd(numberOfChipsInLadder-iChip);
      histoHitMapY[iLadder][iChip] =  histoHitMapXY[iLadder][iChip]->ProjectionX(Form("histoHitMapY_Ladder%d_Chip%d",iLadder,cTopChipId-numberOfChipsInLadder+iChip+1));
      histoHitMapY[iLadder][iChip]->Draw();


      canNumberOfHitsPerCluster[iLadder]->cd(numberOfChipsInLadder-iChip);
      histoNumberOfHitPerCluster[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",iLadder,cTopChipId-numberOfChipsInLadder+iChip+1,histoNumberOfHitPerCluster[iLadder][iChip]->GetEntries()));
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitle("N_{pixels}/Cluster");
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitle("Counts");
      histoNumberOfHitPerCluster[iLadder][iChip]->Draw("");

    }

  }

  //Draw the same histograms for the telescope chips
  TCanvas* canHitMaps_Telescope = new TCanvas(Form("canHitMaps_Telescope"),"canHitMaps_Telescope",1000,1000);
  canHitMaps_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canHitMaps_Telescope);
  canHitMaps_Telescope->SetFillColor(kYellow-10);

  TCanvas* canHitMapsXY_Telescope = new TCanvas(Form("canHitMapsXY_Telescope"),"canHitMapsXY_Telescope",1000,1000);
  canHitMapsXY_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canHitMapsXY_Telescope);
  canHitMapsXY_Telescope->SetFillColor(kYellow-5);

  TCanvas* canHitMapsCol_Telescope = new TCanvas(Form("canHitMapsCol_Telescope"),"canHitMapsCol_Telescope",1000,1000);
  canHitMapsCol_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canHitMapsCol_Telescope);
  canHitMapsCol_Telescope->SetFillColor(kYellow-10);

  TCanvas* canHitMapsRow_Telescope = new TCanvas(Form("canHitMapsRow_Telescope"),"canHitMapsRow_Telescope",1000,1000);
  canHitMapsRow_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canHitMapsRow_Telescope);
  canHitMapsRow_Telescope->SetFillColor(kYellow-10);

  TCanvas* canHitMapsX_Telescope = new TCanvas(Form("canHitMapsX_Telescope"),"canHitMapsX_Telescope",1000,1000);
  canHitMapsX_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canHitMapsX_Telescope);
  canHitMapsX_Telescope->SetFillColor(kYellow-10);

  TCanvas* canHitMapsY_Telescope = new TCanvas(Form("canHitMapsY_Telescope"),"canHitMapsY_Telescope",1000,1000);
  canHitMapsY_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canHitMapsY_Telescope);
  canHitMapsY_Telescope->SetFillColor(kYellow-10);


  TCanvas* canNumberOfHitsPerCluster_Telescope = new TCanvas(Form("canNumberOfHitsPerCluster_Telescope"),"canNumberOfHitsPerCluster_Telescope",1000,1000);
  canNumberOfHitsPerCluster_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canNumberOfHitsPerCluster_Telescope);
  canNumberOfHitsPerCluster_Telescope->SetFillColor(kYellow-10);

  for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    canHitMaps_Telescope->cd(iChip+1);
    histoHitMap_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoHitMap_Telescope[iChip]->GetEntries()));
    histoHitMap_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoHitMap_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoHitMap_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoHitMap_Telescope[iChip]->GetXaxis()->SetTitle("Col");
    histoHitMap_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoHitMap_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoHitMap_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoHitMap_Telescope[iChip]->GetYaxis()->SetTitle("Row");
    histoHitMap_Telescope[iChip]->Draw("colz");

    if(cDrawClusters){
      grClusters_Telescope[iChip]->Draw("5same");
      // histoHitMap_Telescope[iChip]->Draw("same");
    }

    canHitMapsCol_Telescope->cd(iChip+1);
    histoHitMapCol_Telescope[iChip] =  histoHitMap_Telescope[iChip]->ProjectionX(Form("histoHitMapCol_Telescope_Chip%d",cTelescopeChipID[iChip]));
    histoHitMapCol_Telescope[iChip]->Draw();

    canHitMapsRow_Telescope->cd(iChip+1);
    histoHitMapRow_Telescope[iChip] =  histoHitMap_Telescope[iChip]->ProjectionY(Form("histoHitMapRow_Telescope_Chip%d",cTelescopeChipID[iChip]));
    histoHitMapRow_Telescope[iChip]->Draw();


    canHitMapsXY_Telescope->cd(iChip+1);
    histoHitMapXY_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip], histoHitMapXY_Telescope[iChip]->GetEntries()));
    histoHitMapXY_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoHitMapXY_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoHitMapXY_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoHitMapXY_Telescope[iChip]->GetXaxis()->SetTitle("Y");
    histoHitMapXY_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoHitMapXY_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoHitMapXY_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoHitMapXY_Telescope[iChip]->GetYaxis()->SetTitle("X");
    histoHitMapXY_Telescope[iChip]->Draw("colz");

    if(cDrawClusters){
      grClustersXY_Telescope[iChip]->Draw("5same");
      // histoHitMapXY_Telescope[iChip]->Draw("same");
    }

    canHitMapsX_Telescope->cd(iChip+1);
    histoHitMapX_Telescope[iChip] =  histoHitMapXY_Telescope[iChip]->ProjectionY(Form("histoHitMapX_Telescope_Chip%d",cTelescopeChipID[iChip]));
    histoHitMapX_Telescope[iChip]->Draw();

    canHitMapsY_Telescope->cd(iChip+1);
    histoHitMapY_Telescope[iChip] =  histoHitMapXY_Telescope[iChip]->ProjectionX(Form("histoHitMapY_Telescope_Chip%d",cTelescopeChipID[iChip]));
    histoHitMapY_Telescope[iChip]->Draw();


    canNumberOfHitsPerCluster_Telescope->cd(iChip+1);
    histoNumberOfHitPerCluster_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoNumberOfHitPerCluster_Telescope[iChip]->GetEntries()));
    histoNumberOfHitPerCluster_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoNumberOfHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoNumberOfHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoNumberOfHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitle("N_{pixels}/Cluster");
    histoNumberOfHitPerCluster_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoNumberOfHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoNumberOfHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoNumberOfHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitle("Counts");
    histoNumberOfHitPerCluster_Telescope[iChip]->Draw("");

  }

  //Create a TFile to write down the histograms
  TFile *outputHistosFile = new TFile(Form("%s/AnalysisResults.root",outputDirectory.Data()),"RECREATE");

  //An array of directories per ladder
  TDirectory *ladderDirectory[cNumberOfLadders];
  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    ladderDirectory[iLadder] = outputHistosFile->mkdir(Form("Ladder_%d",iLadder));
    ladderDirectory[iLadder]->cd();
    for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
      histoHitMap[iLadder][iChip]->Write();
      histoHitMapXY[iLadder][iChip]->Write();
      histoNumberOfHitPerCluster[iLadder][iChip]->Write();
    }
    canHitMaps[iLadder]->Write();
    canHitMapsXY[iLadder]->Write();
    canHitMapsCol[iLadder]->Write();
    canHitMapsRow[iLadder]->Write();
    canHitMapsX[iLadder]->Write();
    canHitMapsY[iLadder]->Write();
    canNumberOfHitsPerCluster[iLadder]->Write();
  }

  //A directory for the telescope histograms
  TDirectory *telescopeDirectory = outputHistosFile->mkdir("Telescope");
  telescopeDirectory->cd();
  for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    histoHitMap_Telescope[iChip]->Write();
    histoHitMapXY_Telescope[iChip]->Write();
    histoHitMapX_Telescope[iChip]->Write();
    histoHitMapY_Telescope[iChip]->Write();
    histoHitMapCol_Telescope[iChip]->Write();
    histoHitMapRow_Telescope[iChip]->Write();
    histoNumberOfHitPerCluster_Telescope[iChip]->Write();
  }
  canHitMaps_Telescope->Write();
  canHitMapsXY_Telescope->Write();
  canNumberOfHitsPerCluster_Telescope->Write();
  canHitMapsCol_Telescope->Write();
  canHitMapsRow_Telescope->Write();
  canHitMapsX_Telescope->Write();
  canHitMapsY_Telescope->Write();

  outputHistosFile->Close();

}
