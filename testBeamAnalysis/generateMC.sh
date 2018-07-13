#!/bin/bash
if [ "$#" -lt 1 ]
then
  echo "Two arguments are require"
  echo "Usage: `basename $0` -runNameName"
  exit 1
fi

optList="rsfc"
generateRandomEvents=0
getRemoteFile=0
fitTelescopeTracks=0
calculateLadderEffi=0
alignTelescope=0
EXIT=0
while getopts $optList option
do
  case $option in
    r ) generateRandomEvents=1;;
    s ) getRemoteFile=1;;
    f ) fitTelescopeTracks=1;;
    c ) calculateLadderEffi=1;;
    a ) alignTelescope=1;;
    * ) echo "Unimplemented option chosen."
    EXIT=1
    ;;
  esac
done

shift $(($OPTIND - 1))

if [ $EXIT -eq 1 ]; then
  echo "Usage: `basename $0` (-$optList)"
  echo "       -r do not analyse an existing raw file, generate random events instead (default: $generateRandomEvents)"
  echo "       -s  Get the input files from ssh(default: $getRemoteFile)"
  echo "       -f  Fit the telescope tracks"
  echo "       -c  Calculate the lader efficiency if the option f is on"
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
fileNameOnlyTracks="Results_OnlyTracks.root"
fileNameAll="Results_All.root"
# dataPathSSH="l-arakotoz@dedippcaa59:soft/mosaic/high_level/new/new-alpide-software/Data"
dataPathSSH="l-arakotoz@dedippcaa59:soft/mosaic/high_level/old/new-alpide-software/data"

CURRENT_DIR=${BASH_SOURCE%/*}
SOURCE_DIR=$CURRENT_DIR/src
root -l -b -q "CompileUserClasses.C"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOURCE_DIR

rm -r $ResultsDir
mkdir $ResultsDir


if [ $getRemoteFile -eq 1 ]; then
  scp $dataPathSSH/$runName $DataDir/.
fi

if [ $generateRandomEvents -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C" "Common.C" "GenerateRandomEvents.C(100, \"$runName\", \"$ResultsDir\")"
  root -l -b -q "LoadUserClasses.C" "Common.C" "RawFileToTree.C(\"$runName\", \"$fileNameHits\", \"$ResultsDir\")"
else
  root -l -b -q "LoadUserClasses.C" "ReadRawTree.C+(\"$runName\", \"$fileNameHits\", \"$ResultsDir\")"
fi

root -l -b -q "LoadUserClasses.C"  "DrawHitMaps.C+(\"$fileNameHits\", \"$ResultsDir\")"
# root -l -b -q "LoadUserClasses.C"  "MakeHitsAndClusters.C+(\"$fileNameHits\", \"$fileNameClusters\", \"$ResultsDir\")"
#
if [ $alignTelescope -eq 1 ]; then
  root -l "LoadUserClasses.C" "FinalAlignment.C+(\"$fileNameClusters\", \"$ResultsDir\")"
fi

if [ $fitTelescopeTracks -eq 1 ]; then
  root -l -b -q "LoadUserClasses.C" "Common.C" "FitTelescopeTracks.C(\"$fileNameClusters\",\"$fileNameAll\", \"$ResultsDir\")"
  if [ $calculateLadderEffi -eq 1 ]; then
    root -l -b -q "LoadUserClasses.C" "Common.C" "CalculateEffi.C(\"$fileNameAll\", \"$ResultsDir\")"
  fi
fi
#
#
root -l $ResultsDir/AnalysisResults.root "OpenBrowser.C()"
