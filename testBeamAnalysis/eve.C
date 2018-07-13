//Please take care to the fact that the w,y directions chosen for the chips are inverted with respect to the ones on the geometry

void eve(TString outputDirectory = "TestRun", TString inputFileName = "TestBeamEventTree_WithTelescopeTracks.root",Int_t eventID =0)
{
  //Create the view
  TEveManager::Create();

  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    cLadderID[iLadder] = cLadderID_All[cParticipatedLadders[iLadder]];
    cNumberOfChipsInLadder[iLadder] = cNumberOfChipsInLadder_All[cParticipatedLadders[iLadder]];
    cXPositionOfFirstChipInLadder[iLadder] = cXPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cYPositionOfFirstChipInLadder[iLadder] = cYPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
    cZPositionOfFirstChipInLadder[iLadder] = cZPositionOfFirstChipInLadder_All[cParticipatedLadders[iLadder]];
  }



  //Draw the three x,y,z axes
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  TEveArrow* xAxisArrow = new TEveArrow(0, 30, 0., 0., 0., cZPositionOfChipsInTelescope[0]-10);
  xAxisArrow->SetTubeR(0.005);
  xAxisArrow->SetConeR(0.020);
  xAxisArrow->SetConeL(0.060);
  xAxisArrow->SetMainColor(kBlue);
  xAxisArrow->SetPickable(kTRUE);
  gEve->AddElement(xAxisArrow);
  TEveText* textXAxis = new TEveText("  x");
  textXAxis->SetFontSize(20);
  TEveVector xAxisArrowVector = xAxisArrow->GetVector()+xAxisArrow->GetOrigin();
  textXAxis->RefMainTrans().SetPos(xAxisArrowVector.Arr());
  xAxisArrow->AddElement(textXAxis);



  TEveArrow* yAxisArrow = new TEveArrow(30, 0, 0., 0., 0., cZPositionOfChipsInTelescope[0]-10);
  yAxisArrow->SetTubeR(0.005);
  yAxisArrow->SetConeR(0.020);
  yAxisArrow->SetConeL(0.060);
  yAxisArrow->SetMainColor(kBlue);
  yAxisArrow->SetPickable(kTRUE);
  gEve->AddElement(yAxisArrow);
  TEveText* textYAxis = new TEveText("  y");
  textYAxis->SetFontSize(20);
  TEveVector yAxisArrowVector = yAxisArrow->GetVector()+yAxisArrow->GetOrigin();
  textYAxis->RefMainTrans().SetPos(yAxisArrowVector.Arr());
  yAxisArrow->AddElement(textYAxis);

  TEveArrow* zAxisArrow = new TEveArrow(0, 0, 170., 0., 0., cZPositionOfChipsInTelescope[0]-10);
  zAxisArrow->SetTubeR(0.001);
  zAxisArrow->SetConeR(0.006);
  zAxisArrow->SetConeL(0.018);
  zAxisArrow->SetMainColor(kBlue);
  zAxisArrow->SetPickable(kTRUE);
  gEve->AddElement(zAxisArrow);
  TEveText* textZAxis = new TEveText("  z");
  textZAxis->SetFontSize(20);
  TEveVector zAxisArrowVector = zAxisArrow->GetVector()+zAxisArrow->GetOrigin();
  textZAxis->RefMainTrans().SetPos(zAxisArrowVector.Arr());
  zAxisArrow->AddElement(textZAxis);

  //Add a circular marker for the origin
  TEvePointSet* originPoint = new TEvePointSet(1);
  originPoint->SetMarkerColor(kBlue);
  originPoint->SetMarkerStyle(kFullCircle);
  originPoint->SetMarkerSize(2);
  originPoint->SetPoint(0, -0.2,0,0);
  gEve->AddElement(originPoint);

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  //create the telescope chips
  for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    //A chip is a box assuming that the height along z is null
    TEveBox* chipBox = new TEveBox;
    chipBox->SetMainColor(kYellow);
    chipBox->SetMainTransparency(0);

    //TODO: Gentler
    chipBox->SetVertex(0, cYPositionOfChipsInTelescope[iChip]+0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]+0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    chipBox->SetVertex(1, cYPositionOfChipsInTelescope[iChip]-0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]+0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    chipBox->SetVertex(2, cYPositionOfChipsInTelescope[iChip]-0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]-0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    chipBox->SetVertex(3, cYPositionOfChipsInTelescope[iChip]+0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]-0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    chipBox->SetVertex(4, cYPositionOfChipsInTelescope[iChip]+0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]+0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    chipBox->SetVertex(5, cYPositionOfChipsInTelescope[iChip]-0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]+0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    chipBox->SetVertex(6, cYPositionOfChipsInTelescope[iChip]-0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]-0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    chipBox->SetVertex(7, cYPositionOfChipsInTelescope[iChip]+0.5*cChipYSize,cXPositionOfChipsInTelescope[iChip]-0.5*cChipXSize,cZPositionOfChipsInTelescope[iChip]);
    gEve->AddElement(chipBox);


  }

  //similarly draw the chips of the ladders
  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    Int_t numberOfChipsInLadder = cNumberOfChipsInLadder[iLadder];
    for(Int_t iChip=0;iChip<numberOfChipsInLadder;iChip++){
      TEveBox* chipBox = new TEveBox;
      chipBox->SetMainColor(kGreen);
      chipBox->SetMainTransparency(0);

      //TODO: Gentler
      chipBox->SetVertex(0, cYPositionOfFirstChipInLadder[iLadder]+0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]+0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);
      chipBox->SetVertex(1, cYPositionOfFirstChipInLadder[iLadder]-0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]+0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);
      chipBox->SetVertex(2, cYPositionOfFirstChipInLadder[iLadder]-0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]-0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);
      chipBox->SetVertex(3, cYPositionOfFirstChipInLadder[iLadder]+0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]-0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);
      chipBox->SetVertex(4, cYPositionOfFirstChipInLadder[iLadder]+0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]+0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);
      chipBox->SetVertex(5, cYPositionOfFirstChipInLadder[iLadder]-0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]+0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);
      chipBox->SetVertex(6, cYPositionOfFirstChipInLadder[iLadder]-0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]-0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);
      chipBox->SetVertex(7, cYPositionOfFirstChipInLadder[iLadder]+0.5*cChipYSize,cXPositionOfFirstChipInLadder[iLadder]-0.5*cChipXSize-iChip*cChipXSize,cZPositionOfFirstChipInLadder[iLadder]);

      gEve->AddElement(chipBox);
    }
  }


  //Open the tree containing the events, get the clusters and draw them.
  //The output file of AddTelescopeTracks.C
  TFile *inputFile = new TFile(Form("%s/%s",outputDirectory.Data(),inputFileName.Data()));
  TTree *testBeamEventTree_TracksAndClusters = ((TTree*) inputFile->Get("TestBeamEventTree"));
  TestBeamEvent* event = new TestBeamEvent();
  testBeamEventTree_TracksAndClusters->SetBranchAddress("event", &event);
  testBeamEventTree_TracksAndClusters->GetEntry(eventID);


  AlpideTelescope *telescope = event->GetTelescope();
  for(Int_t iChip=0;iChip<cNumberOfChipsInTelescope;iChip++){
    AlpideChip *chip = telescope->GetChip(iChip);

    //Get the clusters in each chip
    Int_t numberOfClusterInChip = chip->GetNumberOfClusters();
    if(numberOfClusterInChip==0) continue;
    TEvePointSet* clusterSet = new TEvePointSet(numberOfClusterInChip);
    clusterSet->SetMarkerColor(kWhite);
    clusterSet->SetMarkerStyle(3);
    clusterSet->SetMarkerSize(0.4);
    for(Int_t iCluster=0;iCluster<numberOfClusterInChip;iCluster++){
      AlpideCluster *cluster = chip->GetCluster(iCluster);
      cout<<"cluster"<<endl;
      //Draw the cluster after getting the x,y,z coordinates
      clusterSet->SetPoint(iCluster, cluster->GetCenterOfMassY(),cluster->GetCenterOfMassX(), cZPositionOfChipsInTelescope[iChip]);
    }
    gEve->AddElement(clusterSet);
  }


  // Get the two ladders, their chips and then the clusters
  for(Int_t iLadder=0;iLadder<cNumberOfLadders;iLadder++){
    AlpideLadder *ladder = event->GetLadder(iLadder);
    Int_t numberOfChipsInLadder = ladder->GetNumberOfChips();
    for(Int_t iChip=0;iChip<numberOfChipsInLadder;iChip++){
      AlpideChip *chip = ladder->GetChip(iChip);

      // //Get the clusters in each chip
      Int_t numberOfClusterInChip = chip->GetNumberOfClusters();
      if(numberOfClusterInChip==0) continue;
      TEvePointSet* clusterSet = new TEvePointSet(numberOfClusterInChip);
      // clusterSet->SetName("Cluster Set per chip");
      clusterSet->SetMarkerColor(kWhite);
      clusterSet->SetMarkerStyle(3);
      clusterSet->SetMarkerSize(0.4);
      for(Int_t iCluster=0;iCluster<numberOfClusterInChip;iCluster++){
        AlpideCluster *cluster = chip->GetCluster(iCluster);
        //Draw the cluster after getting the x,y,z coordinates
        clusterSet->SetPoint(iCluster,  cluster->GetCenterOfMassY(),cluster->GetCenterOfMassX(), cZPositionOfFirstChipInLadder[iLadder]);
      }
      gEve->AddElement(clusterSet);
    }
  }

  //Draw a line



  //Get the telescope tracks and draw
  Int_t numberOfTracks = telescope->GetNumberOfTelescopeTracks();
  if(numberOfTracks != 0){
    TEveStraightLineSet* lineSet = new TEveStraightLineSet();
    lineSet->SetLineColor(kWhite);
    for(Int_t iTrack=0;iTrack<numberOfTracks;iTrack++){
      //Get the track
      TelescopeTrack *track = telescope->GetTelescopeTrack(iTrack);
      //Get two points corresponding to the first telescope chip and the last ladder
      Float_t x1, y1, z1;
      Float_t x2, y2, z2;
      z1 = 0.9*cZPositionOfChipsInTelescope[0];
      z2 = 1.05*cZPositionOfFirstChipInLadder[cNumberOfLadders-1];
      track->GetXYCoordinate(x1,y1,z1);
      track->GetXYCoordinate(x2,y2,z2);
      lineSet->AddLine(y1,x1,z1,y2,x2,z2);

    }
    gEve->AddElement(lineSet);
  }


  gEve->Redraw3D(kTRUE);
  // gEve->FullRedraw3D(kTRUE);
}
