#!/bin/bash
if [ "$#" -lt 2 ]
then
    echo "Two arguments are require"
    echo "Usage: `basename $0` -Event ID -ResultsDirectory"
    exit 1
fi

optList="r"
generateRandomEvents=0
EXIT=0
while getopts $optList option
do
  case $option in
    r ) generateRandomEvents=1;;
    * ) echo "Unimplemented option chosen."
    EXIT=1
    ;;
  esac
done

shift $(($OPTIND - 1))

if [ $EXIT -eq 1 ]; then
  echo "Usage: `basename $0` (-$optList)"
  echo "       -r do not analyse an existing raw file, generate random events instead (default: $generateRandomEvents)"
  exit 4
fi


echo "Checking if root is accessible:"
which root.exe
if [ $? -ne 0 ]; then
  echo "Root not found. Please make sure to have it in the path"
  exit
fi


#Define the name of the output files
ResultsDir=$1
EventID=$2
fileNameAll="Results_All.root"

CURRENT_DIR=${BASH_SOURCE%/*}
SOURCE_DIR=$CURRENT_DIR/src
root -l -b -q "CompileUserClasses.C"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOURCE_DIR


root -l   "LoadUserClasses.C" "Common.C" "eve.C(\"$ResultsDir\",\"$fileNameAll\", $EventID)"
