
#include "CWindsGrid.h"
#include "Farsite5.h"

#include <vector>
#include <filesystem>
#include <algorithm> 
#include <cctype>
#include <locale>

// Some helper string trimming functions. Best to remove and use filesystem
// stuff but leaving for now.

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

int ReadAsciiGrid(int *nCols, int *nRows, double *xllcorner, double *yllcorner, double *res, float **vals, const char *fileName)
{
    char buf[256];
    char seps[] = " ,\t\r\n";
    char *token;
    FILE *in = fopen(fileName, "rt");
    if (!in)
        return e_EMS_FILE_OPEN_ERROR;
    int t;
    float fVal;
    bool hasCols = false, hasRows = false, hasXll = false, hasYll = false, hasRes = false;
    for (t = 0; t < 6; t++)
    {
        fgets(buf, 160, in);
        token = strtok(buf, seps);
        if (strcmp(token, "ncols") == 0) // _strnicmp is windows only, replaced with strcmp, losing the argument count = 5
        {
            token = strtok(NULL, seps);
            if (token)
            {
                *nCols = atol(token);
                hasCols = true;
            }
        }
        if (strcmp(token, "nrows") == 0) // _strnicmp is windows only, replaced with strcmp, losing the argument count = 5
        {
            token = strtok(NULL, seps);
            if (token)
            {
                *nRows = atol(token);
                hasRows = true;
            }
        }
        if (strcmp(token, "xllcorner") == 0) // _strnicmp is windows only, replaced with strcmp, losing the argument count = 9
        {
            token = strtok(NULL, seps);
            if (token)
            {
                *xllcorner = atof(token);
                hasXll = true;
            }
        }
        if (strcmp(token, "yllcorner") == 0) // _strnicmp is windows only, replaced with strcmp, losing the argument count = 9
        {
            token = strtok(NULL, seps);
            if (token)
            {
                *yllcorner = atof(token);
                hasYll = true;
            }
        }
        if (strcmp(token, "cellsize") == 0) // _strnicmp is windows only, replaced with strcmp, losing the argument count = 8
        {
            token = strtok(NULL, seps);
            if (token)
            {
                *res = atof(token);
                hasRes = true;
            }
        }
        if (strcmp(token, "nodata_val") == 0) // _strnicmp is windows only, replaced with strcmp, losing the argument count = 10
        {
            token = strtok(NULL, seps);
            //if (token)
            //	trgGrid-> = atof(token);
        }
    }
    if (!hasRows || !hasCols || !hasRes || !hasXll || !hasYll
        || *nRows <= 0 || *nCols <= 0 || *res <= 0.0)//invalid, can't create grid
    {
        fclose(in);
        return e_EMS_FARSITE_WINDGRID_INVALID;
    }
    long long int nVals = *nRows * *nCols; //replaced windows variable __int64 with long long int
    float *tVals;
    tVals = new float[nVals];
    for (int r = 0; r < nVals; r++)
    {
        tVals[r] = 0.0;
        fscanf(in, "%f", &fVal);
        if (fVal < 0)//can't have NODATA!!!!
        {
            delete[] tVals;
            fclose(in);
            return e_EMS_FARSITE_WINDGRID_HAS_NODATA;
        }
        tVals[r] = fVal;
    }
    fclose(in);
    *vals = tVals;
    return 1;
}



