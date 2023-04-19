// TestFARSITE.cpp : Defines the entry point for the console application.
//

#include "FARSITE.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>

#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>

// A mutex ensures orderly access to std::cout from multiple threads.
std::mutex iomutex;

using namespace std::chrono_literals;
using namespace std;

std::atomic_bool cancelRequest = false;

static const int MAX_PATH = 1512; // char buffer length for file paths

bool ParseCommands(char *in, char *lcp, char *inputs, char *ignitPath, char *barrierPath, char *outPath, int *outType)
{
	strcpy(lcp, "");
	strcpy(inputs, "");
	strcpy(outPath, "");
	*outType = 0;
	char seps[] = " ";
	char *p = strtok(in, seps);
	if(p)
		strcpy(lcp, p);
	else
		return false;
	p = strtok(NULL, seps);
	if(p)
		strcpy(inputs, p);
	else
		return false;
	p = strtok(NULL, seps);
	if(p)
		strcpy(ignitPath, p);
	else
		return false;
	p = strtok(NULL, seps);
	if(p)
		strcpy(barrierPath, p);
	else
		return false;
	p = strtok(NULL, seps);
	if(p)
		strcpy(outPath, p);
	else
		return false;
	p = strtok(NULL, seps);
	if(p)
		*outType = atoi(p);
	else
		return false;
	return true;
}


void printError(char *theerr,  char *fname)
{
    std::lock_guard<std::mutex> iolock(iomutex);
    printf("%s for %s\n", theerr, fname);
}



void ProgressThread(void *_pFarsite, int nFarsites)
{
 	CFarsite **pFarsite = (CFarsite **)_pFarsite;
	int progress = 0;
	while(!cancelRequest)
	{
//		printf("Progress on %d Farsites...\n", nFarsites);// Farsite %.2f percent complete.\n",
		for(int f = 0; f < nFarsites; f++)        
		{
            progress=0;
 			std::lock_guard<std::mutex> {iomutex};
            if(pFarsite[f]) progress = pFarsite[f]->GetFarsiteProgress();
            cout << "Farsite #" << f+1 << " " << progress << "% complete. " << std::flush;
		}
        iomutex.lock();
        cout << "\n" <<  std::flush;
        iomutex.unlock();
        std::this_thread::sleep_for(2000ms);
	}
}


// void buildFarsite(CFarsite *pFarsite, FarsiteRunInputs inputs)
// {

// }

int writeOutputs(CFarsite * pFarsite, int outType, char *outs, int f)
{
    int ret;
    char outputName[MAX_PATH];
    
    iomutex.lock();
    cout << "Writing outputs for Farsite #" << f+1 << ": " << outs << "\n" << std::flush;
    iomutex.unlock();
    if(outType == 0 || outType == 1 || outType == 4)
    {
        if(outType == 4)
        {
            pFarsite->WriteOneHours(outs);
            pFarsite->WriteTenHours(outs);
        }
        sprintf(outputName, "%s_CrownFire.asc", outs);
        ret = pFarsite->WriteCrownFireGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_Intensity.asc", outs);
        ret = pFarsite->WriteIntensityGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_FlameLength.asc", outs);
        ret = pFarsite->WriteFlameLengthGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_SpreadRate.asc", outs);
        ret = pFarsite->WriteSpreadRateGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_SpreadDirection.asc", outs);
        ret = pFarsite->WriteSpreadDirectionGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_HeatPerUnitArea.asc", outs);
        ret = pFarsite->WriteHeatPerUnitAreaGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_ReactionIntensity.asc", outs);
        ret = pFarsite->WriteReactionIntensityGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_ArrivalTime.asc", outs);
        ret = pFarsite->WriteArrivalTimeGrid(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
    }
    if(outType == 0 || outType == 2)
    {
        sprintf(outputName, "%s_CrownFire.fbg", outs);
        ret = pFarsite->WriteCrownFireGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_ArrivalTime.fbg", outs);
        ret = pFarsite->WriteArrivalTimeGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_Intensity.fbg", outs);
        ret = pFarsite->WriteIntensityGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_FlameLength.fbg", outs);
        ret = pFarsite->WriteFlameLengthGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_SpreadRate.fbg", outs);
        ret = pFarsite->WriteSpreadRateGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_SpreadDirection.fbg", outs);
        ret = pFarsite->WriteSpreadDirectionGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_HeatPerUnitArea.fbg", outs);
        ret = pFarsite->WriteHeatPerUnitAreaGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_ReactionIntensity.fbg", outs);
        ret = pFarsite->WriteReactionIntensityGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
        sprintf(outputName, "%s_ArrivalTime.fbg", outs);
        ret = pFarsite->WriteArrivalTimeGridBinary(outputName);
        if(ret != 1)
        {
            char *a = pFarsite->CommandFileError(ret);
            printError(a, outputName);
        }
    }
    /*sprintf(outputName, "%s_Moistures.txt", outs[f]);
      ret = pFarsite->WriteMoistData(outputName);
      if(ret != 1)
      {
      char *a = pFarsite->CommandFileError(ret);
      printError(a, outputName);
      }*/
    sprintf(outputName, "%s_Ignitions.asc", outs);
    ret = pFarsite->WriteIgnitionGrid(outputName);
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputName);
    }
    sprintf(outputName, "%s_Perimeters.shp", outs);
    ret = pFarsite->WritePerimetersShapeFile(outputName);
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputName);
    }
    sprintf(outputName, "%s_SpotGrid.asc", outs);
    ret = pFarsite->WriteSpotGrid(outputName);
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputName);
    }
    sprintf(outputName, "%s_Spots.csv", outs);
    ret = pFarsite->WriteSpotDataFile(outputName);
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputName);
    }
    sprintf(outputName, "%s_Spots.shp", outs);
    ret = pFarsite->WriteSpotShapeFile(outputName);
    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputName);
    }
    sprintf(outputName, "%s_Timings.txt", outs);
    ret = pFarsite->WriteTimingsFile(outputName);

    if(ret != 1)
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(a, outputName);
    }
    
