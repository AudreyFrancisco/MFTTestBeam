// Macro to plot Hit maps of all devices (Telescope + 3 Ladders) for a given event.

void eveClusters(TString outputDirectory = "TestRun", TString inputFileName = "TestBeamEventTree_WithTelescopeTracks.root",Int_t eventID =0){

gStyle->SetPalette(0);

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
  cLadderID[iLadder] = cLadderID_All[cParticipatedLadders[iLadder]];
  cNumberOfChipsInLadder[iLadder] = cNumberOfChipsInLadder_All[cParticipatedLadders[iLadder]];
  cXPositionOfFirstChipInLadder[iLadder] = cXPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
  cYPositionOfFirstChipInLadder[iLadder] = cYPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
  cZPositionOfFirstChipInLadder[iLadder] = cZPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
}

TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
TTree *testBeamEventTree = ((TTree*) inputFile->Get("TestBeamEventTree"));
TestBeamEvent* event = new TestBeamEvent();
testBeamEventTree->SetBranchAddress("event", &event);
testBeamEventTree->GetEntry(eventID);

TH2I ***histoHitMapBigClusters = new TH2I**[cNumberOfLadders];
TH2I ***histoHitMapBigClustersXY = new TH2I**[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
  const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
  histoHitMapBigClusters[iLadder] = new TH2I*[numberOfChipsInLadder];
  histoHitMapBigClustersXY[iLadder] = new TH2I*[numberOfChipsInLadder];
  for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
    histoHitMapBigClusters[iLadder][iChip] = new TH2I(Form("histoHitMapBigClusters_Event%d_Ladder%d_Chip%d",eventID,cLadderID[iLadder],cTopChipId-iChip),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
    histoHitMapBigClusters[iLadder][iChip]->Sumw2();

    histoHitMapBigClustersXY[iLadder][iChip] = new TH2I(Form("histoHitMapBigClustersXY_Event%d_Ladder%d_Chip%d",eventID,cLadderID[iLadder],cTopChipId-iChip),"",cNumberOfRowsInChip,cYPositionOfFirstChipInLadder[iLadder]-0.5*cChipYSize,cYPositionOfFirstChipInLadder[iLadder]+0.5*cChipYSize,cNumberOfColumnsInChip,-1*iChip*cChipXSize+cXPositionOfFirstChipInLadder[iLadder]-0.5*cChipXSize,-1*iChip*cChipXSize+cXPositionOfFirstChipInLadder[iLadder]+0.5*cChipXSize);
    histoHitMapBigClustersXY[iLadder][iChip]->Sumw2();
  }
}

TH2I *histoHitMapBigClusters_Telescope[cNumberOfChipsInTelescope];
TH2I *histoHitMapBigClustersXY_Telescope[cNumberOfChipsInTelescope];

for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  histoHitMapBigClusters_Telescope[iChip] = new TH2I(Form("histoHitMapBigClusters_Event%d_Telescope_Chip%d",eventID,cTelescopeChipID[iChip]),"",cNumberOfColumnsInChip,0,cNumberOfColumnsInChip,cNumberOfRowsInChip,0,cNumberOfRowsInChip);
  histoHitMapBigClusters_Telescope[iChip]->Sumw2();

  histoHitMapBigClustersXY_Telescope[iChip] = new TH2I(Form("histoHitMapBigClustersXY_Event%d_Telescope_Chip%d",eventID,cTelescopeChipID[iChip]),"",cNumberOfRowsInChip,cYPositionOfChipsInTelescope[iChip]-0.5*cChipYSize,cYPositionOfChipsInTelescope[iChip]+0.5*cChipYSize,cNumberOfColumnsInChip,cXPositionOfChipsInTelescope[iChip]-0.5*cChipXSize,cXPositionOfChipsInTelescope[iChip]+0.5*cChipXSize);
  histoHitMapBigClustersXY_Telescope[iChip]->Sumw2();
}

