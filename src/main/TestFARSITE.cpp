// TestFARSITE.cpp : Defines the entry point for the console application.

// Authors: ?? and Dylan Schwilk


// This source file only depends upon the main shared FARSITE library declared
// in FARSITE.h and only interacts with the fire growth model through that
// interface.

#include "FARSITE.h"  

// indicators header library for progress bars
#include "indicators/cursor_control.hpp"
#include "indicators/progress_bar.hpp"
#include "indicators/dynamic_progress.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>

using namespace std::chrono_literals;
using namespace std;

static const string help_str = R"(TestFARSITE expects one parameter
Usage: TestFARSITE [commandfile]
Where:
    [commandfile] is the path to the command file. The command file contains
    command lines for multiple Farsite runs, each run's command on a separate
    line. Each command expects six parameters, all required:

[LCPName] [InputsFileName] [IgnitionFileName] [BarrierFileName] [outputDirPath] [outputsType]

Where:
    [LCPName] is the path to the Landscape File
    [InputsFileName] is the path to the FARSITE Inputs file (ASCII Format)
    [IgnitionFileName] is the path to the Ignition shape file
    [BarrierFileName] is the path to the Barrier shape file (0 if no barrier)
    [outputDirPath] is the path to the output files base name (no extension)
    [outputsType] indicates which output files to write (1 = ASCII grids,
                  2 = FlamMap binary grids, 0 = both grid types, 4 = fuels
                  information plus ASCII grids)
)";

atomic_bool FarsitesComplete = false; // Flag for progress thread to check to
                                      // determine if all farsite runs have
                                      // completed.

// The 6 required command file arguments
struct FarsiteCommand {
    std::string lcp;
    std::string input;
    std::string ignitPath;
    std::string barrierPath;
    std::string outPath;
    int outType;
};

istream& operator>>(istream& is, FarsiteCommand& fc)
{
    FarsiteCommand nfc;
    if(is >> nfc.lcp >> nfc.input >> nfc.ignitPath >> nfc.barrierPath
       >> nfc.outPath >> nfc.outType)
    {
        fc = nfc;
    }
    return is;
}

ostream& operator<<(ostream& os, const FarsiteCommand fc)
{
    os << fc.lcp << " " << fc.input << " " << fc.ignitPath << " "
       << fc.barrierPath << " " << fc.outPath << " " << fc.outType;
    return os;
}

// std::mutex iomutex provide by FARSITE.h
void printError(const char *theerr,  const char *fname)
{
    std::lock_guard<std::mutex> iolock(iomutex);
    cerr << "Error: " << theerr << " for " << fname << "\n";
}

void printMsg(const char *msg)
{
    std::lock_guard<std::mutex> iolock(iomutex);
    cout << msg << "\n";
}

void printMsg(const std::string msg)
{
    std::lock_guard<std::mutex> iolock(iomutex);
    cout << msg << "\n";
}

