//FARSITE.h
//Defines exported class for FARSITE.dll

/*#ifdef BUILD_FARSITEDLL
#define FARSITEDLL_EXPORT __declspec(dllexport)
#else
#define FARSITEDLL_EXPORT __declspec(dllimport)
#endif */
#define FARSITEDLL_EXPORT

class Farsite5;

class FARSITEDLL_EXPORT CFarsite
{
public:
	CFarsite();
	~CFarsite();
	int SetLandscapeFile(const char *_lcpFileName);
	int SetIgnition(const char *shapeFileName);
	int SetBarriers(const char *shapeFileName);
	int SetNumProcessors(int numThreads = 1);
	int SetStartProcessor(int _procNum);
	int GetStartProcessor();
	int GetNumIgnitionCells();

	int LoadFarsiteInputs(const char *_inputFile);
	char *CommandFileError(int i_ErrNum);
	char *GetErrorMessage(int errNum);

	double GetResolution();
	long GetNumCols();
	long GetNumRows();
	double GetWest();
	double GetSouth();
	double GetEast();
	double GetNorth();

	int CanLaunchFarsite(void);
	int LaunchFarsite(void);
    int GetFarsiteProgress() const;
 // double GetFarsiteProgress(int *ai_RunStatus, char cr[]);
// int  GetFarsiteStatus (char cr[]);

	int CancelFarsite(void);

	//output functions
	int WriteArrivalTimeGrid(const char *trgName);
	int WriteIntensityGrid(const char *trgName);
	int WriteFlameLengthGrid(const char *trgName);
	int WriteSpreadRateGrid(const char *trgName);
	int WriteSpreadDirectionGrid(const char *trgName);
	int WriteHeatPerUnitAreaGrid(const char *trgName);
	int WriteReactionIntensityGrid(const char *trgName);
	int WriteCrownFireGrid(const char *trgName);
	int WriteArrivalTimeGridBinary(const char *trgName);
	int WriteIntensityGridBinary(const char *trgName);
	int WriteFlameLengthGridBinary(const char *trgName);
	int WriteSpreadRateGridBinary(const char *trgName);
	int WriteSpreadDirectionGridBinary(const char *trgName);
	int WriteHeatPerUnitAreaGridBinary(const char *trgName);
	int WriteReactionIntensityGridBinary(const char *trgName);
	int WriteCrownFireGridBinary(const char *trgName);
	int WriteIgnitionGrid(const char *trgName);
	int WritePerimetersShapeFile(const char *trgName);
	int WriteTimingsFile(const char *trgName);

	int WriteOneHours(const char *trgName);
	int WriteTenHours(const char *trgName);
	int WriteHundredHours(const char *trgName);
	int WriteMoistData(const char *trgName);
	int WriteSpotDataFile(const char *trgName);
	int WriteSpotShapeFile(const char *trgName);
	int WriteSpotGrid(const char *trgName);

private:
	Farsite5 *m_pFarsite;

};
