// FARSITE.cpp

// Authors? Date?

// Defines the exported functions for the main application level
// interface clsss, CFarsite//

#include "FARSITE.h"
#include "Farsite5.h" // implementation of actual spread model, used as
                      // contained member `m_pFarsite`

#include <iostream>
#include <sstream>

std::mutex iomutex;

// helper function for error logging. Need better solution
void CFarsite::printError(const char *theerr,  const char *fname)
{
    std::lock_guard<std::mutex> iolock(iomutex);
    std::cerr << "Error: " << theerr << " for " << fname << "\n";
}


CFarsite::CFarsite()
{
	m_pFarsite = new Farsite5();
}

CFarsite::~CFarsite()
{
	if(m_pFarsite)
		delete m_pFarsite;
	m_pFarsite = nullptr;
}

int CFarsite::SetLandscapeFile(const char *_lcpFileName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->LoadLandscapeFile(_lcpFileName);
	}
	return 0;
}

int CFarsite::SetIgnition(const char *shapeFileName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->SetIgnitionFileName(shapeFileName);
	}
	return 0;
}

int CFarsite::SetBarriers(const char *shapeFileName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->SetBarrierFileName(shapeFileName);
	}
	return 0;
}

int CFarsite::SetNumProcessors(int numThreads)
{
	return 0;
}

int CFarsite::SetStartProcessor(int _procNum)
{
	return 0;
}

int CFarsite::GetStartProcessor()
{
	return 0;
}

int CFarsite::LoadFarsiteInputs(const char *_inputFile)
{
	if(m_pFarsite)
		return m_pFarsite->LoadInputsFile(_inputFile);
	return 0;
}

char *CFarsite::CommandFileError(int i_ErrNum)
{
	if(m_pFarsite)
		return m_pFarsite->LoadInputError(i_ErrNum);

	return 0;
}


char *CFarsite::GetErrorMessage(int errNum)
{
   return this->CommandFileError(errNum);
}


double CFarsite::GetResolution()
{
	if(m_pFarsite)
		return m_pFarsite->GetCellResolutionX();
	return 0;
}

long CFarsite::GetNumCols()
{
	if(m_pFarsite)
		return m_pFarsite->Header.numeast;
	return 0;
}

long CFarsite::GetNumRows()
{
	if(m_pFarsite)
		return m_pFarsite->Header.numnorth;
	return 0;
}

double CFarsite::GetWest()
{
	if(m_pFarsite)
		return m_pFarsite->Header.WestUtm;
	return 0;
}

double CFarsite::GetSouth()
{
	if(m_pFarsite)
		return m_pFarsite->Header.SouthUtm;
	return 0;
}

double CFarsite::GetEast()
{
	if(m_pFarsite)
		return m_pFarsite->Header.EastUtm;
	return 0;
}

double CFarsite::GetNorth()
{
	if(m_pFarsite)
		return m_pFarsite->Header.NorthUtm;
	return 0;
}

int CFarsite::CanLaunchFarsite(void)
{
	return 0;
}


/**********************************************************************************
*
* Ret: 0 OK, < 0 Error number,
*      use  CFarsite.GetErrorMessage(errnum) to get error message text
**********************************************************************************/
int CFarsite::LaunchFarsite(void)
{
	if(m_pFarsite)
		return m_pFarsite->LaunchFarsite();
	return 0;
}

/****************************************************************************/
int CFarsite::GetFarsiteProgress() const
{
	if (m_pFarsite)
    return m_pFarsite->GetFarsiteProgress();
	return 0;
}

const char* CFarsite::GetFarsiteStatusString() const
{
	if (m_pFarsite)
    return m_pFarsite->GetFarsiteStatusString();
	return "";
}


//#ifdef Larry-Out
/*****************************************************************************/
/*int  CFarsite::GetFarsiteStatus(char cr[])
{
 strcpy (cr,"");
 if (m_pFarsite) {
//   return m_pFarsite->GetFarsiteRunStatus(cr);
   strcpy (cr,"Fix - CFarsite::GetFarsiteStatus()");

 }
 return 0;
}*/
//#endif

int CFarsite::CancelFarsite(void)
{
	if(m_pFarsite)
	{
		return m_pFarsite->CancelFarsite();
	}
	return 0;
}

int CFarsite::WriteArrivalTimeGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteArrivalTimeGrid(trgName);
	}
	return 0;
}

int CFarsite::WriteIntensityGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteIntensityGrid(trgName);
	}
	return 0;
}

int CFarsite::WriteFlameLengthGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteFlameLengthGrid(trgName);
	}
	return 0;
}

int CFarsite::WriteSpreadRateGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteSpreadRateGrid(trgName);
	}
	return 0;
}

int CFarsite::WriteSpreadDirectionGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteSpreadDirectionGrid(trgName);
	}
	return 0;
}

int CFarsite::WriteHeatPerUnitAreaGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteHeatPerUnitAreaGrid(trgName);
	}
	return 0;
}

