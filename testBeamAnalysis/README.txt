In this code, there are two different script that can be run:
-A bash script to run a data analysis "runAnalysis.sh".
-A bash script to display a given interval of events for a given run "eventDisplay.sh"


*********runAnalysis.sh*********
This script compile the different classes in the src directory, and run in chains the different macros of the analysis. The total chain of the analysis is:
1-ReadRawTree.C: It take a raw tree of hits (stored in the Data directory), and group hits that correspond to the same trigger into events.
2-MakeHitsAndClusters.C: It takes the output tree of the 1-, and do the clustering for the different chips.
3-DrawHitMaps.C: Draw the hitmaps and other histograms (multiplicity..) and store them in the results directory
4-FinalAlignment.C: So far this macro align the telescope chips. TODO: Align the ladder using the same macro
5-FitTelescopeTracks.C: Loop over the clusters in the chips of the telescops and fit associated tracks
6-CalculateEffi.C:

***** the script runAnalysis.sh, needs one argument which is the name of the run (date_time) assuming that the data corresponding to this run are available in the Data folder.
***** the different options for the script are: -rsfc
-r  To read the raw trees after merging them. Do not use this option if the tree is already read
-d  Fill the hitmaps and display them
-c  Make the clusters
-f  Fit the telescope tracks
-e  Calculate the lader efficiency if the option f is on
-a  Align the telescope chips before fitting the tracks

*****example:

./runAnalysis.sh -r -d 180703_175155

This will merge the raw tree of the different board from the run 180703_175155, read the raw pixel tree and fill the output tree event and draw the hitmaps.


*********eventDisplay.sh*********
This display an event for an already analysed run. It takes two parameters: The name of the run folder. The id of the event to be displayed.
Example: ./eventDisplay.sh Run_180703_175155 10

This will display the events 10 of the run 180703_175155