// Function to handle progress updates for all farsite instances. Function
// will query CFarsite instances for current task/status (string) and progress
// percentage within that status.
void ProgressThread(void *_pFarsites, int nFarsites, int interval)
{
    using namespace indicators;

    CFarsite **pFarsites = (CFarsite **)_pFarsites;
    ProgressBar* bar_parray[nFarsites];  // to store pointers to dynamically
                                         // allocated progress bars
 
    int f;

    // store the pointers in an array so we can delete them once the
    // DynamicProgress container is out of scope
    for(f=0; f < nFarsites; f++)
    {
        ProgressBar *nbar = new ProgressBar(option::BarWidth{50},
                                            option::PrefixText{string("Farsite #") + to_string(f+1)});
        bar_parray[f] = nbar;
    }
    
    if (!FarsitesComplete)
    {
        DynamicProgress<ProgressBar> bars;
        for(f=0; f < nFarsites; f++)
        {
            bars.push_back(*(bar_parray[f]));
        }
    
        while(!FarsitesComplete)
        {
            for(f = 0; f < nFarsites; f++)        
            {
                if(pFarsites[f]) {
                    const char* status = pFarsites[f]->GetFarsiteStatusString();
                    int progress = pFarsites[f]->GetFarsiteProgress();
                    bars[f].set_progress(progress);
                    if(std::strcmp(status, "Complete")==0)
                    {
                        bars[f].set_option(option::PostfixText{"Writing files"});
                    } else {
                        bars[f].set_option(option::PostfixText{status});
                    }
                }                
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
        for(f=0;f<nFarsites;f++)
        {
            bars[f].mark_as_completed();
        }
    }

    // delete dynamically allocated bars
    for(f=0; f < nFarsites; f++)
    {
        delete bar_parray[f];
    }
}
    
// This function must be called from the main thread -- it is unsafe to have
// multiple versions running concurrently due to how the ICF class handles
// error messages and strings. I'm not precisely sure why because it seems like
// it should be ok. A thread-safe ICF class would allow this to be run in a
// thread.
int LoadCommandInputs(CFarsite *pFarsite, int f, FarsiteCommand fc)
{
    int ret;
    printMsg(string( "Loading lcp file for Farsite #") + to_string(f+1) + ": " + fc.lcp);
    if ( !pFarsite->SetLandscapeFile(fc.lcp.c_str()))
    {
        printMsg(string( "Error Loading lcp file for Farsite #") + to_string(f+1) + ": " + fc.lcp);
        return -1;
    }

    printMsg(string( "Loading inputs file for Farsite #") + to_string(f+1) + ": " + fc.input);
    ret = pFarsite->LoadFarsiteInputs(fc.input.c_str());
    if ( ret != 1 )
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(fc.input.c_str(), a);
        return ret;
    }

    printMsg(string( "Loading ignition file for Farsite #") + to_string(f+1) + ": " + fc.ignitPath);
    ret = pFarsite->SetIgnition(fc.ignitPath.c_str());
    if ( ret != 1 )
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(fc.input.c_str(), a);
        return ret;
    }
    
    if(fc.barrierPath.length() > 2)
    {
        printMsg(string( "Loading barrier file for Farsite #") + to_string(f+1) + ": " + fc.barrierPath);
        pFarsite->SetBarriers(fc.barrierPath.c_str());
    }
    return ret;
}

/*******************************************************************************************
 * WriteOutputs Write all outputs. outputPath is directory and base file name.
 * The code below appends an underscore, file type description, and extension
 * to this string to create the various output file name. outType is integer
 * indicating which group of outputs to produce. This is verbose and could be
 * handled more cleanly. I had this set to update progress but it it was
 * choppy. Better to just test when writing is occurring and use some spinner
 * or similar I think.
 *
 *******************************************************************************************/
