// Macro to check the distribution of cluster patterns

void ClusterPatternDistribution(TString outputDirectory = "TestRun", TString inputFileName = "Results_All.root",Int_t boxSize =2){

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

TH1I ***histoPatternDistribution = new TH1I**[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
  const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
  histoPatternDistribution[iLadder] = new TH1I*[numberOfChipsInLadder];
  for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){
    histoPatternDistribution[iLadder][iChip] = new TH1I(Form("histoPatternDistribution_Ladder%d_Chip%d",cLadderID[iLadder],cTopChipId-iChip),"",2*pow(2,boxSize*boxSize),0,2*pow(2,boxSize*boxSize));
    histoPatternDistribution[iLadder][iChip]->Sumw2();
  }
}

TH1I *histoPatternDistribution_Telescope[cNumberOfChipsInTelescope];

for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  histoPatternDistribution_Telescope[iChip] = new TH1I(Form("histoPatternDistribution_Chip%d",cTelescopeChipID[iChip]),"",2*pow(2,boxSize*boxSize),0,2*pow(2,boxSize*boxSize));
  histoPatternDistribution_Telescope[iChip]->Sumw2();
}

Int_t minCol, minRow, patternCode, numberOfPixels, pixCol, pixRow = 0;

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
        if( (cluster->GetColSpread() > boxSize) || (cluster->GetRowSpread() > boxSize) ) break;

        minCol = cluster->GetColMin();
        minRow = cluster->GetRowMin();

        numberOfPixels = cluster->GetNumberOfPixels();
        // printf("minCol = %d  minRow = %d  numberOfPixels = %d \n",minCol,minRow,numberOfPixels);
        for(int iPixel=0;iPixel<numberOfPixels;iPixel++){
          pixCol = cluster->GetPixel(iPixel)->GetCol();
          pixRow = cluster->GetPixel(iPixel)->GetRow();
          // printf("pixCol = %d  pixRow = %d\n",pixCol,pixRow);
          patternCode += 1 << ((pixRow-minRow)*boxSize + (pixCol-minCol));
        }
        histoPatternDistribution[iLadder][iChip]->Fill(patternCode);
        // printf("----- patternCode = %d\n",patternCode);
        if( (patternCode==2) || (patternCode==4) || (patternCode==8) || (patternCode==10) || (patternCode==12) ){
          printf("minCol = %d  minRow = %d  numberOfPixels = %d \n",minCol,minRow,numberOfPixels);
          patternCode = 0;
          for(int iPixel=0;iPixel<numberOfPixels;iPixel++){
            pixCol = cluster->GetPixel(iPixel)->GetCol();
            pixRow = cluster->GetPixel(iPixel)->GetRow();
            printf("pixCol = %d  pixRow = %d\n",pixCol,pixRow);
            patternCode += 1 << ((pixRow-minRow)*boxSize + (pixCol-minCol));
          }
          printf("EventID = %d\n",iEvent);
          printf("----- patternCode = %d\n",patternCode);
          // printf("Ladder = %d  Chip = %d  EventID = %d\n",iLadder,iChip,iEvent);
        }
        patternCode = 0;
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
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
      if( (cluster->GetColSpread() > boxSize) || (cluster->GetRowSpread() > boxSize) ) break;
      minCol = cluster->GetColMin();
      minRow = cluster->GetRowMin();

      numberOfPixels = cluster->GetNumberOfPixels();
      // printf("minCol = %d  minRow = %d  numberOfPixels = %d \n",minCol,minRow,numberOfPixels);
      for(int iPixel=0;iPixel<numberOfPixels;iPixel++){
        pixCol = cluster->GetPixel(iPixel)->GetCol();
        pixRow = cluster->GetPixel(iPixel)->GetRow();
        // printf("pixCol = %d  pixRow = %d\n",pixCol,pixRow);
        patternCode += 1 << ((pixRow-minRow)*boxSize + (pixCol-minCol));
      }
      histoPatternDistribution_Telescope[iChip]->Fill(patternCode);
      // printf("----- patternCode = %d\n",patternCode);
      if( (patternCode==2) || (patternCode==4) || (patternCode==8) || (patternCode==10) || (patternCode==12) ){
        printf("minCol = %d  minRow = %d  numberOfPixels = %d \n",minCol,minRow,numberOfPixels);
        patternCode = 0;
        for(int iPixel=0;iPixel<numberOfPixels;iPixel++){
          pixCol = cluster->GetPixel(iPixel)->GetCol();
          pixRow = cluster->GetPixel(iPixel)->GetRow();
          printf("pixCol = %d  pixRow = %d\n",pixCol,pixRow);
          patternCode += 1 << ((pixRow-minRow)*boxSize + (pixCol-minCol));
        }
        printf("EventID = %d\n",iEvent);
        printf("----- patternCode = %d\n",patternCode);
        // printf("Ladder = %d  Chip = %d  EventID = %d\n",iLadder,iChip,iEvent);
      }
      patternCode = 0;
    }
  }

}

