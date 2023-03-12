// TestFARSITE.cpp : Defines the entry point for the console application.
//

#include "FARSITE.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctime>
bool cancelRequest = false;
int nFarsites = 0;

#ifdef WIN32
#include <sys/time.h>
#include <direct.h>
#include <conio.h>
#include <process.h>
#include <windows.h>



unsigned int __stdcall  ProgressThread(void *_pFarsite)
{
 	CFarsite *pFarsite = (CFarsite *)_pFarsite;
	//Sleep(1000);
	double progress = 0.0;
	while(!cancelRequest && (progress = pFarsite->GetFarsiteProgress()) < 1.0)
	{
		printf("ProgressThread (10 second notification): Farsite %.2f percent complete.\n",
			progress * 100.0);
		Sleep(10000);
	}
	//Sleep(1000);
	return 1;
}


unsigned int __stdcall  CheckKey( void *_pFarsite )
{
 	CFarsite *pFarsite = (CFarsite *)_pFarsite;
   if(_getch() != 0)
   {
		printf("Farsite terminate request....\n");
		pFarsite->CancelFarsite();    /* _endthread implied */
		cancelRequest = true;
   }
   return 1;
}

unsigned int __stdcall  MultiCheckKey( void *_pFarsite )
{
 	CFarsite *pFarsite = (CFarsite *)_pFarsite;
   if(_getch() != 0)
   {
		printf("Farsite terminate request....\n");
		for(int f = 0; f < nFarsites; f++)
			pFarsite[f].CancelFarsite();    /* _endthread implied */
		cancelRequest = true;
   }
   return 1;
}

unsigned int __stdcall RunFarsiteProc(void *_pFarsite)
{
	CFarsite *pFarsite = (CFarsite *)_pFarsite;
	int ret = pFarsite->LaunchFarsite();
	if(ret != 1)
	{
	   char *a = pFarsite->CommandFileError(ret);
	   printf ("%s\n",a);
	   cancelRequest = true;
	}
	return ret;
}
#else
#include <stdio.h>
#include <stdlib.h>

#endif
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

#ifdef WIN32
int nOmpFarsites = 0;
unsigned int __stdcall  ompProgressThread(void *_pFarsite)
{
 	CFarsite **pFarsite = (CFarsite **)_pFarsite;
	//Sleep(1000);
	double progress = 0.0;
	while(!cancelRequest)// && (progress = pFarsite->GetFarsiteProgress()) < 1.0)
	{
		printf("Progress...\n");// Farsite %.2f percent complete.\n",
		for(int f = 0; f < nOmpFarsites; f++)
		{
			if(pFarsite[f])
			{
				progress = pFarsite[f]->GetFarsiteProgress();
				//if(progress > 0)
					printf("Farsite %d: %.3f complete\n",// Farsite %.2f percent complete.\n",
						f + 1, progress * 100.0);
			}
		}
		Sleep(5000);
	}
	//Sleep(100);
	return 1;
}

unsigned int __stdcall  ompCheckKey( void *_pFarsite )
{
 	CFarsite **pFarsite = (CFarsite **)_pFarsite;
   if(_getch() != 0)
   {
		printf("Farsite terminate request....\n");
		for(int f = 0; f < nOmpFarsites; f++)
		{
			if(pFarsite[f])
				pFarsite[f]->CancelFarsite();    /* _endthread implied */
		}
		cancelRequest = true;
   }
   return 1;
}

typedef CFarsite * LPFARSITE;

int openMPMain(int argc, char* argv[])
{
	char lcpFileName[512], inputsFileName[512], ignitName[512], barrierName[512], baseOutputsPath[512];
	if(argc < 2)
	{
		printf("TestFARSITE expects one parameter\nTestFARSITE Usage:\n"
			"TestFARSITE [commandfile]\n"
			"Where:\n\t[commandfile] is the path to the command file.\n");
		printf("The command file contains command lines for multiple Farsite runs, each run's command on a seperate line.\n");
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
	char outputName[512];
	int outType;// = atoi(argv[4]);
	nFarsites = 0;
	char buf[512];
	while(!feof(cmd))
	{
		fgets(buf, 512, cmd);

		if(ParseCommands(buf, lcpFileName, inputsFileName, ignitName, barrierName, baseOutputsPath, &outType))//assume valid commands
			nFarsites++;
	}
	rewind(cmd);
	//SYSTEM_INFO sysInfo;
	//GetSystemInfo(&sysInfo);
	//nOmpFarsites =  sysInfo.dwNumberOfProcessors;

	nOmpFarsites = nFarsites;
	char **lcps = new char *[nFarsites];
	char **inputs = new char *[nFarsites];
	char **ignits = new char *[nFarsites];
	char **barriers = new char *[nFarsites];
	char **outs = new char *[nFarsites];
	int *outTypes = new int[nFarsites];
	CFarsite **pFarsites = new CFarsite*[nOmpFarsites];
	for(int i = 0; i < nOmpFarsites; i++)
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
		fgets(buf, 512, cmd);
		if(!ParseCommands(buf, lcps[i], inputs[i], ignits[i], barriers[i], outs[i], &outTypes[i]))
			continue;
	}

	HANDLE threadHandle[1];
	HANDLE checkKeyHandles[1];
	unsigned int ThreadID1, ThreadID2;
	printf("Starting 'CheckKey' thread.\n");
	checkKeyHandles[0] =(HANDLE) ::_beginthreadex(NULL, 0, ompCheckKey, (void *) pFarsites, NULL, &ThreadID1);
	printf("Starting 'ProgressThread' thread.\n");
    threadHandle[0] =(HANDLE) ::_beginthreadex(NULL, 0, ompProgressThread, &pFarsites[0], NULL, &ThreadID2);
	int ret;
	int f, waitTime;

#pragma omp parallel for default(shared) private (buf, ret, outputName, waitTime) schedule (dynamic, 1)
	for(f = 0; f < nFarsites; f++)
	{
		#pragma omp critical
		{
			waitTime = f;
			printf("Sleeping %d ms for iteration %d\n", waitTime*f, f);
			Sleep(waitTime*f);
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
		#pragma omp critical
		{
			CFarsite *tFarsite = pFarsites[f];
			pFarsites[f] = NULL;
			delete tFarsite;
		}

	}
	bool complete = !cancelRequest;
	cancelRequest = true;
	WaitForMultipleObjects(1, threadHandle, TRUE, INFINITE);
	if(complete)
	{
		printf("Press any key to exit.\n");
		WaitForMultipleObjects(1, checkKeyHandles, TRUE, INFINITE);

	}
	CloseHandle(checkKeyHandles[0]);
	CloseHandle(threadHandle[0]);
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
#else
#define MAX_PATH 1512 // char buffer length for file paths
int linuxMain(int argc, char* argv[])
{
    char lcpFileName[MAX_PATH], inputsFileName[MAX_PATH], ignitName[MAX_PATH], barrierName[MAX_PATH], baseOutputsPath[MAX_PATH];
	if(argc < 2)
	{
		printf("TestFARSITE expects one parameter\nTestFARSITE Usage:\n"
			"TestFARSITE [commandfile]\n"
			"Where:\n\t[commandfile] is the path to the command file.\n");
		printf("The command file contains command lines for multiple Farsite runs, each run's command on a seperate line.\n");
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
	nFarsites = 0;
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
#endif


// Call main function depending on platform
int main(int argc, char* argv[])
{
	//regMain(argc, argv);
	//multiMain(argc, argv);
#ifdef WIN32
	openMPMain(argc, argv);
#else
	linuxMain(argc, argv);
#endif
}
