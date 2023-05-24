// FARSITE.h

// Date and authors?

// Defines the main user-level (or shared library) class for the farsite fire
// spread model. In the windows version, this is built to `FARSITE.dll`



/*
#ifdef BUILD_FARSITEDLL
#define FARSITEDLL_EXPORT __declspec(dllexport)
#else
#define FARSITEDLL_EXPORT __declspec(dllimport)
#endif
*/

#define FARSITEDLL_EXPORT

// Declare main implementation class. Note that this is used inside the
// CFarsite class as a member object. No inheritance is used.
class Farsite5;

#include <mutex>

extern std::mutex iomutex;  // for locking std::cout and cerr. An alternative
                            // with c++20 is to use a synced stream object.


// CFarsite class. This is interface for application-level code.
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

    // Progress indicators for use in progress bars, spinners, etc.
    int GetFarsiteProgress() const;
    const char* GetFarsiteStatusString() const;

	int CancelFarsite(void);

	// output functions
    
    // write all outputs
    int writeOutputs(int outType, const std::string outputPath);

    // write individual spatial products
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

    // Internally can set the progress tracker of the Farsite5 object
    void SetFarsiteProgress(int newProgress);

    // For error output thread safe
    void printError(const char *theerr,  const char *fname);

};
