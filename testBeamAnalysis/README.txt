In this code, there are three different script that can be run:
-A standalone .C macro "CompareTriggerTime.C".
-A bash script to run a data analysis "runAnalysis.sh".
-A bash script to display a given interval of events for a given run "eventDisplay.sh"

*********CompareTriggerTime.C*********
This macro simply read the raw trees that correspond to different boards and plot the delay between the time triggers. It takes as argument the base name of the run, and the two index of the two boards.
******example:

root -l CompareTriggerTime.C\(\"multinoiseScan_180703_175155-B0\",3010,0\)

This will plot the trigger time delay between the two boards 3010 (ladder) and 0 (telescope) for the run 180703_175155.



*********runAnalysis.sh*********
This script compile the different classes in the src directory, and run in chains the different macros of the analysis. The total chain of the analysis is:
1-RawTreeToTree.C: It take a raw tree of hits (stored in the Data directory), and group hits that correspond to the same trigger into events.
2-MakeHitsAndClusters.C: It takes the output tree of the 1-, and do the clustering for the different chips.
3-DrawHitMaps.C: Draw the hitmaps and other histograms (multiplicity..) and store them in the results directory
4-FitTelescopeTracks.C: Loop over the clusters in the chips of the telescops and fit associated tracks
5-CalculateEffi.C:

***** the script runAnalysis.sh, needs three arguments. The name of the raw tree. The detector board to which this tree corresponds (TODO: read different trees at the same time), and the third argument is the folder name where the results will be saved.
***** the different options for the script are: -rsfc
-r: To generate random events
-s: To download the raw file name from ssh
-f: To fit the telescope tracks
-c: To Calculate the efficiency in the ladder if the telescope tracks are fitted

*****example:

./runAnalysis.sh -s multinoiseScan_180703_175155-B0-ibhic0.root 0 Run_180703_175155

This will get the raw file, and run the first 3 steps of the analysis and save the results in the folder Run_180703_175155


*********eventDisplay.sh*********
This display an event for an already analysed run. It takes three parameters: The name of the run folder. The minimum and the maximum of the events to be displayed.
Example: ./eventDisplay.sh Run_180703_175155 0 10

This will display the events 0-10 of the run 180703_175155