int writeOutputs(CFarsite *pFarsite, int outType, const std::string outputPath)
{
    // 0 = all outputs, 1 = ascii grids, 2 = binary grids, 4 = same as 1 plus
    // one and ten hour fuels
    int ret;
     
    if(outType == 0 || outType == 1 || outType == 4)
    {
        // type 4:
        if(outType == 4)
        {
            pFarsite->WriteOneHours(outputPath.c_str());
           
            pFarsite->WriteTenHours(outputPath.c_str());
           
        }

        // type 0 and 1:
        ret = pFarsite->WriteCrownFireGrid( (outputPath + "_CrownFire.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }

        ret = pFarsite->WriteArrivalTimeGrid((outputPath + "_ArrivalTime.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteIntensityGrid( (outputPath + "_Intensity.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteFlameLengthGrid((outputPath + "_FlameLength.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteSpreadRateGrid( (outputPath + "_SpreadRate.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteSpreadDirectionGrid( (outputPath + "_SpreadDirection.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteHeatPerUnitAreaGrid( (outputPath + "_HeatPerUnitArea.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteReactionIntensityGrid( (outputPath + "_ReactionIntensity.asc").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
       
    }
        
    if(outType == 0 || outType == 2)
    {
        ret = pFarsite->WriteCrownFireGridBinary( (outputPath + "_CrownFire.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteArrivalTimeGridBinary( (outputPath + "ArrivalTime.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteIntensityGridBinary( (outputPath + "_Intensity.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteFlameLengthGridBinary( (outputPath + "_FlameLength.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteSpreadRateGridBinary( (outputPath + "_SpreadRate.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteSpreadDirectionGridBinary((outputPath + "_SpreadDirection.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteHeatPerUnitAreaGridBinary( (outputPath + "_HeatPerUnitArea.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
        
        ret = pFarsite->WriteReactionIntensityGridBinary((outputPath + "_ReactionIntensity.fbg").c_str());
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputPath.c_str());
        }
       
    }
    /*sprintf(outputPath.c_str(), "%s_Moistures.txt", outs[f]);
      ret = pFarsite->WriteMoistData(outputPath.c_str());
      if(ret != 1)
      {
      char *a = pFarsite->CommandFileError(ret);
      printError(a, outputPath.c_str());
      }*/

    // all output types:
    ret = pFarsite->WriteIgnitionGrid( (outputPath + "_Ignitions.asc").c_str());
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputPath.c_str());
    }
    
    ret = pFarsite->WritePerimetersShapeFile( (outputPath + "_Perimeters.shp").c_str());
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputPath.c_str());
    }
    
    ret = pFarsite->WriteSpotGrid( (outputPath + "_SpotGrid.asc").c_str());
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputPath.c_str());
    }
    
    ret = pFarsite->WriteSpotDataFile( (outputPath + "_Spots.csv").c_str());
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputPath.c_str());
    }
    
    ret = pFarsite->WriteSpotShapeFile( (outputPath + "_Spots.shp").c_str());
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputPath.c_str());
    }
       
    ret = pFarsite->WriteTimingsFile( (outputPath + "_Timings.txt").c_str());
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputPath.c_str());
    }

    return ret;
}


// Launch a farsite run. this can run asynchronously in its own thread. LoadCommandInputs
// must have been already called on the Farsite object.
void LaunchFarsite(void *_pFarsite, int f, FarsiteCommand fc)
{
    // cast array of pointers to farsite objects.
  	CFarsite *pFarsite = (CFarsite *)_pFarsite;
    int ret;
    ret = pFarsite->LaunchFarsite();

    if(ret != 1) 
    {
        printMsg(string("Error: LaunchFarsite failure for farsite #") + to_string(f+1) );
        return;  // throw?
    }
     // write outputs
    ret = writeOutputs(pFarsite, fc.outType, fc.outPath);
    if(ret != 1) 
    {
        printMsg(string("Error: Writing results failure for farsite #") + to_string(f+1) );
        return;
    }
}

// Multi process version of main(). 
int MPMain(int argc, char* argv[])
{
	if(argc != 2)
	{
        cout << help_str;
		exit(1);
	}

    ifstream cmdfile;
    std::string cmdline; 
    std::vector<FarsiteCommand> cmds;
    cmdfile.open(argv[1]);
    
    while(std::getline(cmdfile, cmdline))
    {
        std::istringstream str(cmdline);
        FarsiteCommand fc;
        str  >> fc;
        cmds.push_back(fc);
    }
    
    int nFarsites = cmds.size(); 
   
    std::vector<std::thread> FarsiteThreads;
	CFarsite **pFarsites = new CFarsite*[nFarsites];
	for(int i = 0; i < nFarsites; i++)
		pFarsites[i] = nullptr;  // start empty

    auto progressThread = std::thread(ProgressThread, &pFarsites[0], nFarsites, 500);

    int f;
	for(f = 0; f < nFarsites; f++)
	{
		pFarsites[f] = new CFarsite();
		
		if(!FarsitesComplete)
		{
            // Load inputs
            int ret = LoadCommandInputs(pFarsites[f], f, cmds[f]);
            if(ret != 1) 
            {
                char *a = pFarsites[f]->CommandFileError(ret);
                printError(cmds[f].input.c_str(), a);
                delete pFarsites[f];
                pFarsites[f] = nullptr;
            }

            // Launch each simulation in own thread
			printMsg(string("Launching Farsite #") + to_string(f+1));
                     
            FarsiteThreads.push_back(std::thread(LaunchFarsite, pFarsites[f], f, cmds[f]));
		}
		
	}
    
	while (!FarsiteThreads.empty())
    {
        FarsiteThreads.back().join();
        FarsiteThreads.pop_back();
    }

    FarsitesComplete = true;
    progressThread.join();

//	if(complete)
//	{
		printMsg("Done\n");
//	}
    
	for(int i = 0; i < nFarsites; i++)
	{
        if(pFarsites[f])
            delete pFarsites[f];
	}
	delete[] pFarsites;
    cmdfile.close();
	return 0;
}


// Call main function
int main(int argc, char* argv[])
{
	MPMain(argc, argv);
}