return ret;
}

// For handing to std:thread() return ignored
int LaunchFarsite(void *_pFarsite, int outType, char *outs, int f)
{
  	CFarsite *pFarsite = (CFarsite *)_pFarsite;
    
    int	ret = pFarsite->LaunchFarsite();
    if(ret != 1) 
    {
        char *a = pFarsite->CommandFileError(ret);
        printError(outs,a);
        return ret;
    }

    ret = writeOutputs(pFarsite, outType, outs, f);
    return ret;
}

int MPMain(int argc, char* argv[])
{
	char lcpFileName[MAX_PATH], inputsFileName[MAX_PATH], ignitName[MAX_PATH], barrierName[MAX_PATH], baseOutputsPath[MAX_PATH];
	if(argc < 2)
	{
		printf("TestFARSITE expects one parameter\nTestFARSITE Usage:\n"
			"TestFARSITE [commandfile]\n"
			"Where:\n\t[commandfile] is the path to the command file.\n");
		printf("The command file contains command lines for multiple Farsite runs, each run's command on a separate line.\n");
		printf("Each command expects six parameters, all required\n"
			"[LCPName] [InputsFileName] [IgnitionFileName] [BarrierFileName] [outputDirPath] [outputsType]\n"
			"Where:\n\t[LCPName] is the path to the Landscape File\n"
			"\t[InputsFileName] is the path to the FARSITE Inputs File (ASCII Format)\n"
			"\t[IgnitionFileName] is the path to the Ignition shape File\n"
			"\t[BarrierFileName] is the path to the Barrier shape File (0 if no barrier)\n"
			"\t[outputDirPath] is the path to the output files base name (no extension)\n"
			"\t[outputsType] is the file type for outputs (0 = both, 1 = ASCII grid, 2 = FlamMap binary grid\n\n");
		exit(1);
	}
	FILE *cmd = fopen(argv[1], "rt");
	if(!cmd)
	{
		printf("Error, cannot open %s\n", argv[1]);
		exit(1);
	}

	int outType;// = atoi(argv[4]);
	int nFarsites = 0;
	char buf[MAX_PATH];
	while(!feof(cmd))
	{
		fgets(buf, MAX_PATH, cmd);

		if(ParseCommands(buf, lcpFileName, inputsFileName, ignitName, barrierName, baseOutputsPath, &outType))//assume valid commands
			nFarsites++;
	}
	rewind(cmd);

    
	char **lcps = new char *[nFarsites];
	char **inputs = new char *[nFarsites];
	char **ignits = new char *[nFarsites];
	char **barriers = new char *[nFarsites];
	char **outs = new char *[nFarsites];
	int *outTypes = new int[nFarsites];
    std::vector<std::thread> FarsiteThreads;
	CFarsite **pFarsites = new CFarsite*[nFarsites];
	for(int i = 0; i < nFarsites; i++)
		pFarsites[i] = nullptr;
	for(int i = 0; i < nFarsites; i++)
	{
		lcps[i] = new char[MAX_PATH];
		inputs[i] = new char[MAX_PATH];
		ignits[i] = new char[MAX_PATH];
		barriers[i] = new char[MAX_PATH];
		outs[i] = new char[MAX_PATH];
		strcpy(lcps[i], "");
		strcpy(inputs[i], "");
		strcpy(ignits[i], "");
		strcpy(barriers[i], "");
		strcpy(outs[i], "");
		fgets(buf, MAX_PATH, cmd);
		if(!ParseCommands(buf, lcps[i], inputs[i], ignits[i], barriers[i], outs[i], &outTypes[i]))
			continue;
	}

	printf("Starting 'ProgressThread' thread.\n");
    auto progressThread = std::thread(ProgressThread, &pFarsites[0], nFarsites);
	int ret;
	int f;
    
//    auto waitTime =  1000ms;
        
	for(f = 0; f < nFarsites; f++)
	{
		{
//			printf("Sleeping 1000 ms for iteration %d\n", f);
//			std::this_thread::sleep_for(waitTime);
			pFarsites[f] = new CFarsite();
		}
		if(!cancelRequest)
		{
			printf("Loading lcp file for Farsite #%d: %s\n", f + 1, lcps[f]);
			if ( !pFarsites[f]->SetLandscapeFile(lcps[f]))
			{
				printf ("Can't open: %s \n", lcpFileName);
				delete pFarsites[f];
				pFarsites[f] = NULL;
				//_getch();
				continue;
			}
		}
		if(!cancelRequest)
		{
			printf("Loading inputs file for Farsite #%d: %s\n", f + 1, inputs[f]);
			ret = pFarsites[f]->LoadFarsiteInputs(inputs[f]);
			if ( ret != 1 )
			{
                char *a = pFarsites[f]->CommandFileError(ret);
                printf ("%s\n",a);
                delete pFarsites[f];
				pFarsites[f] = nullptr;
				continue;
			}
		}
		if(!cancelRequest)
		{
			printf("Loading ignition file for Farsite #%d: %s\n", f + 1, ignits[f]);
			pFarsites[f]->SetIgnition(ignits[f]);
			if(strlen(barriers[f]) > 2)
			{
				printf("Loading barrier file for Farsite #%d: %s\n", f + 1, barriers[f]);
				pFarsites[f]->SetBarriers(barriers[f]);
			}
		}
		if(!cancelRequest)
		{
			printf("Launching Farsite #%d\n", f + 1);
            FarsiteThreads.push_back(std::thread(LaunchFarsite, pFarsites[f], outTypes[f], outs[f], f));
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
		printf("Done\n");
	}

	fclose(cmd);
	delete[] pFarsites;
	for(int i = 0; i < nFarsites; i++)
	{
		delete[] lcps[i];
		delete[] inputs[i];
		delete[] outs[i];
		delete[] ignits[i];
		delete[] barriers[i];
	}
	delete[] lcps;
	delete[] inputs;
	delete[] outs;
	delete[] ignits;
	delete[] barriers;
	delete[] outTypes;
	return 0;
}








int linuxMain(int argc, char* argv[])
{
    char lcpFileName[MAX_PATH], inputsFileName[MAX_PATH], ignitName[MAX_PATH], barrierName[MAX_PATH], baseOutputsPath[MAX_PATH];
	if(argc < 2)
	{
		printf("TestFARSITE expects one parameter\nTestFARSITE Usage:\n"
			"TestFARSITE [commandfile]\n"
			"Where:\n\t[commandfile] is the path to the command file.\n");
		printf("The command file contains command lines for multiple Farsite runs, each run's command on a separate line.\n");
		printf("Each command expects six parameters, all required\n"
			"[LCPName] [InputsFileName] [IgnitionFileName] [BarrierFileName] [outputDirPath] [outputsType]\n"
			"Where:\n\t[LCPName] is the path to the Landscape File\n"
			"\t[InputsFileName] is the path to the FARSITE Inputs File (ASCII Format)\n"
			"\t[IgnitionFileName] is the path to the Ignition shape File\n"
			"\t[BarrierFileName] is the path to the Barrier shape File (0 if no barrier)\n"
			"\t[outputDirPath] is the path to the output files base name (no extension)\n"
			"\t[outputsType] is the file type for outputs (0 = both, 1 = ASCII grid, 2 = FlamMap binary grid\n\n");
		exit(1);
	}
	FILE *cmd = fopen(argv[1], "rt");
	if(!cmd)
	{
		printf("Error, cannot open %s\n", argv[1]);
		exit(1);
	}
    char outputName[1512];
	int outType;// = atoi(argv[4]);
	int nFarsites = 0;
    char buf[1512];
	while(!feof(cmd))
	{
        fgets(buf, 1512, cmd);

		if(ParseCommands(buf, lcpFileName, inputsFileName, ignitName, barrierName, baseOutputsPath, &outType))//assume valid commands
			nFarsites++;
	}
	rewind(cmd);
	//SYSTEM_INFO sysInfo;
	//GetSystemInfo(&sysInfo);
	//nOmpFarsites =  sysInfo.dwNumberOfProcessors;

	//nOmpFarsites = nFarsites;
	char **lcps = new char *[nFarsites];
	char **inputs = new char *[nFarsites];
	char **ignits = new char *[nFarsites];
	char **barriers = new char *[nFarsites];
	char **outs = new char *[nFarsites];
	int *outTypes = new int[nFarsites];
	CFarsite **pFarsites = new CFarsite*[nFarsites];
	for(int i = 0; i < nFarsites; i++)
		pFarsites[i] = NULL;
	for(int i = 0; i < nFarsites; i++)
	{
		lcps[i] = new char[MAX_PATH];
		inputs[i] = new char[MAX_PATH];
		ignits[i] = new char[MAX_PATH];
		barriers[i] = new char[MAX_PATH];
		outs[i] = new char[MAX_PATH];
		strcpy(lcps[i], "");
		strcpy(inputs[i], "");
		strcpy(ignits[i], "");
		strcpy(barriers[i], "");
		strcpy(outs[i], "");
        fgets(buf, 1512, cmd);
		if(!ParseCommands(buf, lcps[i], inputs[i], ignits[i], barriers[i], outs[i], &outTypes[i]))
			continue;
	}

	int ret;
	int f;
	for(f = 0; f < nFarsites; f++)
	{
        pFarsites[f] = new CFarsite();
		if(!cancelRequest)
		{
			printf("Loading lcp file for Farsite #%d: %s\n", f + 1, lcps[f]);
			if ( !pFarsites[f]->SetLandscapeFile(lcps[f]))
			{
				printf ("Can't open: %s \n", lcpFileName);
				delete pFarsites[f];
				pFarsites[f] = NULL;
				//_getch();
				continue;
			}
		}
		if(!cancelRequest)
		{
			printf("Loading inputs file for Farsite #%d: %s\n", f + 1, inputs[f]);
			ret = pFarsites[f]->LoadFarsiteInputs(inputs[f]);
			if ( ret != 1 )
			{
			   char *a = pFarsites[f]->CommandFileError(ret);
			   printf ("%s\n",a);
				delete pFarsites[f];
				pFarsites[f] = NULL;
				continue;
			}
		}
		if(!cancelRequest)
		{
			printf("Loading ignition file for Farsite #%d: %s\n", f + 1, ignits[f]);
			pFarsites[f]->SetIgnition(ignits[f]);
			if(strlen(barriers[f]) > 2)
			{
				printf("Loading barrier file for Farsite #%d: %s\n", f + 1, barriers[f]);
				pFarsites[f]->SetBarriers(barriers[f]);
			}
		}
		if(!cancelRequest)
		{
			printf("Launching Farsite #%d\n", f + 1);
			ret = pFarsites[f]->LaunchFarsite();
			if(ret != 1)
			{
			   char *a = pFarsites[f]->CommandFileError(ret);
			   printf ("%s\n",a);
				delete pFarsites[f];
				pFarsites[f] = NULL;
				continue;
			}
		}
		if(!cancelRequest)
		{
			printf("Writing outputs for Farsite #%d to %s\n", f + 1, outs[f]);
			if(outTypes[f] == 0 || outTypes[f] == 1 || outTypes[f] == 4)
			{
				if(outTypes[f] == 4)
				{
					pFarsites[f]->WriteOneHours(outs[f]);
					pFarsites[f]->WriteTenHours(outs[f]);
				}
				sprintf(outputName, "%s_CrownFire.asc", outs[f]);
				ret = pFarsites[f]->WriteCrownFireGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_Intensity.asc", outs[f]);
				ret = pFarsites[f]->WriteIntensityGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_FlameLength.asc", outs[f]);
				ret = pFarsites[f]->WriteFlameLengthGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_SpreadRate.asc", outs[f]);
				ret = pFarsites[f]->WriteSpreadRateGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_SpreadDirection.asc", outs[f]);
				ret = pFarsites[f]->WriteSpreadDirectionGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_HeatPerUnitArea.asc", outs[f]);
				ret = pFarsites[f]->WriteHeatPerUnitAreaGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_ReactionIntensity.asc", outs[f]);
				ret = pFarsites[f]->WriteReactionIntensityGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_ArrivalTime.asc", outs[f]);
				ret = pFarsites[f]->WriteArrivalTimeGrid(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
			}
			if(outTypes[f] == 0 || outTypes[f] == 2)
			{
				sprintf(outputName, "%s_CrownFire.fbg", outs[f]);
				ret = pFarsites[f]->WriteCrownFireGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_ArrivalTime.fbg", outs[f]);
				ret = pFarsites[f]->WriteArrivalTimeGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_Intensity.fbg", outs[f]);
				ret = pFarsites[f]->WriteIntensityGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_FlameLength.fbg", outs[f]);
				ret = pFarsites[f]->WriteFlameLengthGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_SpreadRate.fbg", outs[f]);
				ret = pFarsites[f]->WriteSpreadRateGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_SpreadDirection.fbg", outs[f]);
				ret = pFarsites[f]->WriteSpreadDirectionGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_HeatPerUnitArea.fbg", outs[f]);
				ret = pFarsites[f]->WriteHeatPerUnitAreaGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_ReactionIntensity.fbg", outs[f]);
				ret = pFarsites[f]->WriteReactionIntensityGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
				sprintf(outputName, "%s_ArrivalTime.fbg", outs[f]);
				ret = pFarsites[f]->WriteArrivalTimeGridBinary(outputName);
				if(ret != 1)
				{
					char *a = pFarsites[f]->CommandFileError(ret);
					printf ("%s for %s\n",a, outputName);
				}
			}
			/*sprintf(outputName, "%s_Moistures.txt", outs[f]);
			ret = pFarsites[f]->WriteMoistData(outputName);
			if(ret != 1)
			{
				char *a = pFarsites[f]->CommandFileError(ret);
				printf ("%s for %s\n",a, outputName);
			}*/
			sprintf(outputName, "%s_Ignitions.asc", outs[f]);
			ret = pFarsites[f]->WriteIgnitionGrid(outputName);
			if(ret != 1)
			{
				char *a = pFarsites[f]->CommandFileError(ret);
				printf ("%s for %s\n",a, outputName);
			}
			sprintf(outputName, "%s_Perimeters.shp", outs[f]);
			ret = pFarsites[f]->WritePerimetersShapeFile(outputName);
			if(ret != 1)
			{
				char *a = pFarsites[f]->CommandFileError(ret);
				printf ("%s for %s\n",a, outputName);
			}
			sprintf(outputName, "%s_SpotGrid.asc", outs[f]);
			ret = pFarsites[f]->WriteSpotGrid(outputName);
			if(ret != 1)
			{
				char *a = pFarsites[f]->CommandFileError(ret);
				printf ("%s for %s\n",a, outputName);
			}
			sprintf(outputName, "%s_Spots.csv", outs[f]);
			ret = pFarsites[f]->WriteSpotDataFile(outputName);
			if(ret != 1)
			{
				char *a = pFarsites[f]->CommandFileError(ret);
				printf ("%s for %s\n",a, outputName);
			}
			sprintf(outputName, "%s_Spots.shp", outs[f]);
			ret = pFarsites[f]->WriteSpotShapeFile(outputName);
			if(ret != 1)
			{
				char *a = pFarsites[f]->CommandFileError(ret);
				printf ("%s for %s\n",a, outputName);
			}
			sprintf(outputName, "%s_Timings.txt", outs[f]);
			ret = pFarsites[f]->WriteTimingsFile(outputName);
			if(ret != 1)
			{
				char *a = pFarsites[f]->CommandFileError(ret);
				printf ("%s for %s\n",a, outputName);
			}
		}
        delete pFarsites[f];
        pFarsites[f] = NULL;

	}
	fclose(cmd);
	delete[] pFarsites;
	for(int i = 0; i < nFarsites; i++)
	{
		delete[] lcps[i];
		delete[] inputs[i];
		delete[] outs[i];
		delete[] ignits[i];
		delete[] barriers[i];
	}
	delete[] lcps;
	delete[] inputs;
	delete[] outs;
	delete[] ignits;
	delete[] barriers;
	delete[] outTypes;
	return 0;
}


// Call main function depending on platform
int main(int argc, char* argv[])
{

	MPMain(argc, argv);
//	linuxMain(argc, argv);
}