int CAtmWindGrid::Load(short month, short day, short hour, const char *speedFileName,
                       const char *dirFileName, bool isMetric/* = false*/)
{

    m_month = month;
    m_day = day;
    m_hour = hour;
    int tCols, tRows;
    double tRes, tXllCorner, tYllCorner;
    float *tVals = NULL;
    int status = ReadAsciiGrid(&tCols, &tRows, &tXllCorner, &tYllCorner,
                               &tRes, &tVals, speedFileName);
    if (status != 1)
    {
        if (tVals)
        {
            delete[] tVals;
        }
        return status;
    }
    m_nCols = tCols;
    m_nRows = tRows;
    m_xllcorner = tXllCorner;
    m_yllcorner = tYllCorner;
    m_res = tRes;
    m_speedVals = tVals;
    tVals = NULL;
    status = ReadAsciiGrid(&tCols, &tRows, &tXllCorner, &tYllCorner,
                           &tRes, &tVals, dirFileName);
    //basic error checks
    if (status != 1)
    {
        if (tVals)
            delete[] tVals;
        if (m_speedVals)
            delete[] m_speedVals;
        m_speedVals = NULL;
        return status;
    }
    if (tCols != m_nCols || tRows != m_nRows || tXllCorner != m_xllcorner
        || tYllCorner != m_yllcorner || tRes != m_res)
    {
        if (tVals)
            delete[] tVals;
        if (m_speedVals)
            delete[] m_speedVals;
        m_speedVals = NULL;
        return e_EMS_FARSITE_WINDGRID_SIZE_MISMATCH;
    }
    m_dirVals = tVals;
    if (isMetric)
    {//convert to mph here from km/hour, ALSO COVERT FROM 10M TO 20' HEIGHT
        const double kph2mph = 0.62137119223733;

        for (long long int c = 0; c < m_nCols * m_nRows; c++) //replaced windows variable __int64 with long long int
        {
            if (m_speedVals[c] > 0.0)
            {
                //first, convert to 20'
                m_speedVals[c] /= 1.15;
                //now convert km/h to mph
                m_speedVals[c] *= kph2mph;
            }
        }
    }
    return 1;

}


short CAtmWindGrid::GetMonth()
{
    return m_month;
}

short CAtmWindGrid::GetDay()
{
    return m_day;
}

short CAtmWindGrid::GetHour()
{
    return m_hour;
}

int CAtmWindGrid::GetSimTime()
{
    return m_simTime;
}

float CAtmWindGrid::GetWindSpeed(double x, double y)
{
    long long int pos = GetPos(x, y); //replaced windows variable __int64 with long long int
    if (pos >= 0 || pos < m_nRows * m_nCols)
        return m_speedVals[pos];
    return NODATA_VAL;
}

float CAtmWindGrid::GetWindDir(double x, double y)
{
    long long int pos = GetPos(x, y); //replaced windows variable __int64 with long long int
    if (pos >= 0 || pos < m_nRows * m_nCols)
        return m_dirVals[pos];
    return NODATA_VAL;
}

void CAtmWindGrid::SetSimTime(Farsite5 *pFarsite)
{
    int hr, mn;
    hr = m_hour / 100;
    mn = m_hour - (hr * 100);
    double val = pFarsite->ConvertActualTimeToSimtime(m_month, m_day, hr*100, mn, false);
    m_simTime = val;
}

double CAtmWindGrid::GetEast()
{
    return m_xllcorner + m_nCols * m_res;
}

double CAtmWindGrid::GetNorth()
{
    return m_yllcorner + m_nRows * m_res;
}

double CAtmWindGrid::GetWest()
{
    return m_xllcorner;
}

double CAtmWindGrid::GetSouth()
{
    return m_yllcorner;
}

double CAtmWindGrid::GetRes()
{
    return m_res;
}

void CAtmWindGrid::GetWinds(double x, double y, float *spd, float *dir)
{
    *spd = GetWindSpeed(x, y);
    *dir = GetWindDir(x, y);
}

long long int CAtmWindGrid::GetPos(double x, double y) //replaced windows variable __int64 with long long int
{
    long long int r, c; //replaced windows variable __int64 with long long int
    r = (m_yllcorner + m_res * m_nRows - y) / m_res;
    //r = (y - m_yllcorner) / m_res;
    c = (x - m_xllcorner) / m_res;
    if (r >= 0 && r < m_nRows && c >= 0 && c < m_nCols)
        return r * m_nCols + c;
    return -1;
}

CWindGrids::CWindGrids()
{
    //std::vector<CAtmWindGrid *> m_vpGrids;
}

CWindGrids::~CWindGrids()
{
    while (m_vpGrids.size() > 0)
    {
        delete m_vpGrids.back();
        m_vpGrids.pop_back();
    }
}

//const char DELIMITER = '\"';

void Tokenize(const char delim, std::string str, std::vector<std::string> &token_v)
{
    size_t start = str.find_first_not_of(delim), end = start;

    while (start != std::string::npos) {
        // Find next occurence of delimiter
        end = str.find(delim, start);
        // Push back the token found into vector
        token_v.push_back(str.substr(start, end - start));
        // Skip all occurences of the delimiter to find new start
        start = str.find_first_not_of(delim, end);
    }
}

