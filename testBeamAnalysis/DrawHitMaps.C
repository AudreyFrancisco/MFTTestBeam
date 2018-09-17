//This macro is only for drawing of the hitmpas. It take the output of the previous macro (MakeHitsAndClusters.C).
//needed root classes
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH3F.h"
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

//needed macros
#include "Common.C"

//The main function
void DrawHitMaps(TString inputFileNameHits = "Results_OnlyHits.root",TString inputFileNameClusters = "Results_WithClusters.root", TString outputDirectory = "TestRun")
{
  gStyle->SetOptStat(0);

  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    cLadderID[iLadder] = cLadderID_All[cParticipatedLadders[iLadder]];
    cNumberOfChipsInLadder[iLadder] = cNumberOfChipsInLadder_All[cParticipatedLadders[iLadder]];
    cXPositionOfFirstChipInLadder[iLadder] = cXPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cYPositionOfFirstChipInLadder[iLadder] = cYPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cZPositionOfFirstChipInLadder[iLadder] = cZPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
  }



  //The output of RawFileToTree.C. This tree will be cloned and upadted below
  TFile *inputFile;
  inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileNameClusters.Data()));
  if(!inputFile){
    inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileNameHits.Data()));
  }
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
  TH1I ***histoNumberOfClusterPerEvent = new TH1I**[cNumberOfLadders];
  TH1I ***histoClusterSpreadX = new TH1I**[cNumberOfLadders];
  TH1I ***histoClusterSpreadY = new TH1I**[cNumberOfLadders];
  TH2I ***histoSpreadXVsNHitPerCluster = new TH2I**[cNumberOfLadders];
  TH2I ***histoSpreadYVsNHitPerCluster = new TH2I**[cNumberOfLadders];
  TH2I ***histoSpreadXY = new TH2I**[cNumberOfLadders];
  TH3I ***histoSpreadXYVsNHitPerCluster = new TH3I**[cNumberOfLadders];
  TH1I ***histoRelativeXDevOfCOM = new TH1I**[cNumberOfLadders];
  TH1I ***histoRelativeYDevOfCOM = new TH1I**[cNumberOfLadders];
  TH1I ***histoRelativePositionOfCOM = new TH1I**[cNumberOfLadders];

  TGraphErrors ***probaClusterSize = new TGraphErrors**[cNumberOfLadders];
  TGraphErrors ***grClusters = new TGraphErrors**[cNumberOfLadders];


  for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
    histoHitMap[iLadder] = new TH2I*[numberOfChipsInLadder];
    histoHitMapXY[iLadder] = new TH2I*[numberOfChipsInLadder];
    histoHitMapX[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoHitMapY[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoHitMapCol[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoHitMapRow[iLadder] = new TH1D*[numberOfChipsInLadder];
    histoNumberOfHitPerCluster[iLadder] = new TH1I*[numberOfChipsInLadder];
    histoNumberOfClusterPerEvent[iLadder] = new TH1I*[numberOfChipsInLadder];
    histoClusterSpreadX[iLadder] = new TH1I*[numberOfChipsInLadder];
    histoClusterSpreadY[iLadder] = new TH1I*[numberOfChipsInLadder];
    histoSpreadXVsNHitPerCluster[iLadder] = new TH2I*[numberOfChipsInLadder];
    histoSpreadYVsNHitPerCluster[iLadder] = new TH2I*[numberOfChipsInLadder];
    histoSpreadXY[iLadder] = new TH2I*[numberOfChipsInLadder];
    histoSpreadXYVsNHitPerCluster[iLadder] = new TH3I*[numberOfChipsInLadder];
    histoRelativeXDevOfCOM[iLadder] = new TH1I*[numberOfChipsInLadder];
    histoRelativeYDevOfCOM[iLadder] = new TH1I*[numberOfChipsInLadder];
    histoRelativePositionOfCOM[iLadder] = new TH1I*[numberOfChipsInLadder];
    // histoProbaClusterSize[iLadder] = new TH1D*[numberOfChipsInLadder];
    probaClusterSize[iLadder] = new TGraphErrors*[numberOfChipsInLadder];
    grClusters[iLadder] = new TGraphErrors*[numberOfChipsInLadder];

    for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){

      histoHitMap[iLadder][iChip] = new TH2I(Form("histoHitMap_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
      histoHitMap[iLadder][iChip]->Sumw2();

      histoHitMapXY[iLadder][iChip] = new TH2I(Form("histoHitMapXY_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cNumberOfRowsInChip,cYPositionOfFirstChipInLadder[iLadder]-0.5*cChipYSize,cYPositionOfFirstChipInLadder[iLadder]+0.5*cChipYSize,cNumberOfColumnsInChip,-1*iChip*cChipXSize+cXPositionOfFirstChipInLadder[iLadder]-0.5*cChipXSize,-1*iChip*cChipXSize+cXPositionOfFirstChipInLadder[iLadder]+0.5*cChipXSize);
      histoHitMapXY[iLadder][iChip]->Sumw2();

      histoNumberOfHitPerCluster[iLadder][iChip] = new TH1I(Form("histoNumberOfHitPerCluster_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
      histoNumberOfHitPerCluster[iLadder][iChip]->Sumw2();

      histoNumberOfClusterPerEvent[iLadder][iChip] = new TH1I(Form("histoNumberOfClusterPerEvent_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cMaxNumberOfClusterPerEvent,0,cMaxNumberOfClusterPerEvent);
      histoNumberOfClusterPerEvent[iLadder][iChip]->Sumw2();

      histoClusterSpreadX[iLadder][iChip] = new TH1I(Form("histoClusterSpreadX_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip);
      histoClusterSpreadX[iLadder][iChip]->Sumw2();

      histoClusterSpreadY[iLadder][iChip] = new TH1I(Form("histoClusterSpreadY_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cNumberOfRowsInChip,0,cNumberOfRowsInChip);
      histoClusterSpreadY[iLadder][iChip]->Sumw2();

      histoSpreadXVsNHitPerCluster[iLadder][iChip] = new TH2I(Form("histoSpreadXVsNHitPerCluster_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cNumberOfColumnsInChip,0,cNumberOfColumnsInChip);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->Sumw2();

      histoSpreadYVsNHitPerCluster[iLadder][iChip] = new TH2I(Form("histoSpreadYVsNHitPerCluster_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->Sumw2();

      histoSpreadXY[iLadder][iChip] = new TH2I(Form("histoSpreadXY_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
      histoSpreadXY[iLadder][iChip]->Sumw2();

      histoSpreadXYVsNHitPerCluster[iLadder][iChip] = new TH3I(Form("histoSpreadXYVsNHitPerCluster_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->Sumw2();

      histoRelativeXDevOfCOM[iLadder][iChip] = new TH1I(Form("histoRelativeXDevOfCOM_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",51,-cMaxSpreadX/10,cMaxSpreadX/10);
      histoRelativeXDevOfCOM[iLadder][iChip]->Sumw2();

      histoRelativeYDevOfCOM[iLadder][iChip] = new TH1I(Form("histoRelativeYDevOfCOM_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",51,-cMaxSpreadX/10,cMaxSpreadX/10);
      histoRelativeYDevOfCOM[iLadder][iChip]->Sumw2();

      histoRelativePositionOfCOM[iLadder][iChip] = new TH1I(Form("histoRelativePositionOfCOM_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",50,0,cMaxSpreadX/10);
      histoRelativePositionOfCOM[iLadder][iChip]->Sumw2();

      probaClusterSize[iLadder][iChip] = new TGraphErrors(1);
      probaClusterSize[iLadder][iChip]->SetTitle(Form("probaClusterSize_Ladder%d_Chip%d",iLadder,iChip));
      probaClusterSize[iLadder][iChip]->SetFillColorAlpha(kRed,0.2);
      probaClusterSize[iLadder][iChip]->SetLineColorAlpha(kRed,0.2);
      probaClusterSize[iLadder][iChip]->SetFillStyle(0);

      //also define set of chips to be saved and cumulated in order to be used further for drawing purpose.
      grClusters[iLadder][iChip] = new TGraphErrors(1);
      grClusters[iLadder][iChip]->SetFillColorAlpha(kRed,0.2);
      grClusters[iLadder][iChip]->SetLineColorAlpha(kRed,0.2);
      grClusters[iLadder][iChip]->SetFillStyle(0);

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
  TH1I *histoNumberOfClusterPerEvent_Telescope[cNumberOfChipsInTelescope];
  TH1I *histoClusterSpreadX_Telescope[cNumberOfChipsInTelescope];
  TH1I *histoClusterSpreadY_Telescope[cNumberOfChipsInTelescope];
  TH2I *histoSpreadXVsNHitPerCluster_Telescope[cNumberOfChipsInTelescope];
  TH2I *histoSpreadYVsNHitPerCluster_Telescope[cNumberOfChipsInTelescope];
  TH2I *histoSpreadXY_Telescope[cNumberOfChipsInTelescope];
  TH3I *histoSpreadXYVsNHitPerCluster_Telescope[cNumberOfChipsInTelescope];
  TH1I *histoRelativeXDevOfCOM_Telescope[cNumberOfChipsInTelescope];
  TH1I *histoRelativeYDevOfCOM_Telescope[cNumberOfChipsInTelescope];
  TH1I *histoRelativePositionOfCOM_Telescope[cNumberOfChipsInTelescope];
  // TH1D *histoProbaClusterSize_Telescope[cNumberOfChipsInTelescope];

  TGraphErrors *probaClusterSize_Telescope[cNumberOfChipsInTelescope];
  TGraphErrors *grClusters_Telescope[cNumberOfChipsInTelescope];

  for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){

    histoHitMap_Telescope[iChip] = new TH2I(Form("histoHitMap_Telescope_Chip%d",cTelescopeChipID[iChip]),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
    histoHitMap_Telescope[iChip]->Sumw2();

    histoHitMapXY_Telescope[iChip] = new TH2I(Form("histoHitMapXY_Telescope_Chip%d",cTelescopeChipID[iChip]),"",cNumberOfRowsInChip,cYPositionOfChipsInTelescope[iChip]-0.5*cChipYSize,cYPositionOfChipsInTelescope[iChip]+0.5*cChipYSize,cNumberOfColumnsInChip,cXPositionOfChipsInTelescope[iChip]-0.5*cChipXSize,cXPositionOfChipsInTelescope[iChip]+0.5*cChipXSize);
    histoHitMapXY_Telescope[iChip]->Sumw2();

    histoNumberOfHitPerCluster_Telescope[iChip] = new TH1I(Form("histoNumberOfHitPerCluster_Telescope_Chip%d",cTelescopeChipID[iChip]),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
    histoNumberOfHitPerCluster_Telescope[iChip]->Sumw2();

    histoNumberOfClusterPerEvent_Telescope[iChip] = new TH1I(Form("histoNumberOfClusterPerEvent_Telescope_Chip%d",cTelescopeChipID[iChip]),"",cMaxNumberOfClusterPerEvent,0,cMaxNumberOfClusterPerEvent);
    histoNumberOfClusterPerEvent_Telescope[iChip]->Sumw2();

    histoClusterSpreadX_Telescope[iChip] = new TH1I(Form("histoClusterSpreadX_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip);
    histoClusterSpreadX_Telescope[iChip]->Sumw2();

    histoClusterSpreadY_Telescope[iChip] = new TH1I(Form("histoClusterSpreadY_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",cNumberOfRowsInChip,0,cNumberOfRowsInChip);
    histoClusterSpreadY_Telescope[iChip]->Sumw2();

    histoSpreadXVsNHitPerCluster_Telescope[iChip] = new TH2I(Form("histoSpreadXVsNHitPerCluster_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cNumberOfColumnsInChip,0,cNumberOfColumnsInChip);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->Sumw2();

    histoSpreadYVsNHitPerCluster_Telescope[iChip] = new TH2I(Form("histoSpreadYVsNHitPerCluster_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->Sumw2();

    histoSpreadXY_Telescope[iChip] = new TH2I(Form("histoSpreadXY_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
    histoSpreadXY_Telescope[iChip]->Sumw2();

    histoSpreadXYVsNHitPerCluster_Telescope[iChip] = new TH3I(Form("histoSpreadXYVsNHitPerCluster_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->Sumw2();

    histoRelativeXDevOfCOM_Telescope[iChip] = new TH1I(Form("histoRelativeXDevOfCOM_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",51,-cMaxSpreadX/10,cMaxSpreadX/10);
    histoRelativeXDevOfCOM_Telescope[iChip]->Sumw2();

    histoRelativeYDevOfCOM_Telescope[iChip] = new TH1I(Form("histoRelativeYDevOfCOM_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",51,-cMaxSpreadX/10,cMaxSpreadX/10);
    histoRelativeYDevOfCOM_Telescope[iChip]->Sumw2();

    histoRelativePositionOfCOM_Telescope[iChip] = new TH1I(Form("histoRelativePositionOfCOM_Telescope%d_Chip%d",cTelescopeChipID[iChip],cTopChipId-iChip),"",50,0,cMaxSpreadX/10);
    histoRelativePositionOfCOM_Telescope[iChip]->Sumw2();

    probaClusterSize_Telescope[iChip] = new TGraphErrors(1);
    probaClusterSize_Telescope[iChip]->SetTitle(Form("probaClusterSize_Telescope_Chip%d",iChip));
    probaClusterSize_Telescope[iChip]->SetFillColorAlpha(kRed,0.2);
    probaClusterSize_Telescope[iChip]->SetLineColorAlpha(kRed,0.2);
    probaClusterSize_Telescope[iChip]->SetFillStyle(0);
    //also define set of chips to be saved and cumulated in order to be used further for drawing purpose.
    grClusters_Telescope[iChip] = new TGraphErrors(1);
    grClusters_Telescope[iChip]->SetFillColorAlpha(kRed,0.2);
    grClusters_Telescope[iChip]->SetLineColorAlpha(kRed,0.2);
    grClusters_Telescope[iChip]->SetFillStyle(0);

  }

  // TODO : make the number of chips per ladder variable
  Int_t totalNumberOfClusters_Ladder[cNumberOfLadders][3] = {0};
  Int_t totalNumberOfClusters_Telescope[cNumberOfChipsInTelescope] = {0};

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
        totalNumberOfClusters_Ladder[iLadder][iChip]+=numberOfClusters;
        histoNumberOfClusterPerEvent[iLadder][iChip]->Fill(numberOfClusters);
        for(int iCluster=0;iCluster<numberOfClusters;iCluster++){
          AlpideCluster *cluster = chip->GetCluster(iCluster);
          histoNumberOfHitPerCluster[iLadder][iChip]->Fill(cluster->GetNumberOfPixels());
          histoClusterSpreadX[iLadder][iChip]->Fill(cluster->GetColSpread());
          histoClusterSpreadY[iLadder][iChip]->Fill(cluster->GetRowSpread());
          histoSpreadXVsNHitPerCluster[iLadder][iChip]->Fill(cluster->GetNumberOfPixels(),cluster->GetColSpread());
          histoSpreadYVsNHitPerCluster[iLadder][iChip]->Fill(cluster->GetNumberOfPixels(),cluster->GetRowSpread());
          histoSpreadXY[iLadder][iChip]->Fill(cluster->GetColSpread(),cluster->GetRowSpread());
          histoSpreadXYVsNHitPerCluster[iLadder][iChip]->Fill(cluster->GetColSpread(),cluster->GetRowSpread(),cluster->GetNumberOfPixels());
          histoRelativeXDevOfCOM[iLadder][iChip]->Fill( cluster->GetCenterOfMassX() - (cluster->GetXMax() - (cluster->GetSpreadX())/2) );
          histoRelativeYDevOfCOM[iLadder][iChip]->Fill( cluster->GetCenterOfMassY() - (cluster->GetYMax() - (cluster->GetSpreadY())/2) );
          histoRelativePositionOfCOM[iLadder][iChip]->Fill(sqrt( pow(cluster->GetCenterOfMassX() - (cluster->GetXMax() - (cluster->GetSpreadX())/2),2) + pow(cluster->GetCenterOfMassY() - (cluster->GetYMax() - (cluster->GetSpreadY())/2),2) ));
          // if(cluster->GetNumberOfPixels() > 50){
          //   printf("Ladder %d, chip %d : %d pixels cluster found for event %d \n",iLadder,iChip,cluster->GetNumberOfPixels(),iEvent);
          // }
          if(cluster->GetNumberOfPixels() < cLimitOfLargeCluster) continue;
          //Add the cluster to the TGraphErrors to be drawn at the end:
          Int_t numberOfPointsInGraph = grClusters[iLadder][iChip]->GetN();
          grClusters[iLadder][iChip]->SetPoint(numberOfPointsInGraph,cluster->GetCenterOfMassCol()-1,cluster->GetCenterOfMassRow());
          grClusters[iLadder][iChip]->SetPointError(numberOfPointsInGraph,0.5*cluster->GetColSpread()+2,0.5*cluster->GetRowSpread()+2);

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
      totalNumberOfClusters_Telescope[iChip]+=numberOfClusters;
      histoNumberOfClusterPerEvent_Telescope[iChip]->Fill(numberOfClusters);
      for(int iCluster=0;iCluster<numberOfClusters;iCluster++){
        AlpideCluster *cluster = chip->GetCluster(iCluster);
        histoNumberOfHitPerCluster_Telescope[iChip]->Fill(cluster->GetNumberOfPixels());
        histoClusterSpreadX_Telescope[iChip]->Fill(cluster->GetColSpread());
        histoClusterSpreadY_Telescope[iChip]->Fill(cluster->GetRowSpread());
        histoSpreadXVsNHitPerCluster_Telescope[iChip]->Fill(cluster->GetNumberOfPixels(),cluster->GetColSpread());
        histoSpreadYVsNHitPerCluster_Telescope[iChip]->Fill(cluster->GetNumberOfPixels(),cluster->GetRowSpread());
        histoSpreadXY_Telescope[iChip]->Fill(cluster->GetColSpread(),cluster->GetRowSpread());
        histoSpreadXYVsNHitPerCluster_Telescope[iChip]->Fill(cluster->GetColSpread(),cluster->GetRowSpread(),cluster->GetNumberOfPixels());
        histoRelativeXDevOfCOM_Telescope[iChip]->Fill( cluster->GetCenterOfMassX() - (cluster->GetXMax() - (cluster->GetSpreadX())/2) );
        histoRelativeYDevOfCOM_Telescope[iChip]->Fill( cluster->GetCenterOfMassY() - (cluster->GetYMax() - (cluster->GetSpreadY())/2) );
        histoRelativePositionOfCOM_Telescope[iChip]->Fill(sqrt( pow(cluster->GetCenterOfMassX() - (cluster->GetXMax() - (cluster->GetSpreadX())/2),2) + pow(cluster->GetCenterOfMassY() - (cluster->GetYMax() - (cluster->GetSpreadY())/2),2) ));
        // if(cluster->GetNumberOfPixels() > 50){
        //   printf("Telescope, chip %d : %d pixels cluster found for event %d \n",iChip,cluster->GetNumberOfPixels(),iEvent);
        // }
        if(cluster->GetNumberOfPixels() < cLimitOfLargeCluster) continue;
        //Add the cluster to the TGraphErrors to be drawn at the end:
        Int_t numberOfPointsInGraph = grClusters_Telescope[iChip]->GetN();
        grClusters_Telescope[iChip]->SetPoint(numberOfPointsInGraph,cluster->GetCenterOfMassCol()-1,cluster->GetCenterOfMassRow());
        grClusters_Telescope[iChip]->SetPointError(numberOfPointsInGraph,0.5*cluster->GetColSpread()+2,0.5*cluster->GetRowSpread()+2);

      }
    }
  }

  Int_t binMin = 0;
  Int_t binMax = 0;
  for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
    for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
        // printf("totalNumberOfClusters_Ladder[%d][%d] = %d\n",iLadder,iChip,totalNumberOfClusters_Ladder[iLadder][iChip]);
      for(int iClusterSize=1;iClusterSize<cMaxNumberOfHitsPerCluster;iClusterSize++){
        binMin = histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->FindBin(1.5);
        binMax = histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->FindBin(iClusterSize+0.5);
        probaClusterSize[iLadder][iChip]->SetPoint(iClusterSize-1,iClusterSize,histoNumberOfHitPerCluster[iLadder][iChip]->Integral(binMin,binMax)/totalNumberOfClusters_Ladder[iLadder][iChip]);
        probaClusterSize[iLadder][iChip]->SetPointError(iClusterSize-1,iClusterSize/iClusterSize,(histoNumberOfHitPerCluster[iLadder][iChip]->Integral(binMin,binMax)/totalNumberOfClusters_Ladder[iLadder][iChip])/20.);
        // printf("Ladder %d  : binMin = %d  binMax = %d  binContent = %f  Integral = %f  Filling : %f \n",iLadder,binMin,binMax,histoNumberOfHitPerCluster[iLadder][iChip]->GetBinContent(binMax),histoNumberOfHitPerCluster[iLadder][iChip]->Integral(binMin,binMax),histoNumberOfHitPerCluster[iLadder][iChip]->Integral(binMin,binMax)/totalNumberOfClusters_Ladder[iLadder][iChip]);
      }
    }
  }

  for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    // printf("totalNumberOfClusters_Telescope[%d] = %d\n",iChip,totalNumberOfClusters_Telescope[iChip]);
    for(int iClusterSize=1;iClusterSize<cMaxNumberOfHitsPerCluster;iClusterSize++){
      binMin = histoNumberOfHitPerCluster_Telescope[iChip]->GetXaxis()->FindBin(1.5);
      binMax = histoNumberOfHitPerCluster_Telescope[iChip]->GetXaxis()->FindBin(iClusterSize+0.5);
      probaClusterSize_Telescope[iChip]->SetPoint(iClusterSize-1,iClusterSize,histoNumberOfHitPerCluster_Telescope[iChip]->Integral(binMin,binMax)/totalNumberOfClusters_Telescope[iChip]);
      probaClusterSize_Telescope[iChip]->SetPointError(iClusterSize-1,iClusterSize/iClusterSize,(histoNumberOfHitPerCluster_Telescope[iChip]->Integral(binMin,binMax)/totalNumberOfClusters_Telescope[iChip])/20.);
      // printf("Telescope : binMin = %d  binMax = %d  binContent = %f  Integral = %f  Filling : %f\n",binMin,binMax,histoNumberOfHitPerCluster_Telescope[iChip]->GetBinContent(binMax),histoNumberOfHitPerCluster_Telescope[iChip]->Integral(binMin,binMax),histoNumberOfHitPerCluster_Telescope[iChip]->Integral(binMin,binMax)/totalNumberOfClusters_Telescope[iChip]);
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
  TCanvas* canNumberOfClusterPerEvent[cNumberOfLadders];
  TCanvas* canSpreadX[cNumberOfLadders];
  TCanvas* canSpreadY[cNumberOfLadders];
  TCanvas* canSpreadXVsNHitPerCluster[cNumberOfLadders];
  TCanvas* canSpreadYVsNHitPerCluster[cNumberOfLadders];
  TCanvas* canSpreadXY[cNumberOfLadders];
  TCanvas* canSpreadXYVsNHitPerCluster[cNumberOfLadders];
  TCanvas* canRelativeXDevOfCOM[cNumberOfLadders];
  TCanvas* canRelativeYDevOfCOM[cNumberOfLadders];
  TCanvas* canRelativePositionOfCOM[cNumberOfLadders];
  TCanvas* canProbaClusterSize[cNumberOfLadders];

  for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    int numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];

    canHitMaps[iLadder] = new TCanvas(Form("canHitMaps_Ladder%d",cLadderID[iLadder]),"canHitMaps",1000,1000);
    canHitMaps[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMaps[iLadder]);
    canHitMaps[iLadder]->SetFillColor(kGray);

    canHitMapsCol[iLadder] = new TCanvas(Form("canHitMapsCol_Ladder%d",cLadderID[iLadder]),"canHitMaps",1000,1000);
    canHitMapsCol[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsCol[iLadder]);
    canHitMapsCol[iLadder]->SetFillColor(kGray);

    canHitMapsRow[iLadder] = new TCanvas(Form("canHitMapsRow_Ladder%d",cLadderID[iLadder]),"canHitMaps",1000,1000);
    canHitMapsRow[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsRow[iLadder]);
    canHitMapsRow[iLadder]->SetFillColor(kGray);

    canHitMapsXY[iLadder] = new TCanvas(Form("canHitMapsXY_Ladder%d",cLadderID[iLadder]),"canHitMaps",500,1000);
    canHitMapsXY[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsXY[iLadder]);
    canHitMapsXY[iLadder]->SetFillColor(kGray);

    canHitMapsX[iLadder] = new TCanvas(Form("canHitMapsX_Ladder%d",cLadderID[iLadder]),"canHitMaps",1000,1000);
    canHitMapsX[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsX[iLadder]);
    canHitMapsX[iLadder]->SetFillColor(kGray);

    canHitMapsY[iLadder] = new TCanvas(Form("canHitMapsY_Ladder%d",cLadderID[iLadder]),"canHitMaps",1000,1000);
    canHitMapsY[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canHitMapsY[iLadder]);
    canHitMapsY[iLadder]->SetFillColor(kGray);

    canNumberOfHitsPerCluster[iLadder] = new TCanvas(Form("canNumberOfHitsPerCluster_Ladder%d",cLadderID[iLadder]),"canNumberOfHitsPerCluster",1000,1000);
    canNumberOfHitsPerCluster[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canNumberOfHitsPerCluster[iLadder]);
    canNumberOfHitsPerCluster[iLadder]->SetFillColor(kGray);

    canNumberOfClusterPerEvent[iLadder] = new TCanvas(Form("canNumberOfClusterPerEvent_Ladder%d",cLadderID[iLadder]),"canNumberOfClusterPerEvent",1000,1000);
    canNumberOfClusterPerEvent[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canNumberOfClusterPerEvent[iLadder]);
    canNumberOfClusterPerEvent[iLadder]->SetFillColor(kGray);

    canSpreadX[iLadder] = new TCanvas(Form("canSpreadX_Ladder%d",cLadderID[iLadder]),"canSpreadX",1000,1000);
    canSpreadX[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canSpreadX[iLadder]);
    canSpreadX[iLadder]->SetFillColor(kGray);

    canSpreadY[iLadder] = new TCanvas(Form("canSpreadY_Ladder%d",cLadderID[iLadder]),"canSpreadY",1000,1000);
    canSpreadY[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canSpreadY[iLadder]);
    canSpreadY[iLadder]->SetFillColor(kGray);

    canSpreadXVsNHitPerCluster[iLadder] = new TCanvas(Form("canSpreadXVsNHitPerCluster_Ladder%d",cLadderID[iLadder]),"canSpreadXVsNHitPerCluster",1000,1000);
    canSpreadXVsNHitPerCluster[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canSpreadXVsNHitPerCluster[iLadder]);
    canSpreadXVsNHitPerCluster[iLadder]->SetFillColor(kGray);

    canSpreadYVsNHitPerCluster[iLadder] = new TCanvas(Form("canSpreadYVsNHitPerCluster_Ladder%d",cLadderID[iLadder]),"canSpreadYVsNHitPerCluster",1000,1000);
    canSpreadYVsNHitPerCluster[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canSpreadYVsNHitPerCluster[iLadder]);
    canSpreadYVsNHitPerCluster[iLadder]->SetFillColor(kGray);

    canSpreadXY[iLadder] = new TCanvas(Form("canSpreadXY_Ladder%d",cLadderID[iLadder]),"canSpreadXY",1000,1000);
    canSpreadXY[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canSpreadXY[iLadder]);
    canSpreadXY[iLadder]->SetFillColor(kGray);

    canSpreadXYVsNHitPerCluster[iLadder] = new TCanvas(Form("canSpreadXYVsNHitPerCluster_Ladder%d",cLadderID[iLadder]),"canSpreadXYVsNHitPerCluster",1000,1000);
    canSpreadXYVsNHitPerCluster[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canSpreadXYVsNHitPerCluster[iLadder]);
    canSpreadXYVsNHitPerCluster[iLadder]->SetFillColor(kGray);

    canRelativeXDevOfCOM[iLadder] = new TCanvas(Form("canRelativeXDevOfCOM_Ladder%d",cLadderID[iLadder]),"canRelativeXDevOfCOM",1000,1000);
    canRelativeXDevOfCOM[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canRelativeXDevOfCOM[iLadder]);
    canRelativeXDevOfCOM[iLadder]->SetFillColor(kGray);

    canRelativeYDevOfCOM[iLadder] = new TCanvas(Form("canRelativeYDevOfCOM_Ladder%d",cLadderID[iLadder]),"canRelativeYDevOfCOM",1000,1000);
    canRelativeYDevOfCOM[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canRelativeYDevOfCOM[iLadder]);
    canRelativeYDevOfCOM[iLadder]->SetFillColor(kGray);

    canRelativePositionOfCOM[iLadder] = new TCanvas(Form("canRelativePositionOfCOM_Ladder%d",cLadderID[iLadder]),"canRelativePositionOfCOM",1000,1000);
    canRelativePositionOfCOM[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canRelativePositionOfCOM[iLadder]);
    canRelativePositionOfCOM[iLadder]->SetFillColor(kGray);

    canProbaClusterSize[iLadder] = new TCanvas(Form("canProbaClusterSize_Ladder%d",cLadderID[iLadder]),"canProbaClusterSize",1000,1000);
    canProbaClusterSize[iLadder]->Divide(1,numberOfChipsInLadder);
    SetCanvasStyle(canProbaClusterSize[iLadder]);
    canProbaClusterSize[iLadder]->SetFillColor(kGray);


    for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
      canHitMaps[iLadder]->cd(iChip+1);
      histoHitMap[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoHitMap[iLadder][iChip]->GetEntries()));
      histoHitMap[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoHitMap[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoHitMap[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoHitMap[iLadder][iChip]->GetXaxis()->SetTitle("Col");
      histoHitMap[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoHitMap[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoHitMap[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoHitMap[iLadder][iChip]->GetYaxis()->SetTitle("Row");
      histoHitMap[iLadder][iChip]->Draw("colz");

      if(cDrawLargeClusters){
        grClusters[iLadder][iChip]->Draw("5same");
        // histoHitMap[iLadder][iChip]->Draw("same");
      }

      canHitMapsCol[iLadder]->cd(iChip+1);
      histoHitMapCol[iLadder][iChip] =  histoHitMap[iLadder][iChip]->ProjectionX(Form("histoHitMapCol_Ladder%d_Chip%d",iLadder,cTopChipId-iChip));
      histoHitMapCol[iLadder][iChip]->Draw();

      canHitMapsRow[iLadder]->cd(iChip+1);
      histoHitMapRow[iLadder][iChip] =  histoHitMap[iLadder][iChip]->ProjectionY(Form("histoHitMapRow_Ladder%d_Chip%d",iLadder,cTopChipId-iChip));
      histoHitMapRow[iLadder][iChip]->Draw();

      canHitMapsXY[iLadder]->cd(iChip+1);
      histoHitMapXY[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoHitMapXY[iLadder][iChip]->GetEntries()));
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoHitMapXY[iLadder][iChip]->GetXaxis()->SetTitle("Y");
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetTitle("X");
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoHitMapXY[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoHitMapXY[iLadder][iChip]->Draw("colz");

      canHitMapsX[iLadder]->cd(iChip+1);
      histoHitMapX[iLadder][iChip] =  histoHitMapXY[iLadder][iChip]->ProjectionY(Form("histoHitMapX_Ladder%d_Chip%d",iLadder,cTopChipId-iChip));
      histoHitMapX[iLadder][iChip]->Draw();

      canHitMapsY[iLadder]->cd(iChip+1);
      histoHitMapY[iLadder][iChip] =  histoHitMapXY[iLadder][iChip]->ProjectionX(Form("histoHitMapY_Ladder%d_Chip%d",iLadder,cTopChipId-iChip));
      histoHitMapY[iLadder][iChip]->Draw();

      canNumberOfHitsPerCluster[iLadder]->cd(iChip+1);
      histoNumberOfHitPerCluster[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoNumberOfHitPerCluster[iLadder][iChip]->GetEntries()));
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitle("N_{pixels}/Cluster");
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoNumberOfHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
      histoNumberOfHitPerCluster[iLadder][iChip]->Draw("");

      canNumberOfClusterPerEvent[iLadder]->cd(iChip+1);
      histoNumberOfClusterPerEvent[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoNumberOfClusterPerEvent[iLadder][iChip]->GetEntries()));
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetXaxis()->SetTitle("N_{pixels}/Cluster");
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoNumberOfClusterPerEvent[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
      histoNumberOfClusterPerEvent[iLadder][iChip]->Draw("");

      canSpreadX[iLadder]->cd(iChip+1);
      histoClusterSpreadX[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoClusterSpreadX[iLadder][iChip]->GetEntries()));
      histoClusterSpreadX[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoClusterSpreadX[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoClusterSpreadX[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoClusterSpreadX[iLadder][iChip]->GetXaxis()->SetTitle("Cluster Spread X (# pix)");
      histoClusterSpreadX[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoClusterSpreadX[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoClusterSpreadX[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoClusterSpreadX[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
      histoClusterSpreadX[iLadder][iChip]->Draw("");

      canSpreadY[iLadder]->cd(iChip+1);
      histoClusterSpreadY[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoClusterSpreadY[iLadder][iChip]->GetEntries()));
      histoClusterSpreadY[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoClusterSpreadY[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoClusterSpreadY[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoClusterSpreadY[iLadder][iChip]->GetXaxis()->SetTitle("Cluster Spread Y (# pix)");
      histoClusterSpreadY[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoClusterSpreadY[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoClusterSpreadY[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoClusterSpreadY[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
      histoClusterSpreadY[iLadder][iChip]->Draw("");

      TF1 *identity0 = new TF1("identity0","x",0,cMaxNumberOfHitsPerCluster);
      TF1 *sqrt0 = new TF1("sqrt0","sqrt(x)",0,cMaxNumberOfHitsPerCluster);
      canSpreadXVsNHitPerCluster[iLadder]->cd(iChip+1);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetEntries()));
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitle("Spread X (# pix)");
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->Draw("colz");
      identity0->Draw("LSAME");
      sqrt0->Draw("LSAME");

      TF1 *identity1 = new TF1("identity1","x",0,cMaxNumberOfHitsPerCluster);
      TF1 *sqrt1 = new TF1("sqrt1","sqrt(x)",0,cMaxNumberOfHitsPerCluster);
      canSpreadYVsNHitPerCluster[iLadder]->cd(iChip+1);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetEntries()));
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitle("Spread Y (# pix)");
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->Draw("colz");
      identity1->Draw("LSAME");
      sqrt1->Draw("LSAME");

      TF1 *identity2 = new TF1("identity2","x",0,cMaxNumberOfHitsPerCluster);
      canSpreadXY[iLadder]->cd(iChip+1);
      histoSpreadXY[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoSpreadXY[iLadder][iChip]->GetEntries()));
      histoSpreadXY[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoSpreadXY[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoSpreadXY[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoSpreadXY[iLadder][iChip]->GetXaxis()->SetTitle("Spread X (# pix)");
      histoSpreadXY[iLadder][iChip]->GetYaxis()->SetTitle("Spread Y (# pix)");
      histoSpreadXY[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoSpreadXY[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoSpreadXY[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoSpreadXY[iLadder][iChip]->Draw("colz");
      identity2->Draw("LSAME");

      canSpreadXYVsNHitPerCluster[iLadder]->cd(iChip+1);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetEntries()));
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetXaxis()->SetTitle("Spread X (# pix)");
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitle("Spread Y (# pix)");
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetZaxis()->SetTitle("N_{pixel}/Cluster");
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetZaxis()->SetLabelSize(0.05);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetZaxis()->SetTitleSize(0.05);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->GetZaxis()->SetTitleOffset(1);
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->Draw("colz");

      canRelativeXDevOfCOM[iLadder]->cd(iChip+1);
      histoRelativeXDevOfCOM[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoRelativeXDevOfCOM[iLadder][iChip]->GetEntries()));
      histoRelativeXDevOfCOM[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoRelativeXDevOfCOM[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoRelativeXDevOfCOM[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoRelativeXDevOfCOM[iLadder][iChip]->GetXaxis()->SetTitle("X dev of COM (mm)");
      histoRelativeXDevOfCOM[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoRelativeXDevOfCOM[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoRelativeXDevOfCOM[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoRelativeXDevOfCOM[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
      histoRelativeXDevOfCOM[iLadder][iChip]->Draw("");

      canRelativeYDevOfCOM[iLadder]->cd(iChip+1);
      histoRelativeYDevOfCOM[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoRelativeYDevOfCOM[iLadder][iChip]->GetEntries()));
      histoRelativeYDevOfCOM[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoRelativeYDevOfCOM[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoRelativeYDevOfCOM[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoRelativeYDevOfCOM[iLadder][iChip]->GetXaxis()->SetTitle("Y dev of COM (mm)");
      histoRelativeYDevOfCOM[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoRelativeYDevOfCOM[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoRelativeYDevOfCOM[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoRelativeYDevOfCOM[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
      histoRelativeYDevOfCOM[iLadder][iChip]->Draw("");

      canRelativePositionOfCOM[iLadder]->cd(iChip+1);
      histoRelativePositionOfCOM[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoRelativePositionOfCOM[iLadder][iChip]->GetEntries()));
      histoRelativePositionOfCOM[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      histoRelativePositionOfCOM[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      histoRelativePositionOfCOM[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      histoRelativePositionOfCOM[iLadder][iChip]->GetXaxis()->SetTitle("Total dev of COM (mm)");
      histoRelativePositionOfCOM[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      histoRelativePositionOfCOM[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      histoRelativePositionOfCOM[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      histoRelativePositionOfCOM[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
      histoRelativePositionOfCOM[iLadder][iChip]->Draw("");

      TF1 *constant1 = new TF1("constant1","1",0,cMaxNumberOfHitsPerCluster);
      canProbaClusterSize[iLadder]->cd(iChip+1);
      probaClusterSize[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d",cLadderID[iLadder],cTopChipId-iChip));
      probaClusterSize[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
      probaClusterSize[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
      probaClusterSize[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
      probaClusterSize[iLadder][iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
      probaClusterSize[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
      probaClusterSize[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
      probaClusterSize[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
      probaClusterSize[iLadder][iChip]->GetYaxis()->SetTitle("Probability");
      probaClusterSize[iLadder][iChip]->SetMarkerColor(4);
      probaClusterSize[iLadder][iChip]->SetMarkerStyle(21);
      probaClusterSize[iLadder][iChip]->Draw("P");
      constant1->Draw("LSAME");

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

  TCanvas* canNumberOfClusterPerEvent_Telescope = new TCanvas(Form("canNumberOfClusterPerEvent_Telescope"),"canNumberOfClusterPerEvent_Telescope",1000,1000);
  canNumberOfClusterPerEvent_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canNumberOfClusterPerEvent_Telescope);
  canNumberOfClusterPerEvent_Telescope->SetFillColor(kYellow-10);

  TCanvas* canSpreadX_Telescope = new TCanvas(Form("canSpreadX_Telescope"),"canSpreadX_Telescope",1000,1000);
  canSpreadX_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canSpreadX_Telescope);
  canSpreadX_Telescope->SetFillColor(kYellow-10);

  TCanvas* canSpreadY_Telescope = new TCanvas(Form("canSpreadY_Telescope"),"canSpreadY_Telescope",1000,1000);
  canSpreadY_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canSpreadY_Telescope);
  canSpreadY_Telescope->SetFillColor(kYellow-10);

  TCanvas* canSpreadXVsNHitPerCluster_Telescope = new TCanvas(Form("canSpreadXVsNHitPerCluster_Telescope"),"canSpreadXVsNHitPerCluster_Telescope",1000,1000);
  canSpreadXVsNHitPerCluster_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canSpreadXVsNHitPerCluster_Telescope);
  canSpreadXVsNHitPerCluster_Telescope->SetFillColor(kYellow-10);

  TCanvas* canSpreadYVsNHitPerCluster_Telescope = new TCanvas(Form("canSpreadYVsNHitPerCluster_Telescope"),"canSpreadYVsNHitPerCluster_Telescope",1000,1000);
  canSpreadYVsNHitPerCluster_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canSpreadYVsNHitPerCluster_Telescope);
  canSpreadYVsNHitPerCluster_Telescope->SetFillColor(kYellow-10);

  TCanvas* canSpreadXY_Telescope = new TCanvas(Form("canSpreadXY_Telescope"),"canSpreadXY_Telescope",1000,1000);
  canSpreadXY_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canSpreadXY_Telescope);
  canSpreadXY_Telescope->SetFillColor(kYellow-10);

  TCanvas* canSpreadXYVsNHitPerCluster_Telescope = new TCanvas(Form("canSpreadXYVsNHitPerCluster_Telescope"),"canSpreadXYVsNHitPerCluster_Telescope",1000,1000);
  canSpreadXYVsNHitPerCluster_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canSpreadXYVsNHitPerCluster_Telescope);
  canSpreadXYVsNHitPerCluster_Telescope->SetFillColor(kYellow-10);

  TCanvas* canRelativeXDevOfCOM_Telescope = new TCanvas(Form("canRelativeXDevOfCOM_Telescope"),"canRelativeXDevOfCOM_Telescope",1000,1000);
  canRelativeXDevOfCOM_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canRelativeXDevOfCOM_Telescope);
  canRelativeXDevOfCOM_Telescope->SetFillColor(kYellow-10);

  TCanvas* canRelativeYDevOfCOM_Telescope = new TCanvas(Form("canRelativeYDevOfCOM_Telescope"),"canRelativeYDevOfCOM_Telescope",1000,1000);
  canRelativeYDevOfCOM_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canRelativeYDevOfCOM_Telescope);
  canRelativeYDevOfCOM_Telescope->SetFillColor(kYellow-10);

  TCanvas* canRelativePositionOfCOM_Telescope = new TCanvas(Form("canRelativePositionOfCOM_Telescope"),"canRelativePositionOfCOM_Telescope",1000,1000);
  canRelativePositionOfCOM_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canRelativePositionOfCOM_Telescope);
  canRelativePositionOfCOM_Telescope->SetFillColor(kYellow-10);

  TCanvas* canProbaClusterSize_Telescope = new TCanvas(Form("canProbaClusterSize_Telescope"),"canProbaClusterSize_Telescope",1000,1000);
  canProbaClusterSize_Telescope->Divide(1,cNumberOfChipsInTelescope);
  SetCanvasStyle(canProbaClusterSize_Telescope);
  canProbaClusterSize_Telescope->SetFillColor(kYellow-10);

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

    if(cDrawLargeClusters){
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
    histoNumberOfHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
    histoNumberOfHitPerCluster_Telescope[iChip]->Draw("");

    canNumberOfClusterPerEvent_Telescope->cd(iChip+1);
    histoNumberOfClusterPerEvent_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoNumberOfClusterPerEvent_Telescope[iChip]->GetEntries()));
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetXaxis()->SetTitle("N_{pixels}/Cluster");
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoNumberOfClusterPerEvent_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
    histoNumberOfClusterPerEvent_Telescope[iChip]->Draw("");

    canSpreadX_Telescope->cd(iChip+1);
    histoClusterSpreadX_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoClusterSpreadX_Telescope[iChip]->GetEntries()));
    histoClusterSpreadX_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoClusterSpreadX_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoClusterSpreadX_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoClusterSpreadX_Telescope[iChip]->GetXaxis()->SetTitle("Cluster Spread X (# pix)");
    histoClusterSpreadX_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoClusterSpreadX_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoClusterSpreadX_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoClusterSpreadX_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
    histoClusterSpreadX_Telescope[iChip]->Draw("");

    canSpreadY_Telescope->cd(iChip+1);
    histoClusterSpreadY_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoClusterSpreadY_Telescope[iChip]->GetEntries()));
    histoClusterSpreadY_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoClusterSpreadY_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoClusterSpreadY_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoClusterSpreadY_Telescope[iChip]->GetXaxis()->SetTitle("Cluster Spread Y (# pix)");
    histoClusterSpreadY_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoClusterSpreadY_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoClusterSpreadY_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoClusterSpreadY_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
    histoClusterSpreadY_Telescope[iChip]->Draw("");

    TF1 *identity3 = new TF1("identity3","x",0,cMaxNumberOfHitsPerCluster);
    TF1 *sqrt3 = new TF1("sqrt3","sqrt(x)",0,cMaxNumberOfHitsPerCluster);
    canSpreadXVsNHitPerCluster_Telescope->cd(iChip+1);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetEntries()));
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitle("Spread X (# pix)");
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->Draw("colz");
    identity3->Draw("LSAME");
    sqrt3->Draw("LSAME");

    TF1 *identity4 = new TF1("identity4","x",0,cMaxNumberOfHitsPerCluster);
    TF1 *sqrt4 = new TF1("sqrt4","sqrt(x)",0,cMaxNumberOfHitsPerCluster);
    canSpreadYVsNHitPerCluster_Telescope->cd(iChip+1);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetEntries()));
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitle("Spread Y (# pix)");
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->Draw("colz");
    identity4->Draw("LSAME");
    sqrt4->Draw("LSAME");

    TF1 *identity5 = new TF1("identity5","x",0,cMaxNumberOfHitsPerCluster);
    canSpreadXY_Telescope->cd(iChip+1);
    histoSpreadXY_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoSpreadXY_Telescope[iChip]->GetEntries()));
    histoSpreadXY_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoSpreadXY_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoSpreadXY_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoSpreadXY_Telescope[iChip]->GetXaxis()->SetTitle("Spread X (# pix)");
    histoSpreadXY_Telescope[iChip]->GetYaxis()->SetTitle("Spread Y (# pix)");
    histoSpreadXY_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoSpreadXY_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoSpreadXY_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoSpreadXY_Telescope[iChip]->Draw("colz");
    identity5->Draw("LSAME");

    canSpreadXYVsNHitPerCluster_Telescope->cd(iChip+1);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetEntries()));
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetXaxis()->SetTitle("Spread X (# pix)");
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitle("Spread Y (# pix)");
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetZaxis()->SetTitle("N_{pixel}/Cluster");
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetZaxis()->SetLabelSize(0.05);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetZaxis()->SetTitleSize(0.05);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->GetZaxis()->SetTitleOffset(1);
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->Draw("colz");

    canRelativeXDevOfCOM_Telescope->cd(iChip+1);
    histoRelativeXDevOfCOM_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoRelativeXDevOfCOM_Telescope[iChip]->GetEntries()));
    histoRelativeXDevOfCOM_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoRelativeXDevOfCOM_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoRelativeXDevOfCOM_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoRelativeXDevOfCOM_Telescope[iChip]->GetXaxis()->SetTitle("X dev of COM (mm)");
    histoRelativeXDevOfCOM_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoRelativeXDevOfCOM_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoRelativeXDevOfCOM_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoRelativeXDevOfCOM_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
    histoRelativeXDevOfCOM_Telescope[iChip]->Draw("");

    canRelativeYDevOfCOM_Telescope->cd(iChip+1);
    histoRelativeYDevOfCOM_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoRelativeYDevOfCOM_Telescope[iChip]->GetEntries()));
    histoRelativeYDevOfCOM_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoRelativeYDevOfCOM_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoRelativeYDevOfCOM_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoRelativeYDevOfCOM_Telescope[iChip]->GetXaxis()->SetTitle("Y dev of COM (mm)");
    histoRelativeYDevOfCOM_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoRelativeYDevOfCOM_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoRelativeYDevOfCOM_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoRelativeYDevOfCOM_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
    histoRelativeYDevOfCOM_Telescope[iChip]->Draw("");

    canRelativePositionOfCOM_Telescope->cd(iChip+1);
    histoRelativePositionOfCOM_Telescope[iChip]->SetTitle(Form("Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoRelativePositionOfCOM_Telescope[iChip]->GetEntries()));
    histoRelativePositionOfCOM_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    histoRelativePositionOfCOM_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    histoRelativePositionOfCOM_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    histoRelativePositionOfCOM_Telescope[iChip]->GetXaxis()->SetTitle("Total dev of COM (mm)");
    histoRelativePositionOfCOM_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    histoRelativePositionOfCOM_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    histoRelativePositionOfCOM_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    histoRelativePositionOfCOM_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
    histoRelativePositionOfCOM_Telescope[iChip]->Draw("");

    TF1 *constant2 = new TF1("constant2","1",0,cMaxNumberOfHitsPerCluster);
    canProbaClusterSize_Telescope->cd(iChip+1);
    probaClusterSize_Telescope[iChip]->SetTitle(Form("Chip %d",cTelescopeChipID[iChip]));
    probaClusterSize_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
    probaClusterSize_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
    probaClusterSize_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
    probaClusterSize_Telescope[iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
    probaClusterSize_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
    probaClusterSize_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
    probaClusterSize_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
    probaClusterSize_Telescope[iChip]->GetYaxis()->SetTitle("Probability");
    probaClusterSize_Telescope[iChip]->SetMarkerColor(4);
    probaClusterSize_Telescope[iChip]->SetMarkerStyle(21);
    probaClusterSize_Telescope[iChip]->Draw("P");
    constant2->Draw("LSAME");

  }

  //Create a TFile to write down the histograms
  TFile *outputHistosFile = new TFile(Form("%s/AnalysisResults.root",outputDirectory.Data()),"RECREATE");

  //An array of directories per ladder
  TDirectory *ladderDirectory[cNumberOfLadders];
  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    ladderDirectory[iLadder] = outputHistosFile->mkdir(Form("Ladder_%d",cLadderID[iLadder]));
    ladderDirectory[iLadder]->cd();
    for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
      histoHitMap[iLadder][iChip]->Write();
      histoHitMapXY[iLadder][iChip]->Write();
      histoNumberOfHitPerCluster[iLadder][iChip]->Write();
      histoNumberOfClusterPerEvent[iLadder][iChip]->Write();
      histoClusterSpreadX[iLadder][iChip]->Write();
      histoClusterSpreadY[iLadder][iChip]->Write();
      histoSpreadXVsNHitPerCluster[iLadder][iChip]->Write();
      histoSpreadYVsNHitPerCluster[iLadder][iChip]->Write();
      histoSpreadXY[iLadder][iChip]->Write();
      histoSpreadXYVsNHitPerCluster[iLadder][iChip]->Write();
      histoRelativeXDevOfCOM[iLadder][iChip]->Write();
      histoRelativeYDevOfCOM[iLadder][iChip]->Write();
      histoRelativePositionOfCOM[iLadder][iChip]->Write();
      probaClusterSize[iLadder][iChip]->Write();
    }
    canHitMaps[iLadder]->Write();
    canHitMapsXY[iLadder]->Write();
    canHitMapsCol[iLadder]->Write();
    canHitMapsRow[iLadder]->Write();
    canHitMapsX[iLadder]->Write();
    canHitMapsY[iLadder]->Write();
    canNumberOfHitsPerCluster[iLadder]->Write();
    canNumberOfClusterPerEvent[iLadder]->Write();
    canSpreadX[iLadder]->Write();
    canSpreadY[iLadder]->Write();
    canSpreadXVsNHitPerCluster[iLadder]->Write();
    canSpreadYVsNHitPerCluster[iLadder]->Write();
    canSpreadXY[iLadder]->Write();
    canSpreadXYVsNHitPerCluster[iLadder]->Write();
    canRelativeXDevOfCOM[iLadder]->Write();
    canRelativeYDevOfCOM[iLadder]->Write();
    canRelativePositionOfCOM[iLadder]->Write();
    canProbaClusterSize[iLadder]->Write();
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
    histoNumberOfClusterPerEvent_Telescope[iChip]->Write();
    histoClusterSpreadX_Telescope[iChip]->Write();
    histoClusterSpreadY_Telescope[iChip]->Write();
    histoSpreadXVsNHitPerCluster_Telescope[iChip]->Write();
    histoSpreadYVsNHitPerCluster_Telescope[iChip]->Write();
    histoSpreadXY_Telescope[iChip]->Write();
    histoSpreadXYVsNHitPerCluster_Telescope[iChip]->Write();
    histoRelativeXDevOfCOM_Telescope[iChip]->Write();
    histoRelativeYDevOfCOM_Telescope[iChip]->Write();
    histoRelativePositionOfCOM_Telescope[iChip]->Write();
    probaClusterSize_Telescope[iChip]->Write();
  }
  canHitMaps_Telescope->Write();
  canHitMapsXY_Telescope->Write();
  canHitMapsCol_Telescope->Write();
  canHitMapsRow_Telescope->Write();
  canHitMapsX_Telescope->Write();
  canHitMapsY_Telescope->Write();
  canNumberOfHitsPerCluster_Telescope->Write();
  canNumberOfClusterPerEvent_Telescope->Write();
  canSpreadX_Telescope->Write();
  canSpreadY_Telescope->Write();
  canSpreadXVsNHitPerCluster_Telescope->Write();
  canSpreadYVsNHitPerCluster_Telescope->Write();
  canSpreadXY_Telescope->Write();
  canSpreadXYVsNHitPerCluster_Telescope->Write();
  canRelativeXDevOfCOM_Telescope->Write();
  canRelativeYDevOfCOM_Telescope->Write();
  canRelativePositionOfCOM_Telescope->Write();
  canProbaClusterSize_Telescope->Write();

  outputHistosFile->Close();

}