// Fill ladder hit maps
for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){

  AlpideLadder *ladder = event->GetLadder(iLadder);
  Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];

  for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){

    AlpideChip *chip = ladder->GetChip(iChip);
    //Fill the hit histograms
    Int_t numberOfHitPixels = chip->GetNumberOfHitPixels();
    for(int iHitPixel=0;iHitPixel<numberOfHitPixels;iHitPixel++){
      AlpidePixel *pixel = chip->GetHitPixel(iHitPixel);
      histoHitMapBigClusters[iLadder][iChip]->Fill(pixel->GetCol(),pixel->GetRow());
      histoHitMapBigClustersXY[iLadder][iChip]->Fill(pixel->GetYCoordinate(),pixel->GetXCoordinate());
    }
  }
}

// Fill Telescope hit maps
AlpideTelescope *telescope = event->GetTelescope();
for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){

  AlpideChip *chip = telescope->GetChip(iChip);
  //Fill the hit histograms
  Int_t numberOfHitPixels = chip->GetNumberOfHitPixels();
  for(int iHitPixel=0;iHitPixel<numberOfHitPixels;iHitPixel++){
    AlpidePixel *pixel = chip->GetHitPixel(iHitPixel);
    histoHitMapBigClusters_Telescope[iChip]->Fill(pixel->GetCol(),pixel->GetRow());
    histoHitMapBigClustersXY_Telescope[iChip]->Fill(pixel->GetYCoordinate(),pixel->GetXCoordinate());
  }
}