TCanvas* canPatternDistribution[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){

  const Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];

  canPatternDistribution[iLadder] = new TCanvas(Form("canPatternDistribution_Ladder%d",cLadderID[iLadder]),"canPatternDistribution_Ladder",1000,1000);
  canPatternDistribution[iLadder]->Divide(1,numberOfChipsInLadder);
  SetCanvasStyle(canPatternDistribution[iLadder]);
  canPatternDistribution[iLadder]->SetFillColor(kGray);

  for(int iChip=0;iChip<numberOfChipsInLadder;iChip++){

    canPatternDistribution[iLadder]->cd(iChip+1);
    histoPatternDistribution[iLadder][iChip]->SetTitle(Form("Ladder %d, Chip %d, %1.1f entries",cLadderID[iLadder],cTopChipId-iChip,histoPatternDistribution[iLadder][iChip]->GetEntries()));
    histoPatternDistribution[iLadder][iChip]->GetXaxis()->SetLabelSize(0.05);
    histoPatternDistribution[iLadder][iChip]->GetXaxis()->SetTitleSize(0.05);
    histoPatternDistribution[iLadder][iChip]->GetXaxis()->SetTitleOffset(1);
    histoPatternDistribution[iLadder][iChip]->GetXaxis()->SetTitle("Pattern Code");
    histoPatternDistribution[iLadder][iChip]->GetYaxis()->SetLabelSize(0.05);
    histoPatternDistribution[iLadder][iChip]->GetYaxis()->SetTitleSize(0.05);
    histoPatternDistribution[iLadder][iChip]->GetYaxis()->SetTitleOffset(1);
    histoPatternDistribution[iLadder][iChip]->GetYaxis()->SetTitle("# Clusters");
    histoPatternDistribution[iLadder][iChip]->Draw("");
  }
}

TCanvas* canPatternDistribution_Telescope = new TCanvas("canPatternDistribution_Telescope","canPatternDistribution_Telescope",1000,1000);
canPatternDistribution_Telescope->Divide(1,cNumberOfChipsInTelescope);
SetCanvasStyle(canPatternDistribution_Telescope);
canPatternDistribution_Telescope->SetFillColor(kYellow-10);

for(int iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  canPatternDistribution_Telescope->cd(iChip+1);
  histoPatternDistribution_Telescope[iChip]->SetTitle(Form("Telescope, Chip %d, %1.1f entries",cTelescopeChipID[iChip],histoPatternDistribution_Telescope[iChip]->GetEntries()));
  histoPatternDistribution_Telescope[iChip]->GetXaxis()->SetLabelSize(0.05);
  histoPatternDistribution_Telescope[iChip]->GetXaxis()->SetTitleSize(0.05);
  histoPatternDistribution_Telescope[iChip]->GetXaxis()->SetTitleOffset(1);
  histoPatternDistribution_Telescope[iChip]->GetXaxis()->SetTitle("Pattern Code");
  histoPatternDistribution_Telescope[iChip]->GetYaxis()->SetLabelSize(0.05);
  histoPatternDistribution_Telescope[iChip]->GetYaxis()->SetTitleSize(0.05);
  histoPatternDistribution_Telescope[iChip]->GetYaxis()->SetTitleOffset(1);
  histoPatternDistribution_Telescope[iChip]->GetYaxis()->SetTitle("# Clusters");
  histoPatternDistribution_Telescope[iChip]->Draw("COL");
}

TFile *outputHistosFile = new TFile(Form("%s/ClusterPatternDistribution.root",outputDirectory.Data()),"RECREATE");
TDirectory *ladderDirectory[cNumberOfLadders];

for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
  ladderDirectory[iLadder] = outputHistosFile->mkdir(Form("Ladder_%d",cLadderID[iLadder]));
  ladderDirectory[iLadder]->cd();

  for(Int_t iChip=0;iChip<cNumberOfChipsInLadder[iLadder];iChip++){
    histoPatternDistribution[iLadder][iChip]->Write();
  }
  canPatternDistribution[iLadder]->Write();
}

TDirectory *telescopeDirectory = outputHistosFile->mkdir("Telescope");
telescopeDirectory->cd();
for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
  histoPatternDistribution_Telescope[iChip]->Write();
}
canPatternDistribution_Telescope->Write();

outputHistosFile->Close();

}
