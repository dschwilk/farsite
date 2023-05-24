// CWindsGrid.h
//
// Author? Date?
// Define gridded winds class. This allows WindNinja outputs as wx inputs to farsite.


#include "Farsite5.h"

#include <vector>

#ifndef CWindsGrid_h
#define CWindsGrid_h

class CAtmWindGrid
{
public:
    CAtmWindGrid();
    ~CAtmWindGrid();
    int Load(short month, short day, short hour, const char *speedFileName, const char *dirFileName, bool isMetric = false);
    short GetMonth();
    short GetDay();
    short GetHour();
    int GetSimTime();
    float GetWindSpeed(double x, double y);
    float GetWindDir(double x, double y);
    void SetSimTime(Farsite5 *pFarsite);
    double GetEast();
    double GetNorth();
    double GetWest();
    double GetSouth();
    double GetRes();
    void GetWinds(double x, double y, float *spd, float *dir);
private:
    long long int GetPos(double x, double y); //replaced windows variable __int64 with long long int
    long long int m_nCols, m_nRows; //replaced windows variable __int64 with long long int
    double m_xllcorner, m_yllcorner, m_res;
    float *m_speedVals;
    float *m_dirVals;

    short m_month;
    short m_day;
    short m_hour;
    int m_simTime;
};

class CWindGrids
{
public:
    CWindGrids();
    ~CWindGrids();
    int Create(const char *atmFileName);
    static bool CompBySimTime(CAtmWindGrid *pGrid1, CAtmWindGrid *pGrid2);
    void SetSimTimes(Farsite5 *pFarsite);
    int CheckCoverage(Farsite5 *pFarsite);
    int CheckTimes(Farsite5 *pFarsite);
    bool IsValid();
    void GetWinds(double simTime, double x, double y, double *speed, double *direction);
private:
    std::vector<CAtmWindGrid *> m_vpGrids;

};


#endif