TCanvas* canHitMapBigClusters[cNumberOfLadders];
TCanvas* canHitMapBigClustersXY[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){

  const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];

  canHitMapBigClusters[iLadder] = new TCanvas(Form("canHitMapBigClusters_Event%d_Ladder%d",eventID,cLadderID[iLadder]),"canHitMapBigClusters_Ladder",1000,1000);
  canHitMapBigClusters[iLadder]->Divide(1,numberOfChipsInLadder);
  SetCanvasStyle(canHitMapBigClusters[iLadder]);
  canHitMapBigClusters[iLadder]->SetFillColor(kGray);

  canHitMapBigClustersXY[iLadder] = new TCanvas(Form("canHitMapBigClustersXY_Event%d_Ladder%d",eventID,cLadderID[iLadder]),"canHitMapBigClustersXY_Ladder",1000,1000);
  canHitMapBigClustersXY[iLadder]->Divide(1,numberOfChipsInLadder);
  SetCanvasStyle(canHitMapBigClustersXY[iLadder]);
  canHitMapBigClustersXY[iLadder]->SetFillColor(kGray);

  for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){

    canHitMapBigClusters[iLadder]->cd(iChip+1);
    histoHitMapBigClusters[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, Event %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,eventID,histoHitMapBigClusters[iLadder][iChip]->GetEntries()));
    histoHitMapBigClusters[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
    histoHitMapBigClusters[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
    histoHitMapBigClusters[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
    histoHitMapBigClusters[iLadder][iChip]->GetXaxis()->SetTitle("Col");
    histoHitMapBigClusters[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
    histoHitMapBigClusters[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
    histoHitMapBigClusters[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
    histoHitMapBigClusters[iLadder][iChip]->GetYaxis()->SetTitle("Row");
    histoHitMapBigClusters[iLadder][iChip]->Draw("COL");

    canHitMapBigClustersXY[iLadder]->cd(iChip+1);
    histoHitMapBigClustersXY[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, Event %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,eventID,histoHitMapBigClustersXY[iLadder][iChip]->GetEntries()));

    histoHitMapBigClustersXY[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
    histoHitMapBigClustersXY[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
    histoHitMapBigClustersXY[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
    histoHitMapBigClustersXY[iLadder][iChip]->GetXaxis()->SetTitle("X (mm)");
    histoHitMapBigClustersXY[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
    histoHitMapBigClustersXY[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
    histoHitMapBigClustersXY[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
    histoHitMapBigClustersXY[iLadder][iChip]->GetYaxis()->SetTitle("Y (mm)");
    histoHitMapBigClustersXY[iLadder][iChip]->Draw("COL");
  }
}

TCanvas* canHitMapBigClusters_Telescope = new TCanvas(Form("canHitMapBigClusters_Telescope_Event%d",eventID),"canHitMapBigClusters_Telescope",1000,1000);
canHitMapBigClusters_Telescope->Divide(1,cNumberOfChipsInTelescope);
SetCanvasStyle(canHitMapBigClusters_Telescope);
canHitMapBigClusters_Telescope->SetFillColor(kYellow-10);

TCanvas* canHitMapBigClustersXY_Telescope = new TCanvas(Form("canHitMapBigClustersXY_Telescope_Event%d",eventID),"canHitMapBigClustersXY_Telescope",1000,1000);
canHitMapBigClustersXY_Telescope->Divide(1,cNumberOfChipsInTelescope);
SetCanvasStyle(canHitMapBigClustersXY_Telescope);
canHitMapBigClustersXY_Telescope->SetFillColor(kYellow-5);

for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  canHitMapBigClusters_Telescope->cd(iChip+1);
  histoHitMapBigClusters_Telescope[iChip]->SetTitle(Form("Telescope, Chip %d, Event %d, %1.1f entries",cTelescopeChipID[iChip],eventID,histoHitMapBigClusters_Telescope[iChip]->GetEntries()));
  histoHitMapBigClusters_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
  histoHitMapBigClusters_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
  histoHitMapBigClusters_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
  histoHitMapBigClusters_Telescope[iChip]->GetXaxis()->SetTitle("Col");
  histoHitMapBigClusters_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
  histoHitMapBigClusters_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
  histoHitMapBigClusters_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
  histoHitMapBigClusters_Telescope[iChip]->GetYaxis()->SetTitle("Row");
  histoHitMapBigClusters_Telescope[iChip]->Draw("COL");

  canHitMapBigClustersXY_Telescope->cd(iChip+1);
  histoHitMapBigClustersXY_Telescope[iChip]->SetTitle(Form("Telescope, Chip %d, Event %d, %1.1f entries",cTelescopeChipID[iChip],eventID,histoHitMapBigClustersXY_Telescope[iChip]->GetEntries()));
  histoHitMapBigClustersXY_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
  histoHitMapBigClustersXY_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
  histoHitMapBigClustersXY_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
  histoHitMapBigClustersXY_Telescope[iChip]->GetXaxis()->SetTitle("X (mm)");
  histoHitMapBigClustersXY_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
  histoHitMapBigClustersXY_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
  histoHitMapBigClustersXY_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
  histoHitMapBigClustersXY_Telescope[iChip]->GetYaxis()->SetTitle("Y (mm)");
  histoHitMapBigClustersXY_Telescope[iChip]->Draw("COL");
}

TFile *outputHistosFile = new TFile(Form("%s/BigClusters.root",outputDirectory.Data()),"RECREATE");
TDirectory *ladderDirectory[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
  ladderDirectory[iLadder] = outputHistosFile->mkdir(Form("Ladder_%d",cLadderID[iLadder]));
  ladderDirectory[iLadder]->cd();

  for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
    histoHitMapBigClusters[iLadder][iChip]->Write();
    histoHitMapBigClustersXY[iLadder][iChip]->Write();
  }
  canHitMapBigClusters[iLadder]->Write();
  canHitMapBigClustersXY[iLadder]->Write();
}

TDirectory *telescopeDirectory = outputHistosFile->mkdir("Telescope");
telescopeDirectory->cd();
for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  histoHitMapBigClusters_Telescope[iChip]->Write();
  histoHitMapBigClustersXY_Telescope[iChip]->Write();
}
canHitMapBigClusters_Telescope->Write();
canHitMapBigClustersXY_Telescope->Write();

outputHistosFile->Close();

}
