// TestFARSITE.cpp : Defines the entry point for the console application.
//

#include "FARSITE.h"

#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

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
    [outputsType] is the file type for outputs (0 = both, 1 = ASCII grid,
                  2 = FlamMap binary grid

)";


mutex iomutex;  // for locking std::cout
atomic_bool cancelRequest = false; // Flag to end program based on user input.
                                   // Not yet changed based on user.

// static const int MAX_PATH = 1512; // char buffer length for file paths

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
    
void printError(const char *theerr,  const char *fname)
{
    std::lock_guard<std::mutex> iolock(iomutex);
    cout << "Error: " << theerr << " for " << fname << "\n";
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

// This is very simple for now. It would be nice to have a progress bar of the
// type in the indicators library.
void ProgressThread(void *_pFarsites, int nFarsites)
{
 	CFarsite **pFarsites = (CFarsite **)_pFarsites;
	int progress = 0;
	while(!cancelRequest)
	{
		for(int f = 0; f < nFarsites; f++)        
		{
            progress=0;
 			std::lock_guard<std::mutex> {iomutex};
            if(pFarsites[f]) {
                progress = pFarsites[f]->GetFarsiteProgress();
                const char* status = pFarsites[f]->GetFarsiteStatusString();
          
                if (progress == 0)
                {
                    cout << "Farsite #" << f+1 << status;
                } else if (progress == 100)
                {
                    cout << "Farsite #" << f+1 << " Writing results. " << status;
                } else
                {              
                    cout << "Farsite #" << f+1 << " " << progress << "% complete. " << status;
                }
            }
		}
        iomutex.lock();
        cout << "\n" <<  std::flush;
        iomutex.unlock();
        std::this_thread::sleep_for(1000ms);
	}
}

// Write all outputs. outputPath is directory and base file name. The code
// below appends an underscore, file type description, and extension to this
// string to create the various output file name. outType is integer indicating
// which group of outputs to porduce.
int writeOutputs(CFarsite * pFarsite, int outType, std::string outputPath, int f)
{
    int ret;
    if(outType == 0 || outType == 1 || outType == 4)
    {
        if(outType == 4)
        {
            pFarsite->WriteOneHours(outputPath.c_str());
            pFarsite->WriteTenHours(outputPath.c_str());
        }
        ret = pFarsite->WriteCrownFireGrid( (outputPath + "_CrownFire.asc").c_str());
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
        ret = pFarsite->WriteArrivalTimeGrid((outputPath + "_ArrivalTime.asc").c_str());
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
        ret = pFarsite->WriteArrivalTimeGridBinary( (outputPath + "_ArrivalTime.fbg").c_str());
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


// This function must be called from the main thread -- it is unsafe to have
// multiple versions running concurrently due to how the ICF class handles
// error messages and strings. I'm not sure why because it seems like it should
// be ok. A thread-safe ICF class would allow this to be run in a thread.
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


// Launch a farsite run. this can run asynchronously in its own thread.
// LoadCommandInputs must have been already called on the Farsite object.
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
    ret = writeOutputs(pFarsite, fc.outType, fc.outPath, f);
    if(ret != 1) 
    {
        printMsg(string("Error: Writing results failure for farsite #") + to_string(f+1) );
        return;  // throw?
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

	printMsg("Starting 'ProgressThread' thread.");
    auto progressThread = std::thread(ProgressThread, &pFarsites[0], nFarsites);
	int f;
           
	for(f = 0; f < nFarsites; f++)
	{
		pFarsites[f] = new CFarsite();
		
		if(!cancelRequest)
		{
            // Load inputs
            int ret = LoadCommandInputs(pFarsites[f], f, cmds[f]);
            if(ret != 1) 
            {
                //printMsg(string("Command File Error in farsite #") + to_string(f+1));
                char *a = pFarsites[f]->CommandFileError(ret);
                printError(cmds[f].input.c_str(),a);
                delete pFarsites[f];
                pFarsites[f] = nullptr;
            }

            // Launch simulation in own thread
			printMsg(string("Launching Farsite #") + to_string(f+1));
            FarsiteThreads.push_back(std::thread(LaunchFarsite, pFarsites[f], f, cmds[f]));
		}
		
	}
    
	bool complete = !cancelRequest;
	while (!FarsiteThreads.empty())
    {
        FarsiteThreads.back().join();
        FarsiteThreads.pop_back();
    }

    cancelRequest = true;
    progressThread.join();

	if(complete)
	{
		printMsg("Done\n");
	}
    
	for(int i = 0; i < nFarsites; i++)
	{
        if(pFarsites[f])
            delete pFarsites[f];
	}
	delete[] pFarsites;
    cmdfile.close();
	return 0;
}

// int linuxMain(int argc, char* argv[])
// {
//     char lcpFileName[MAX_PATH], inputsFileName[MAX_PATH], ignitName[MAX_PATH], barrierName[MAX_PATH], baseOutputsPath[MAX_PATH];
// 	if(argc < 2)
// 	{
// 		printf("TestFARSITE expects one parameter\nTestFARSITE Usage:\n"
// 			"TestFARSITE [commandfile]\n"
// 			"Where:\n\t[commandfile] is the path to the command file.\n");
// 		printf("The command file contains command lines for multiple Farsite runs, each run's command on a separate line.\n");
// 		printf("Each command expects six parameters, all required\n"
// 			"[LCPName] [InputsFileName] [IgnitionFileName] [BarrierFileName] [outputDirPath] [outputsType]\n"
// 			"Where:\n\t[LCPName] is the path to the Landscape File\n"
// 			"\t[InputsFileName] is the path to the FARSITE Inputs File (ASCII Format)\n"
// 			"\t[IgnitionFileName] is the path to the Ignition shape File\n"
// 			"\t[BarrierFileName] is the path to the Barrier shape File (0 if no barrier)\n"
// 			"\t[outputDirPath] is the path to the output files base name (no extension)\n"
// 			"\t[outputsType] is the file type for outputs (0 = both, 1 = ASCII grid, 2 = FlamMap binary grid\n\n");
// 		exit(1);
// 	}
// 	FILE *cmd = fopen(argv[1], "rt");
// 	if(!cmd)
// 	{
// 		printf("Error, cannot open %s\n", argv[1]);
// 		exit(1);
// 	}
//     char outputName[1512];
// 	int outType;// = atoi(argv[4]);
// 	int nFarsites = 0;
//     char buf[1512];
// 	while(!feof(cmd))
// 	{
//         fgets(buf, 1512, cmd);

// 		if(ParseCommands(buf, lcpFileName, inputsFileName, ignitName, barrierName, baseOutputsPath, &outType))//assume valid commands
// 			nFarsites++;
// 	}
// 	rewind(cmd);
// 	//SYSTEM_INFO sysInfo;
// 	//GetSystemInfo(&sysInfo);
// 	//nOmpFarsites =  sysInfo.dwNumberOfProcessors;

// 	//nOmpFarsites = nFarsites;
// 	char **lcps = new char *[nFarsites];
// 	char **inputs = new char *[nFarsites];
// 	char **ignits = new char *[nFarsites];
// 	char **barriers = new char *[nFarsites];
// 	char **outs = new char *[nFarsites];
// 	int *outTypes = new int[nFarsites];
// 	CFarsite **pFarsites = new CFarsite*[nFarsites];
// 	for(int i = 0; i < nFarsites; i++)
// 		pFarsites[i] = NULL;
// 	for(int i = 0; i < nFarsites; i++)
// 	{
// 		lcps[i] = new char[MAX_PATH];
// 		inputs[i] = new char[MAX_PATH];
// 		ignits[i] = new char[MAX_PATH];
// 		barriers[i] = new char[MAX_PATH];
// 		outs[i] = new char[MAX_PATH];
// 		strcpy(lcps[i], "");
// 		strcpy(inputs[i], "");
// 		strcpy(ignits[i], "");
// 		strcpy(barriers[i], "");
// 		strcpy(outs[i], "");
//         fgets(buf, 1512, cmd);
// 		if(!ParseCommands(buf, lcps[i], inputs[i], ignits[i], barriers[i], outs[i], &outTypes[i]))
// 			continue;
// 	}

// 	int ret;
// 	int f;
// 	for(f = 0; f < nFarsites; f++)
// 	{
//         pFarsites[f] = new CFarsite();
// 		if(!cancelRequest)
// 		{
// 			printf("Loading lcp file for Farsite #%d: %s\n", f + 1, lcps[f]);
// 			if ( !pFarsites[f]->SetLandscapeFile(lcps[f]))
// 			{
// 				printf ("Can't open: %s \n", lcpFileName);
// 				delete pFarsites[f];
// 				pFarsites[f] = nullptr;
// 				//_getch();
// 				continue;
// 			}
// 		}
// 		if(!cancelRequest)
// 		{
// 			printf("Loading inputs file for Farsite #%d: %s\n", f + 1, inputs[f]);
// 			ret = pFarsites[f]->LoadFarsiteInputs(inputs[f]);
// 			if ( ret != 1 )
// 			{
// 			   char *a = pFarsites[f]->CommandFileError(ret);
// 			   printf ("%s\n",a);
// 				delete pFarsites[f];
// 				pFarsites[f] = NULL;
// 				continue;
// 			}
// 		}
// 		if(!cancelRequest)
// 		{
// 			printf("Loading ignition file for Farsite #%d: %s\n", f + 1, ignits[f]);
// 			pFarsites[f]->SetIgnition(ignits[f]);
// 			if(strlen(barriers[f]) > 2)
// 			{
// 				printf("Loading barrier file for Farsite #%d: %s\n", f + 1, barriers[f]);
// 				pFarsites[f]->SetBarriers(barriers[f]);
// 			}
// 		}
// 		if(!cancelRequest)
// 		{
// 			printf("Launching Farsite #%d\n", f + 1);
// 			ret = pFarsites[f]->LaunchFarsite();
// 			if(ret != 1)
// 			{
// 			   char *a = pFarsites[f]->CommandFileError(ret);
// 			   printf ("%s\n",a);
// 				delete pFarsites[f];
// 				pFarsites[f] = NULL;
// 				continue;
// 			}
// 		}
// 		if(!cancelRequest)
// 		{
// 			printf("Writing outputs for Farsite #%d to %s\n", f + 1, outs[f]);
// 			if(outTypes[f] == 0 || outTypes[f] == 1 || outTypes[f] == 4)
// 			{
// 				if(outTypes[f] == 4)
// 				{
// 					pFarsites[f]->WriteOneHours(outs[f]);
// 					pFarsites[f]->WriteTenHours(outs[f]);
// 				}
// 				sprintf(outputName, "%s_CrownFire.asc", outs[f]);
// 				ret = pFarsites[f]->WriteCrownFireGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_Intensity.asc", outs[f]);
// 				ret = pFarsites[f]->WriteIntensityGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_FlameLength.asc", outs[f]);
// 				ret = pFarsites[f]->WriteFlameLengthGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_SpreadRate.asc", outs[f]);
// 				ret = pFarsites[f]->WriteSpreadRateGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_SpreadDirection.asc", outs[f]);
// 				ret = pFarsites[f]->WriteSpreadDirectionGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_HeatPerUnitArea.asc", outs[f]);
// 				ret = pFarsites[f]->WriteHeatPerUnitAreaGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_ReactionIntensity.asc", outs[f]);
// 				ret = pFarsites[f]->WriteReactionIntensityGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_ArrivalTime.asc", outs[f]);
// 				ret = pFarsites[f]->WriteArrivalTimeGrid(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 			}
// 			if(outTypes[f] == 0 || outTypes[f] == 2)
// 			{
// 				sprintf(outputName, "%s_CrownFire.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteCrownFireGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_ArrivalTime.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteArrivalTimeGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_Intensity.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteIntensityGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_FlameLength.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteFlameLengthGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_SpreadRate.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteSpreadRateGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_SpreadDirection.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteSpreadDirectionGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_HeatPerUnitArea.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteHeatPerUnitAreaGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_ReactionIntensity.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteReactionIntensityGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 				sprintf(outputName, "%s_ArrivalTime.fbg", outs[f]);
// 				ret = pFarsites[f]->WriteArrivalTimeGridBinary(outputName);
// 				if(ret != 1)
// 				{
// 					char *a = pFarsites[f]->CommandFileError(ret);
// 					printf ("%s for %s\n",a, outputName);
// 				}
// 			}
// 			/*sprintf(outputName, "%s_Moistures.txt", outs[f]);
// 			ret = pFarsites[f]->WriteMoistData(outputName);
// 			if(ret != 1)
// 			{
// 				char *a = pFarsites[f]->CommandFileError(ret);
// 				printf ("%s for %s\n",a, outputName);
// 			}*/
// 			sprintf(outputName, "%s_Ignitions.asc", outs[f]);
// 			ret = pFarsites[f]->WriteIgnitionGrid(outputName);
// 			if(ret != 1)
// 			{
// 				char *a = pFarsites[f]->CommandFileError(ret);
// 				printf ("%s for %s\n",a, outputName);
// 			}
// 			sprintf(outputName, "%s_Perimeters.shp", outs[f]);
// 			ret = pFarsites[f]->WritePerimetersShapeFile(outputName);
// 			if(ret != 1)
// 			{
// 				char *a = pFarsites[f]->CommandFileError(ret);
// 				printf ("%s for %s\n",a, outputName);
// 			}
// 			sprintf(outputName, "%s_SpotGrid.asc", outs[f]);
// 			ret = pFarsites[f]->WriteSpotGrid(outputName);
// 			if(ret != 1)
// 			{
// 				char *a = pFarsites[f]->CommandFileError(ret);
// 				printf ("%s for %s\n",a, outputName);
// 			}
// 			sprintf(outputName, "%s_Spots.csv", outs[f]);
// 			ret = pFarsites[f]->WriteSpotDataFile(outputName);
// 			if(ret != 1)
// 			{
// 				char *a = pFarsites[f]->CommandFileError(ret);
// 				printf ("%s for %s\n",a, outputName);
// 			}
// 			sprintf(outputName, "%s_Spots.shp", outs[f]);
// 			ret = pFarsites[f]->WriteSpotShapeFile(outputName);
// 			if(ret != 1)
// 			{
// 				char *a = pFarsites[f]->CommandFileError(ret);
// 				printf ("%s for %s\n",a, outputName);
// 			}
// 			sprintf(outputName, "%s_Timings.txt", outs[f]);
// 			ret = pFarsites[f]->WriteTimingsFile(outputName);
// 			if(ret != 1)
// 			{
// 				char *a = pFarsites[f]->CommandFileError(ret);
// 				printf ("%s for %s\n",a, outputName);
// 			}
// 		}
//         delete pFarsites[f];
//         pFarsites[f] = NULL;

// 	}
// 	fclose(cmd);
// 	delete[] pFarsites;
// 	for(int i = 0; i < nFarsites; i++)
// 	{
// 		delete[] lcps[i];
// 		delete[] inputs[i];
// 		delete[] outs[i];
// 		delete[] ignits[i];
// 		delete[] barriers[i];
// 	}
// 	delete[] lcps;
// 	delete[] inputs;
// 	delete[] outs;
// 	delete[] ignits;
// 	delete[] barriers;
// 	delete[] outTypes;
// 	return 0;
// }


// Call main function depending on platform
int main(int argc, char* argv[])
{
	MPMain(argc, argv);
//	linuxMain(argc, argv);
}