int ParseFileNamesFromAtmRec(char *recBuf, std::string *spdName, std::string *dirName)
{
//    int retCount = 0;
    int blankCount = 0, loc = 0, len = strlen(recBuf);
    while (blankCount < 3 && loc < len)
    {
        if (recBuf[loc] == ' ')
            blankCount++;
        loc++;
    }
    std::string filesStr = &recBuf[loc];
    trim(filesStr);
    int qCount = 0;
    for (size_t c = 0; c < filesStr.size(); c++)
    {
        if (filesStr[c] == '\"')
            qCount++;
    }
    if (qCount %2 != 0)
    {
        *spdName = "";
        *dirName = "";
        return 0;
    }
    if (qCount == 0)
    {
//        int tStart = 0;
        std::vector<std::string> v;
        Tokenize(' ', filesStr, v);
        std::vector<std::string>::iterator it;
        for (it = v.begin(); it != v.end(); ++it)
        {
            trim(*it);
        }
        if (v.size() > 1)
        {
            *spdName = v[0];
            it = v.begin();
            ++it;
            while ((*it).size() <= 0 && it != v.end())
                it++;
            *dirName = *it;
            trim(*spdName);
            trim(*dirName);
        }
        return 2;
    }
    else
    {
        std::vector<std::string> vStrs;
        std::vector<std::string>::iterator it;

        Tokenize('\"', filesStr, vStrs);
//        bool dirSet = false;
        *spdName = vStrs[0];
        trim(*spdName);
        for (it = vStrs.begin(); it != vStrs.end(); ++it)
        {
            trim(*it);
        }
        it = vStrs.begin();
        ++it;
        while ((*it).size() <= 0 && it != vStrs.end())
            it++;
        *dirName = *it;
        trim(*dirName);
        return 2;
    }
    return 0;
}

// atmFileName something like "./examples/cougarCreek/input/cougarCreek.atm"
// For Cougar Creek example
int CWindGrids::Create(const char *atmFileName)
{
    std::filesystem::path path;  //[256]; //, oldPath[256];
    std::filesystem::path oldPath = std::filesystem::current_path();
    path = atmFileName;
    
    //std::cout << "atm file: " << atmFileName << "     Path: "<< oldPath;
    // DWS: Why is code looking for literal backslashes? Is this assuming windows style paths?
    
    // char *p = strrchr(path, '\\');
    // if (p)
    // {
    //     *p = 0;
    //     std::filesystem::current_path(path);
    // }
    FILE *atmFile = fopen(atmFileName, "rt");
    if (!atmFile)
    {
//        std::filesystem::current_path(oldPath);
        return  e_EMS_FILE_OPEN_ERROR;
    }
    char buf[640];// , spdBuf[256], dirBuf[256];
    int month, day, hour;
    char speedFile[256], dirFile[256], unitsStr[64];
    int nRead;
    //first, look for units, default is ENGLISH
    bool isMetric = false;
    while (fgets(buf, 639, atmFile) != NULL)
    {
        nRead = sscanf(buf, "%s", unitsStr);
        if (nRead == 1)
        {
            if (strcasecmp(unitsStr, "METRIC") == 0)
            {
                isMetric = true;
                break;
            }
            else if (strcasecmp(unitsStr, "ENGLISH") == 0)
            {
                isMetric = false;
                break;
            }
            
        }
    }
    rewind(atmFile);
    std::string strSpdName, strDirName;
    while (fgets(buf, 639, atmFile) != NULL)
    {
        nRead = sscanf(buf, "%d %d %d %s %s",
                       &month, &day, &hour, speedFile, dirFile);
        if (nRead == 5)
        {
            std::string cStrSpdName, cStrDirName;
            ParseFileNamesFromAtmRec(buf, &cStrSpdName, &cStrDirName);
            strSpdName = cStrSpdName;
            strDirName = cStrDirName;
            // std::cout << strSpdName <<" " << strDirName <<"\n";
            // Remove all double-quote characters
            // DWS: why what is this for? Why the manual string mangling?
            // strSpdName.erase(
            //     remove(strSpdName.begin(), strSpdName.end(), '\"'),
            //     strSpdName.end());
            // strDirName.erase(
            //     remove(strDirName.begin(), strDirName.end(), '\"'),
            //     strDirName.end());

            CAtmWindGrid *pGrid = new CAtmWindGrid();
            int ret = pGrid->Load(month, day, hour, strSpdName.c_str(), strDirName.c_str(), isMetric);
            if (ret == 1)
                m_vpGrids.push_back(pGrid);
            else
            {
                fclose(atmFile);
                //std::filesystem::current_path(oldPath);
                return ret;
            }
        }
    }
    fclose(atmFile);
    //std::filesystem::current_path(oldPath);
    return 1;
}

