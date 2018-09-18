#!/bin/bash
if [ "$#" -lt 1 ]
then
  echo "Two arguments are require"
  echo "Usage: `basename $0` -runNameName"
  exit 1
fi

optList="rcdafFegpot"
readRawTree=0
drawHitMaps=0
fitTelescopeTracks=0
makeClusters=0
calculateLadderEffi=0
alignTelescope=0
getParticipatedChips=0
generateEvents=0;
getChipPositions=0;
testMacro=0;
fitGlobalTracks=0;
EXIT=0
while getopts $optList option
do
  case $option in
    r ) readRawTree=1;;
    d ) drawHitMaps=1;;
    c ) makeClusters=1;;
    f ) fitTelescopeTracks=1;;
    F ) fitGlobalTracks=1;;
    e ) calculateLadderEffi=1;;
    a ) alignTelescope=1;;
    p ) getParticipatedChips=1;;
    g ) generateEvents=1;;
    o ) getChipPositions=1;;
    t ) testMacro=1;;
    * ) echo "Unimplemented option chosen."
    EXIT=1
    ;;
  esac
done

shift $(($OPTIND - 1))

if [ $EXIT -eq 1 ]; then
  echo "Usage: `basename $0` (-$optList)"
  echo "       -r  To read the raw trees after merging them. Do not use this option if the tree is already read"
  echo "       -d  Fill the hitmaps and display them"
  echo "       -c  Make the clusters"
  echo "       -f  Fit the telescope tracks"
  echo "       -e  Calculate the lader efficiency if the option f is on"
  echo "       -a  Align the telescope chips before fitting the tracks"
  exit 4
fi


echo "Checking if root is accessible:"
which root.exe
if [ $? -ne 0 ]; then
  echo "Root not found. Please make sure to have it in the path"
  exit
fi


#Define the name of the output files
runName=$1
ResultsDir=$1
DataDir="Data"
fileNameHits="Results_OnlyHits.root"
fileNameClusters="Results_Clusters.root"
fileNameAll="Results_All.root"
fileNameGloabalTracks="TestBeamEventTree_WithGlobalTracks.root"


CURRENT_DIR=${BASH_SOURCE%/*}
SOURCE_DIR=$CURRENT_DIR/src
root -l -b -q "CompileUserClasses.C"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOURCE_DIR

mkdir $ResultsDir
rm $ResultsDir/AnalysisResults.root

if [ $generateEvents -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C" "GenerateRandomEvents.C+(\"$ResultsDir\")"
  root -l -b -q "LoadUserClasses.C" "RawFileToTree.C+(\"$fileNameHits\",\"$ResultsDir\")"
fi

if [ $readRawTree -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C" "ReadRawTree.C+(\"$runName\", \"$fileNameHits\", \"$ResultsDir\")"
fi

if [ $makeClusters -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C"  "MakeHitsAndClusters.C+(\"$fileNameHits\", \"$fileNameClusters\", \"$ResultsDir\")"
fi

if [ $drawHitMaps -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C"  "DrawHitMaps.C+(\"$fileNameHits\",\"$fileNameClusters\", \"$ResultsDir\")"
fi

if [ $fitTelescopeTracks -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C" "Common.C" "FitTelescopeTracks.C(\"$fileNameClusters\",\"$fileNameAll\", \"$ResultsDir\")"
fi

if [ $alignTelescope -eq 1 ]; then
  root -l "LoadUserClasses.C" "ThetaAlignment.C+(\"$fileNameClusters\", \"$ResultsDir\")"
fi

if [ $getChipPositions -eq 1 ]; then
  root -l "LoadUserClasses.C" "GetLadderPositions.C+(\"$fileNameClusters\", \"$ResultsDir\")"
fi

if [ $calculateLadderEffi -eq 1 ]; then
  root -l "LoadUserClasses.C" "Common.C" "CalculateEffi.C(\"$fileNameAll\", \"$ResultsDir\")"
fi

if [ $fitGlobalTracks -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C" "Common.C" "FitGlobalTracks.C(\"$fileNameAll\",\"$fileNameGloabalTracks\", \"$ResultsDir\")"
fi


if [ $testMacro -eq 1 ]; then
  root -l "LoadUserClasses.C" "Common.C" "GetResidualInTelescope.C(\"$fileNameAll\", \"$ResultsDir\")"
fi

# if [ $testMacro -eq 1 ]; then
#   root -l -b -q "LoadUserClasses.C" "DeleteReadRawTree.C+(\"$runName\", \"$fileNameHits\", \"$ResultsDir\")"
# fi

#
#
# root -l $ResultsDir/AnalysisResults.root "OpenBrowser.C()"
