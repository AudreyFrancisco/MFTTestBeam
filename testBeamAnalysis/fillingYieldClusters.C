// Macro to generate histograms of filling Yield

void fillingYieldClusters(TString outputDirectory = "TestRun", TString inputFileName = "Results_All.root"){

// gStyle->SetPalette(0);

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
Long_t numberOfEntries = testBeamEventTree->GetEntries();

TH2I ***histoFillingYield = new TH2I**[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
  const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
  histoFillingYield[iLadder] = new TH2I*[numberOfChipsInLadder];
  for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
    histoFillingYield[iLadder][iChip] = new TH2I(Form("histoFillingYield_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,1);
    histoFillingYield[iLadder][iChip]->Sumw2();
  }
}

TH2I *histoFillingYield_Telescope[cNumberOfChipsInTelescope];

for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  histoFillingYield_Telescope[iChip] = new TH2I(Form("histoFillingYield_Chip%d",cTelescopeChipID[iChip]),"",cMaxNumberOfHitsPerCluster,0,cMaxNumberOfHitsPerCluster,cMaxNumberOfHitsPerCluster,0,1);
  histoFillingYield_Telescope[iChip]->Sumw2();
}

Int_t nHitsInCluster, spreadX, spreadY, max = 0;

for(int iEvent=0;iEvent<numberOfEntries;iEvent++){
  printf("Filling histograms  ... %.0f%%%s", 100.*iEvent/numberOfEntries, (iEvent < numberOfEntries) ? "\r" : "\n");

  testBeamEventTree->GetEntry(iEvent);
  // Fill ladder hit maps
  for(int iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    AlpideLadder *ladder = event->GetLadder(iLadder);
    Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
    for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
      AlpideChip *chip = ladder->GetChip(iChip);
      Int_t numberOfClusters = chip->GetNumberOfClusters();
      for(int iCluster=0;iCluster<numberOfClusters;iCluster++){
        AlpideCluster *cluster = chip->GetCluster(iCluster);
        nHitsInCluster = cluster->GetNumberOfPixels();
        spreadX = cluster->GetRowSpread();
        spreadY = cluster->GetColSpread();
        if(spreadX >= spreadY){max = spreadX;}
        else{max = spreadY;}
        histoFillingYield[iLadder][iChip]->Fill(nHitsInCluster,nHitsInCluster/pow(max,2));
      }
    }
  }
  // Fill Telescope hit maps
  AlpideTelescope *telescope = event->GetTelescope();
  for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    AlpideChip *chip = telescope->GetChip(iChip);
    Int_t numberOfClusters = chip->GetNumberOfClusters();
    for(int iCluster=0;iCluster<numberOfClusters;iCluster++){
      AlpideCluster *cluster = chip->GetCluster(iCluster);
      nHitsInCluster = cluster->GetNumberOfPixels();
      spreadX = cluster->GetRowSpread();
      spreadY = cluster->GetColSpread();
      if(spreadX >= spreadY){max = spreadX;}
      else{max = spreadY;}
      histoFillingYield_Telescope[iChip]->Fill(nHitsInCluster,nHitsInCluster/pow(max,2));
    }
  }

}

TCanvas* canFillingYield[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){

  const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];

  canFillingYield[iLadder] = new TCanvas(Form("canFillingYield_Ladder%d",cLadderID[iLadder]),"canFillingYield_Ladder",1000,1000);
  canFillingYield[iLadder]->Divide(1,numberOfChipsInLadder);
  SetCanvasStyle(canFillingYield[iLadder]);
  canFillingYield[iLadder]->SetFillColor(kGray);

  for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){

    canFillingYield[iLadder]->cd(iChip+1);
    histoFillingYield[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoFillingYield[iLadder][iChip]->GetEntries()));
    histoFillingYield[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
    histoFillingYield[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
    histoFillingYield[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
    histoFillingYield[iLadder][iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
    histoFillingYield[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
    histoFillingYield[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
    histoFillingYield[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
    histoFillingYield[iLadder][iChip]->GetYaxis()->SetTitle("Yield");
    histoFillingYield[iLadder][iChip]->Draw("colz");
  }
}

TCanvas* canFillingYield_Telescope = new TCanvas("canFillingYield_Telescope","canFillingYield_Telescope",1000,1000);
canFillingYield_Telescope->Divide(1,cNumberOfChipsInTelescope);
SetCanvasStyle(canFillingYield_Telescope);
canFillingYield_Telescope->SetFillColor(kYellow-10);

for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  canFillingYield_Telescope->cd(iChip+1);
  histoFillingYield_Telescope[iChip]->SetTitle(Form("Telescope, Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoFillingYield_Telescope[iChip]->GetEntries()));
  histoFillingYield_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
  histoFillingYield_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
  histoFillingYield_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
  histoFillingYield_Telescope[iChip]->GetXaxis()->SetTitle("N_{pixel}/Cluster");
  histoFillingYield_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
  histoFillingYield_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
  histoFillingYield_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
  histoFillingYield_Telescope[iChip]->GetYaxis()->SetTitle("Yield");
  histoFillingYield_Telescope[iChip]->Draw("colz");
}

TFile *outputHistosFile = new TFile(Form("%s/FillingYield.root",outputDirectory.Data()),"RECREATE");
TDirectory *ladderDirectory[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
  ladderDirectory[iLadder] = outputHistosFile->mkdir(Form("Ladder_%d",cLadderID[iLadder]));
  ladderDirectory[iLadder]->cd();

  for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
    histoFillingYield[iLadder][iChip]->Write();
  }
  canFillingYield[iLadder]->Write();
}

TDirectory *telescopeDirectory = outputHistosFile->mkdir("Telescope");
telescopeDirectory->cd();
for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  histoFillingYield_Telescope[iChip]->Write();
}
canFillingYield_Telescope->Write();

outputHistosFile->Close();

}