bool CWindGrids::CompBySimTime(CAtmWindGrid *pGrid1, CAtmWindGrid *pGrid2)
{
    return pGrid1->GetSimTime() < pGrid2->GetSimTime();
}

void CWindGrids::SetSimTimes(Farsite5 *pFarsite)
{
    std::vector<CAtmWindGrid *>::iterator it;
    for (it = m_vpGrids.begin(); it != m_vpGrids.end(); ++it)
    {
        CAtmWindGrid *pGrid = *it;
        pGrid->SetSimTime(pFarsite);
        //printf("gridCount = %d, m_month = %d, m_day = %d, m_hour = %d, m_simTime = %d\n",std::distance(m_vpGrids.begin(), it),pGrid->GetMonth(),pGrid->GetDay(),pGrid->GetHour(),pGrid->GetSimTime());
    }
    //now want to make sure grids are sequential
    if(m_vpGrids.size() > 1)
        std::sort(m_vpGrids.begin(), m_vpGrids.end(), CompBySimTime);
}

int CWindGrids::CheckCoverage(Farsite5 *pFarsite)
{
    //check all grids to ensure they cover the landscape
    std::vector<CAtmWindGrid *>::iterator it;
    for (it = m_vpGrids.begin(); it != m_vpGrids.end(); ++it)
    {
        CAtmWindGrid *pGrid = *it;
        if (pGrid->GetWest() > pFarsite->m_xLo
            || pGrid->GetEast() < pFarsite->m_xHi
            || pGrid->GetSouth() > pFarsite->m_yLo
            || pGrid->GetNorth() < pFarsite->m_yHi)
            return e_EMS_FARSITE_WINDGRID_COVERAGE;
    }
    return 1;
}

int CWindGrids::CheckTimes(Farsite5 *pFarsite)
{
    if (m_vpGrids.size() <= 0)
        return e_EMS_FARSITE_WINDGRID_TIME;
    CAtmWindGrid *pGrid = *(m_vpGrids.begin());
    if(pGrid->GetSimTime() > 0)
        return e_EMS_FARSITE_WINDGRID_TIME;
    return 1;
}

bool CWindGrids::IsValid()
{
    if (m_vpGrids.size() > 0)
        return true;
    return false;
}

void CWindGrids::GetWinds(double simTime, double x, double y, double *speed, double *direction)
{
    *speed = 0.0;
    *direction = 0.0;
    std::vector<CAtmWindGrid *>::iterator it;
    CAtmWindGrid *trgGrid = *(m_vpGrids.begin());

    for (it = m_vpGrids.begin(); it != m_vpGrids.end(); ++it)
    {
        CAtmWindGrid *tmpGrid = *it;
        if (tmpGrid->GetSimTime() <= simTime)
            trgGrid = tmpGrid;
        else
            break;
    }
    float spd, dir;
    trgGrid->GetWinds(x, y, &spd, &dir);
    //printf("farsiteSimTime = %f, gridCount = %d, gridSimTime = %d, x = %f, y = %f, speed = %f, dir = %f\n",simTime,std::distance(m_vpGrids.begin(), it)-1,trgGrid->GetSimTime(),x,y,spd,dir);
    *speed = spd;
    *direction = dir;
}

// CAtmWindGrid class starts

CAtmWindGrid::CAtmWindGrid()
{
    m_month = m_day = m_hour = 0;
    m_simTime = -1;
    m_speedVals = m_dirVals = NULL;
}
CAtmWindGrid::~CAtmWindGrid()
{
    if (m_speedVals)
        delete[] m_speedVals;
    if (m_dirVals)
        delete[] m_dirVals;
}


