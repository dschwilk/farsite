

#################  Project General Overview  #################

FARSITE is a fire growth model. See firelab.org/https://www.firelab.org/project/farsite for the general description of Farsite.

There are three versions of Farsite that we know of. On firelab.org under Apps and Products/Fire Behavior/Farsite/Farsite Software you can download an installer (so not source code) for Windows. This version has a nice GUI unlike the one on github, and is actually a little bit farther ahead with some of the features than the github version. This version can be helpful to understand the ideas of the inputs for Farsite, for creating ignition files, and for viewing results from the github version. While you can get an understanding of the inputs to Farsite with this firelab.org version of Farsite, the inputs do not necessarily match up between different versions of Farsite. You almost need to double check the github code directly to understand what it uses for inputs.

Technically Farsite is dead code. The third location is probably the most up to date version, and it is managed by Stuart Brittain. It is definitely not open to the public, and the github Farsite is basically just a section of this Assembla version ripped out to only run on Linux (but ripped out a few years back). It's a bit weird, cause this Assembla version is built for Windows, and is actually more for Flammap than for Farsite, but it still has the Farsite in it as the Assembla version appears to be a collection of a bunch of different Fire Behavior related packages. Probably what happened was that Flammap and Farsite were always very similar (both give spatial fire behavior vs the single point fire behavior of behave, but Flammap doesn't do behavior with time like Farsite), so they were worked on separately, then eventually Farsite got merged back into Flammap. This means that the structure exists to put a lot of Flammap stuff into Farsite, but Farsite has really just kind of been sitting there in that code.

While the Assembla version is not open to the public, it may be possible to get a binary version executable for Windows from Stuart. Stuart maintains the Assembla version of Farsite for use in FlamMap6 desktop and cloud modeling services being developed by IBM. The binaries described in this paragraph were originally written for WFDSS and subsequently integrated into FlamMap desktop. The reason the Assembla version and thus this github version of Farsite have so many extra parts is cause the Assembla code base is not intended just for FlamMap, but for an entire suite of modeling programs, of which Farsite is a distinct project and model.

Also, it looks like the github version was ripped out of the Assembla version sometime relatively close to release of the latest firelab.org version, so it has most of the features of that version, but is behind by maybe a version or two. Eventually, we are hoping that someone will go through and manually update as much as possible to this github version, using the most up to date Assembla version. For now, the github version "should" be good enough for the purposes of general fires with gridded winds.

So here is where the github version is compared to the Windows version and Assembla version: It doesn't have post frontal combustion, it does have moisture stuff, but maybe not the best version, it needs the rogue trajectory spotting fixed by a later update, it needs the ignition shapefile processing to handle old fire perimeters, it needs the ignition shapefiles to handle a single point instead of a small circle of points, it needs gdal stuff builtin to handle shapefiles with projections correctly (now you manually need to make sure everything matches projections and it is fine).

The WindNinja stuff you see in the github version of Farsite, if you open the set of files in an IDE like qt creator and right click on the functions with the "find usages" tool, you will see that at some point the WindNinja stuff exists there, but is not linked in or used anywhere. This is also true of the Flammap stuff, which you probably will see has more inputs than what is used in Farsite. These WindNinja files are an old version of the WindNinja API, so just ignore it. Instead of trying to get that set of code to work, we took the ideas from Assembla farsite to put in functionality for gridded wind files so you can just use output from WindNinja as inputs to Farsite.


Some extra notes from Stu:
The version in Git is not maintained by me, but it was indeed taken from the version I maintain in assembla with the removal of GDAL and WindNinja libraries, and merged with the Fuel Conditioning code.

Neither the assembla nor git versions utilize post-frontal combustion, nor do they allow for attacking the fire. I do not believe anything else was stripped from the git version as compared to the assembla version. Since GDAL and WindNinja have been removed from the git version, that version is not projection aware nor can it run WindNinja on it's own.


#################  General Overview of Examples  #################

Not exactly sure where the files for the Panther example come from, it is apparent that there are two .lcp files for which the panther case is run on, namely 1177973.lcp and cust.lcp. The 1177973 case has to be run as is, while the cust case has the option to try out a different ignition file and custom fuels file by changing some of the input files.

The cougarCreek-fire (others to add later) cases have recently been added, the cougarCreek-fire case uses gridded wind outputs from WindNinja.


#################  Step by Step Guide for Building and Running the Script  #################

The following worked on fresh installations of Ubuntu 16.04 and 18.04:
cd farsite/src
make
./TestFARSITE ../example/Panther/runPanther.txt

You may need to modify all /HOME/LOCATION/farsite with your root to the farsite dir in each of the input files. For running the cougarCreek-fire case, the path you need to modify is $scriptRoot/farsite instead of /HOME/LOCATION/farsite. At some point in time the same fake root path will be changed to be consistent for all the files.

Here is some command line code you can use to modify the paths:
varToReplace="\$scriptRoot"(((might need to add more path stuff here)))
aboveBaseDir="((((root directory to folder location))))
preppedVarToReplace=$(sed 's/\//\\\//g' <<<"$varToReplace")
preppedAboveBaseDir=$(sed 's/\//\\\//g' <<<"$aboveBaseDir")
grep -rl $preppedVarToReplace $finalScriptDir --exclude-dir=.git --exclude-dir=src --exclude=readme | xargs sed -i 's/'$preppedVarToReplace'/'$preppedAboveBaseDir'/g'
success=$?
echo $success

Note that a 0 means success, a 123 means nothing needed replaced, anything else is probably a fail. You'll probably have to play around a bit to get the right paths.


If you hate that changing the paths means running "git status" results in a bunch of changed files you really don't want to keep track of, but that git has to keep track of cause they are the example files, you can use something like the following:
examplesDir=(((path to examples directory)))
git ls-files -- $examplesDir | xargs -l git update-index --assume-unchanged

To change it back, use:
git ls-files -- $examplesDir | xargs -l git update-index --no-assume-unchanged
success=$?


#################  General Overview for Viewing Results on Windows Machine  #################

If you don't want to install farsite, you're on your own, just need some kind of GIS software:
go to https://www.firelab.org/, on this webpage look for "apps and products/fire behavior/farsite". On this farsite page, look for "farsite software". This leads you to a farsite for windows installer.
After installing farsite, run farsite, and in the menu search for "view/view landscape file (.LCP)/2D window". Use the "cougarCreekFire.lcp" file.
After the file loads, right click the section of the box labeled "Visible Theme" in the section with the numbers and colors. This should pop up a menu "Choose Color Ramp". Select "Load Color File (.CLR)" and use the "colorInput.CLR" file. Note you need to make your own if doing a different .lcp file than this example, I randomly changed colors till it looked good, then saved this color scheme.
In the menu search for "view/View Vector File". Change the extensions it is looking for to ".shp", then you can view the "cougarCreekFire_Perimeters.shp" fire perimeters and "cougarCreekFire_Spots.shp" probabilistic spot location files. Need to select the boxes labeled "2D" to get the files to actually show the pictures, left click the lines or dots to choose colors, right click to choose the sizes.
Note that you can see other files as well, but might need to hunt down their specific types using either "vector" or "raster" input from the "view" menu.