int CFarsite::WriteReactionIntensityGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteReactionIntensityGrid(trgName);
	}
	return 0;
}

int CFarsite::WriteCrownFireGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteCrownFireGrid(trgName);
	}
	return 0;
}


int CFarsite::WriteArrivalTimeGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteArrivalTimeGridBinary(trgName);
	}
	return 0;
}


int CFarsite::WriteIntensityGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteIntensityGridBinary(trgName);
	}
	return 0;
}


int CFarsite::WriteFlameLengthGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteFlameLengthGridBinary(trgName);
	}
	return 0;
}
int CFarsite::WriteSpreadRateGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteSpreadRateGridBinary(trgName);
	}
	return 0;
}
int CFarsite::WriteSpreadDirectionGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteSpreadDirectionGridBinary(trgName);
	}
	return 0;
}
int CFarsite::WriteHeatPerUnitAreaGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteHeatPerUnitAreaGridBinary(trgName);
	}
	return 0;
}
int CFarsite::WriteReactionIntensityGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteReactionIntensityGridBinary(trgName);
	}
	return 0;
}
int CFarsite::WriteCrownFireGridBinary(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteCrownFireGridBinary(trgName);
	}
	return 0;
}

int CFarsite::WriteIgnitionGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteIgnitionGrid(trgName);
	}
	return 0;
}

int CFarsite::WritePerimetersShapeFile(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WritePerimetersShapeFile(trgName);
	}
	return 0;
}

int CFarsite::WriteTimingsFile(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteTimingsFile(trgName);
	}
	return 0;
}

int CFarsite::WriteSpotGrid(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteSpotGrid(trgName);
	}
	return 0;
}


/***********************************************************************************/
int CFarsite::WriteOneHours(const char *trgName)
{
	if (m_pFarsite)	{
		 m_pFarsite->LastAccess = -1;
		 char mapName[256];
	 	for (double maptime = 0.0; maptime <= m_pFarsite->burn.SIMTIME; maptime += 60.0)	{
			   if (m_pFarsite->InquireInBurnPeriod(maptime))	{
	   			long mo, dy, hr, mn;
	 	   m_pFarsite->ConvertSimtimeToActualTime(maptime, &mo, &dy, &hr, &mn, false);
			  	sprintf(mapName, "%s_%02ld%02ld_%02ld%02ld_FM1.asc", trgName, mo, dy, hr/100, mn);
			  	m_pFarsite->CmMapEnvironment(0, maptime, mapName);	}
		}
		return 1;
	}
	return 0;
}

int CFarsite::WriteTenHours(const char *trgName)
{
	if(m_pFarsite)
	{
		m_pFarsite->LastAccess = -1;
		char mapName[256];
		for(double maptime = 0.0; maptime <= m_pFarsite->burn.SIMTIME; maptime += 60.0)
		{
			if(m_pFarsite->InquireInBurnPeriod(maptime))
			{
				long mo, dy, hr, mn;
				m_pFarsite->ConvertSimtimeToActualTime(maptime, &mo, &dy, &hr, &mn, false);
				sprintf(mapName, "%s_%02ld%02ld_%02ld%02ld_FM10.asc", trgName, mo, dy, hr/100, mn);
				m_pFarsite->CmMapEnvironment(1, maptime, mapName);
			}
		}
		return 1;
	}
	return 0;
}
int CFarsite::WriteHundredHours(const char *trgName)
{
	if(m_pFarsite)
	{
		m_pFarsite->LastAccess = -1;
		char mapName[256];
		for(double maptime = 0.0; maptime <= m_pFarsite->burn.SIMTIME; maptime += 60.0)
		{
			if(m_pFarsite->InquireInBurnPeriod(maptime))
			{
				long mo, dy, hr, mn;
				m_pFarsite->ConvertSimtimeToActualTime(maptime, &mo, &dy, &hr, &mn, false);
				sprintf(mapName, "%s_%02ld%02ld_%02ld%02ld_FM100.asc", trgName, mo, dy, hr/100, mn);
				m_pFarsite->CmMapEnvironment(2, maptime, mapName);
			}
		}
		return 1;
	}
	return 0;
}

int CFarsite::WriteMoistData(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->ExportMoistureDataText(trgName);
	}
	return 0;
}

int CFarsite::WriteSpotDataFile(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteSpotDataFile(trgName);
	}
	return 0;
}

int CFarsite::WriteSpotShapeFile(const char *trgName)
{
	if(m_pFarsite)
	{
		return m_pFarsite->WriteSpotShapeFile(trgName);
	}
	return 0;
}

int CFarsite::GetNumIgnitionCells()
{
	if(m_pFarsite)
	{
		return m_pFarsite->GetNumIgnitionCells();
	}
	return 0;
}



// private

void CFarsite::SetFarsiteProgress(int newProgress)
{
	this->m_pFarsite->SetFarsiteProgress(newProgress);
}
