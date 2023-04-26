# Project General Overview  #

FARSITE is a trademark owned by Mark Finney. 

FARSITE is a fire growth model. See firelab.org/https://www.firelab.org/project/farsite for the general description of Farsite.

The compiled windows GUI version of farsite is no longer avialable, but the mean travel time model is implemented in [FlamMap](https://www.firelab.org/project/flammap). Compiled windows command-line executables for several Missoula Fire Lab applications including a compiled `TestFARSITE.exe` application are available at https://www.alturassolutions.com/FB/FB_API.htm

This is a fork of the command line code that was released as a mix of public domain and GNU gpl in 2016 and that source includes code by Finney, Collin Bevins and others. This version includes some additions to the weather handling by Loren Atwood in 2018-2019. This is based on file headers and commits so there may be others.

## Notes on this fork by DWS  ##

I downloaded the public domain linux farsite code in September 2016 from the website maintained by Stuart Brittain [Farsite for Linux](http://sbrittain.net/Farsite/Distrib/Linux/Farsite_Linux.htm). I found a couple of minor reasons the code would not compile and fixed those. Following successful compilation, I then made some changes to reorganize the code, create a makefile, and move the examples to their own directory. I got things running, fixed some more compile issues on newer C++ compilers, tested against the examples, and wrote some steering code for batch use in simulations. However, I never used this for the simulation project I was preparing it for. I hosted the code on github and called the repo "firemod" with a README explaining its genealogy. 

A few years later, the RMRS Missoula Fire Sciences Lab GitHub organization page (https://github.com/firelab) hosted code that appears to be originate from the same 2016 archive. The original commit in that repo made in 2018 by nwagenbrenner matches the 2016 dated tar.gz archive except that one syntax error I found had also been fixed in the initial commit. The README in that github repo makes no reference to that "Farsite for Linux", but I've confirmed they have the same exact starting code base). The latest commits to that GitHub hosted repository were in 2019 and involved significant additions to the weather handling code allowing gridded winds so that this version could be used with [WindNinja](https://github.com/firelab/windninja) outputs. An example of their use is in the cougarCreek example. I cloned that github repo to compare with my own `firemod` named version and merged in my minor changes. Then, in early 2023, the source repo called `farsite` at the [RMRS Missoula Fire Sciences github organization page](https://github.com/firelab) was either made private or deleted. After the disappearance of the firelab parent repo, the github forks now show up as forks of mbedward's repo. See [github's explanation for what happens when a public fork is made private or deleted](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/what-happens-to-forks-when-a-repository-is-deleted-or-changes-visibility).

With the deletion of the Missoula Fire Lab's public GitHub repository I am assuming this is abandoned code. I've therefore not been conservative about making unimportant changes that do not influence function. Instead I prioritized understanding the code so did things like re-indent confusingly indented code sections and converting line endings. That said, it should not be too difficult to merge in changes that are based on the original repo if those are ever available.

## Notes on inputs ##

TODO: Document command files, input files, input types


RAWS: dateTime, temp_c, rh, windSpeed_kmh, wind speed (km/hr), precip_mm




# Text that was in the README.md in firelab GitHub repo prior to March 2023: #

[Author apparently Loren Atwood in 2019. When the author(s) of the original readme below I've included below refer to the "github version" they mean this repository's ancestor with an original code commit in March 2018 by nwagenbrenner.]

There are three versions of Farsite that we know of. On firelab.org under Apps and Products/Fire Behavior/Farsite/Farsite Software you can download an installer (so not source code) for Windows. This version has a nice GUI unlike the one on github, and is actually a little bit farther ahead with some of the features than the github version. This version can be helpful to understand the ideas of the inputs for Farsite, for creating ignition files, and for viewing results from the github version. While you can get an understanding of the inputs to Farsite with this firelab.org version of Farsite, the inputs do not necessarily match up between different versions of Farsite. You almost need to double check the github code directly to understand what it uses for inputs.

Technically Farsite is dead code. The third location is probably the most up to date version, and it is managed by Stuart Brittain. It is definitely not open to the public, and the github Farsite is basically just a section of this Assembla version ripped out to only run on Linux (but ripped out a few years back). It's a bit weird, cause this Assgembla version is built for Windows, and is actually more for Flammap than for Farsite, but it still has the Farsite in it as the Assembla version appears to be a collection of a bunch of different Fire Behavior related packages. Probably what happened was that Flammap and Farsite were always very similar (both give spatial fire behavior vs the single point fire behavior of behave, but Flammap doesn't do behavior with time like Farsite), so they were worked on separately, then eventually Farsite got merged back into Flammap. This means that the structure exists to put a lot of Flammap stuff into Farsite, but Farsite has really just kind of been sitting there in that code.

While the Assembla version is not open to the public, it may be possible to get a binary version executable for Windows from Stuart. Stuart maintains the Assembla version of Farsite for use in FlamMap6 desktop and cloud modeling services being developed by IBM. The binaries described in this paragraph were originally written for WFDSS and subsequently integrated into FlamMap desktop. The reason the Assembla version and thus this github version of Farsite have so many extra parts is cause the Assembla code base is not intended just for FlamMap, but for an entire suite of modeling programs, of which Farsite is a distinct project and model.

Also, it looks like the github version was ripped out of the Assembla version sometime relatively close to release of the latest firelab.org version, so it has most of the features of that version, but is behind by maybe a version or two. Eventually, we are hoping that someone will go through and manually update as much as possible to this github version, using the most up to date Assembla version. For now, the github version "should" be good enough for the purposes of general fires with gridded winds.

So here is where the github version is compared to the Windows version and Assembla version: It doesn't have post frontal combustion, it does have moisture stuff, but maybe not the best version, it needs the rogue trajectory spotting fixed by a later update, it needs the ignition shapefile processing to handle old fire perimeters, it needs the ignition shapefiles to handle a single point instead of a small circle of points, it needs gdal stuff builtin to handle shapefiles with projections correctly (now you manually need to make sure everything matches projections and it is fine).

The WindNinja stuff you see in the github version of Farsite, if you open the set of files in an IDE like qt creator and right click on the functions with the "find usages" tool, you will see that at some point the WindNinja stuff exists there, but is not linked in or used anywhere. This is also true of the Flammap stuff, which you probably will see has more inputs than what is used in Farsite. These WindNinja files are an old version of the WindNinja API, so just ignore it. Instead of trying to get that set of code to work, we took the ideas from Assembla farsite to put in functionality for gridded wind files so you can just use output from WindNinja as inputs to Farsite.

## Some extra notes from Stu: ## 

The version in Git is not maintained by me, but it was indeed taken from the version I maintain in assembla with the removal of GDAL and WindNinja libraries, and merged with the Fuel Conditioning code.

Neither the assembla nor git versions utilize post-frontal combustion, nor do they allow for attacking the fire. I do not believe anything else was stripped from the git version as compared to the assembla version. Since GDAL and WindNinja have been removed from the git version, that version is not projection aware nor can it run WindNinja on it's own.


## General Overview of Examples  ##

Not exactly sure where the files for the Panther example come from, it is apparent that there are two .lcp files for which the panther case is run on, namely 1177973.lcp and cust.lcp. The 1177973 case has to be run as is, while the cust case has the option to try out a different ignition file and custom fuels file by changing some of the input files.

The cougarCreek was recently added after work on a separate repo to automate farsite runs using WRF files as inputs to WindNinja: https://github.com/latwood/WRF-WindNinja-FarsiteScript. The cougarCreek case was generated in the first working run of said other repo script and uses gridded wind outputs from WindNinja.

The test1177973.input file is an example input file that Loren put together in the summer of 2018, as he tried to figure out what inputs are actually used in this version of farsite, and what is actually done by the code at different steps. Probably is not an easy read, but might be helpful. To make WindNinja happy during these experiments, the 1177973 case was renamed test1177973.

At some point in time an attempt to convert .raws files to .wnd and .wtr was done using ideas from the test1177973.input file. The script doing the conversion was lost, but the method should have been to simply extract values from the .raws file for the .wnd file, and to do averaging of .raws values to setup the .wnd file. While said script for the conversion was lost, the resulting .wnd and .wtr files were added to the test1177973 case for future use.

The flatland case was given to Loren by Stu after it was discovered that Linux farsite was not running gridded WindNinja winds properly. It acts as the ideal test case because terrain does not affect, which makes it easy to compare using different kinds of inputs on the results. Because of this, when the flatland case was added to the farsite repo, it contains a ton of different case comparisons, as well as pictures showing that the gridded wind fixes to Linux farsite were effective. Note that using gridded WindNinja winds with a .atm file did not work, but gridded WindNinja winds with a .atm file and a .raws file for the weather (ignoring the .raws winds) did work. Part of the reason for this is that the .raws input triggers an interpolation method in Linux farsite to extend the .raws input weather to cover the entire required weather time period, but the same is not done for .atm inputs. Note that it is hard to tell whether the atm vs the RAWS file is working correctly, but the otherRAWS case helps as an additional comparison. When Linux Farsite did not work with WindNinja gridded winds, the atmRAWS and RAWS looked the same regardless of the atm files used.


## Step by Step Guide for Building and Running the Script  ##

```
cd <pathToFarsiteRepo>
make
./bin/TestFARSITE ../examples/Panther/runPanther.txt 2>&1 | tee scriptRun.log
```

Note from DWS: at this point there was a bunch of text about absolute paths. I'll move all that to a historical record files. The above example runs as is for a start. The input files all do work with relative paths but paths must be relative to the working directory (where the executable was called). Allowing paths relative to the files in which they occur would be an important improvement.

# Additional notes #

## Quick notes on code structure by DWS:

- There are two separate date and time libraries here. Older is C style library `cdtlib.h`. Other is C++ `SemTime` class declared in `semtime.h`. Seems main reason for the older library is need for some functions related to solar radiation etc, not implemented in the C++ class version which is purely date-time operations
- There are two implementations of dead fuel moisture calculations. Code allows choosing.
- Overall, a huge amount of pointer use, naked "new", etc. Any modifications to the main fire growth simulation would be quite difficult because of this. Also Many VERY long functions.
- Fire fighting ("attack") code is integrated in the simulation code but has no effect on CLI because no inputs are used. In other words, the flag variables that turn this on are always false. I doubt this is much of a performance cost but it should be a small one because I don't think the compiler can figure out those branches are unreachable. The main drawback to that integration is added complexity to already very long functions with many if statements and explicit loops.


