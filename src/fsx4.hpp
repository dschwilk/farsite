/*
 * NOTICE OF RELEASE TO THE PUBLIC DOMAIN
 *
 * This work was created using public funds by employees of the
 * USDA Forest Service's Fire Science Lab and Systems for Environmental
 * Management.  It is therefore ineligible for copyright under title 17,
 * section 105 of the United States Code.  You may treat it as you would
 * treat any public domain work: it may be used, changed, copied, or
 * redistributed, with or without permission of the authors, for free or
 * for compensation.  You may not claim exclusive ownership of this code
 * because it is already owned by everyone.  Use this software entirely
 * at your own risk.  No warranty of any kind is given.
 *
 * FARSITE is a trademark owned by Mark Finney.  You may not call derived
 * works by the name FARSITE without explicit written permission.
 *
 * A copy of 17-USC-105 should have accompanied this distribution in the file
 * 17USC105.html.  If not, you may access the law via the US Government's
 * public websites:
 *   - http://www.copyright.gov/title17/92chap1.html#105
 *   - http://www.gpoaccess.gov/uscode/  (enter "17USC105" in the search box.)
 */
//#include <owl\owlpch.h>
#ifndef MainHeaderFile
#define MainHeaderFile

//#include "newfms.h"
#include "fsxlandt.h"
#include "fsxpfront.h"


#include <map>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>


#ifndef int32
typedef int32_t int32; 
#endif


typedef struct
{
	double Point;   						// accel const for points
	double Line;						// accel const for lines
	double Limit;						// upper perimeter limit for points
} AccelConstants;


struct Acceleration
{
	AccelConstants ac[261]; 				// store enough for 256 fuel models and default settings for dialog

	Acceleration();
};


class Crown
{
	double CritRos, CritCFB, Ro;

public:
	double A, Io, SFC, CrownFractionBurned, CrownLoadingBurned;
	double HLCB, FoliarMC, CrownBulkDensity, TreeHeight;
	double FlameLength;
	Farsite5 *pFarsite;

	Crown(Farsite5 *_pFarsite);
	~Crown();
	void CrownIgnite(double height, double base, double density);
	double CrownSpread(double avgros, double R10);
	void CrownIntensity(double cros, double* fli);
	void CrownBurn(double avgros, double fli, double AccelConstant);
	double CrownBurn2(double avgros, double fli, double AccelConstant, void *vt);
};


class APolygon
{
	Farsite5 *pFarsite;
public:
	double startx, starty;
	APolygon(Farsite5 *_pFarsite);
	~APolygon();
	long Overlap(long count);
	double direction(double xpt1, double ypt1);
};


class StandardizePolygon		// for reordering perimeter from extreme point
{
	Farsite5 *pFarsite;
public:
	StandardizePolygon(Farsite5 *_pFarsite);
	//{
	//}
	~StandardizePolygon();
	//{
	//}
	long FindExternalPoint(long CurrentFire, long type);
	void ReorderPerimeter(long CurrentFire, long NewBeginning);
	void FindOuterFirePerimeter(long CurrentFire);
	bool Cross(double xpt1, double ypt1, double xpt2, double ypt2,
		double xpt1n, double ypt1n, double xpt2n, double ypt2n, double* newx,
		double* newy, long* dup1, long* dup2);
	void DensityControl(long CurrentFire);
	void RemoveDuplicatePoints(long CurrentFire);
	void RemoveIdenticalPoints(long FireNum);
	void BoundingBox(long CurrentFire);
};


class IgnitionCorrect : public APolygon, public StandardizePolygon
{
	Farsite5 *pFarsite;
public:
	IgnitionCorrect(Farsite5 *_pFarsite);
	~IgnitionCorrect();

	void ReversePoints(long type);
	//void BoundingBox();
	double arp();
};


class XUtilities : public APolygon
{
	long swapnumalloc;
	size_t nmemb;
	Farsite5 *pFarsite;

public:
	long ExNumPts, OldNumPoints;	  // USED BY RASTER AND BURN CLASSES, WITH D-METH2
	double* swapperim;  			  // pter to swap array for perimeter points

	XUtilities(Farsite5 *_pFarsite);
	~XUtilities();
	void AllocSwap(long NumPoint);
	void FreeSwap();
	void GetSwap(long NumPoint, double* xpt, double* ypt, double* ros,
		double* fli, double* rct);
	void SetSwap(long NumPoint, double xpt, double ypt, double ros,
		double fli, double rct);
	void SwapTranz(long writefire, long nump);
	void tranz(long count, long nump);
	void rediscretize(long* count, bool Reorder);
	void RePositionFire(long* firenum);
	void RestoreDeadPoints(long firenum);
};


class CompareRect
{
	double Xlo, Xhi, Ylo, Yhi;
	double xlo, xhi, ylo, yhi;
	Farsite5 *pFarsite;

public:
	double XLO, XHI, YLO, YHI;
	CompareRect(Farsite5 *_pFarsite);
	~CompareRect();
	void InitRect(long FireNum);
	void WriteHiLo(long FireNum);
	void ExchangeRect(long FireNum);
	bool XOverlap();
	bool YOverlap();
	bool BoundCross(long Fire1, long Fire2);
	void DetermineHiLo(double xpt, double ypt);
	bool MergeInAndOutOK1(long Fire1, long Fire2);
};


class CrossThread
{
	long CurrentFire, NextFire;
	long ThreadOrder;
	long SpanAStart, SpanAEnd, SpanBStart, SpanBEnd, Terminate;
	long NumCross, NumAlloc;
	long* intersect;
	double* interpoint;
	unsigned ThreadID;
	X_HANDLE hXEvent;
	APolygon poly;

	bool Cross(double x, double y, double xn, double yn, double cx, double cy,
		double cxn, double cyn, double* nx, double* ny, long* dup1, long* dup2);
	bool ReallocCross(long Num);
	bool AllocCross(long Num);
	void SetInterPoint(long Number, double XCoord, double YCoord, double Ros,
		double Fli, double Rct);
	void SetIntersection(long Number, long XOrder, long YOrder);
	void FreeCross();
	void CrossCompare();
	unsigned RunCrossThread(void* crossthread);
	Farsite5 *pFarsite;

public:
	CrossThread(Farsite5 *_pFarsite);
	~CrossThread();

	void SetRange(long SpanAStart, long SpanAEnd);
	void StartCrossThread(long threadorder, long currentfire, long nextfire);
	long GetNumCross();
	long* GetIsect();
	double* GetIpoint();
};


class Intersections : public XUtilities, public CompareRect,
	public StandardizePolygon
{
	long SpanAStart, SpanAEnd, SpanBStart, SpanBEnd;
	long NoCrosses, numcross, noffset1, noffset2, readnum, writenum;
	long* intersect;						  // stores array addresses for intersecting perimeter points
	long* crossout;
	long* NewIsect;
	long* AltIsect;
	double* interpoint; 					  // stores actual intersection coordinates, ROS, & FLI
	long crossnumalloc, intersectnumalloc, interpointnumalloc;
	long newisectnumalloc;				  // for newclip
	long NumCrossThreads;
	CrossThread** crossthread;
	size_t nmemb;
	Farsite5 *pFarsite;
	//typedef struct
	//{    long number;
	//	double extreme;
	//	void *next;
	//} Extremes;

	bool AllocCrossThreads();
	void FreeCrossThreads();
	bool MergeInAndOutOK2(long Fire1, long Fire2);
	bool SwapBarriersAndFires();
	void AllocIntersection(long Number);
	void FreeIntersection();
	void AllocInterPoint(long Number);
	void FreeInterPoint();
	void GetIntersection(long Number, long* XOrder, long* YOrder);
	void SetIntersection(long Number, long XOrder, long YOrder);
	void GetInterPointCoord(long Number, double* XCoord, double* YCoord);
	void GetInterPointFChx(long Number, double* Ros, double* Fli, double* Rct);
	void SetInterPoint(long Number, double XCoord, double YCoord, double Ros,
		double Fli, double Rct);
	void AllocCrossout(long Number);
	void FreeCrossout();
	long GetCrossout(long Number);
	void SetCrossout(long Number, long Value);
	long GetSpan(long Number, long ReadFire);
	long intercode(long offcheck, long xypt);
	void GetOffcheck(long* offcheck);
	void CheckIllogicalExpansions(long CurrentFire);
	//long GetExtremePair(Extremes *First, long count);
	void FindMergeSpans(long FireNum);
	bool CrossCompare(long* CurrentFire, long NextFire);
	bool EliminateCrossPoints(long CurrentFire);
	bool CrossCompare1(long* CurrentFire, long NextFire);
	void BoundaryBox(long NumPoints);
	void FindFirePerimeter(long CurrentFire, long StartPoint);
	bool MergeFire(long* CurrentFire, long NextFire);
	void MergeBarrier(long* CurrentFire, long NextFire);
	void MergeBarrierNoCross(long* CurrentFire, long NextFire);
	void MergeWrite(long xend, long readstart, long readend, long* xwrite);
	void CheckEnvelopedFires(long Fire1, long Fire2);
	void OrganizeCrosses(long CurrentFire);
	void OrganizeIntersections(long CurrentFire);
	bool TurningNumberOK(long CurrentFire, long StartPoint);
	bool AllocNewIntersection(long NumCross);
	bool FreeNewIntersection();
	void SetNewIntersection(long Alt, long count, long isect1, long isect2);
	void GetNewIntersection(long Alt, long count, long* isect1, long* isect2);

	void ExtinguishSurroundedHotPoints(long CurrentFire);

public:
	PostFrontal post;

	Intersections(Farsite5 *_pFarsite);
	~Intersections();
	double arp(int PerimNum, long count);		// area of fire
	void CrossFires(int check, long* firenum);	// perform loop-clip and mergers
	void CrossesWithBarriers(long FireNum);		// fires X barriers @in sub-timesteps
	void CleanPerimeter(long CurrentFire);
	void ResetIntersectionArrays();
	void CloseCrossThreads();
};

/**
 * \brief This class aggregates fire behavior parameters on the "raster
 *  grid" using the UTM easting and northing values.  This data is then
 * written to a raster file in either GRASS or ARC/Info Grid format.
 *
 * <p>
 * This class has been altered during the conversion from GUI to GUI-less
 * operation.  The original code used the file to aggregate fire behavior
 * parameters onto the grid.  This code aggregates fire behavior using a
 * std::map<> which is keyed with the grid index.  This change was effected
 * because the liberal use of ftell() and fseek() did not turn out to be
 * portable when converted over to linux.  Replacement numbers were written
 * in the <i>middle</i> of existing numeric fields.  The current version
 * writes the <i>entire</i> raster out whenever SelectOutputs() or
 * SelectMemOutputs() is called.
 * </p>
 *
 * <p>
 * This change necessitated a change in the usage pattern of this class.
 * Formerly, whenever the data were dumped to the file, the in-memory cache
 * was flushed and the process of aggregating points started all over again.
 * The former usage was to &quot;flush early, flush often&quot;.  Current
 * usage requires a bit more restraint, due to the fact that the entire
 * file is rewritten every time.  I now have FARSITE configured to call
 * SelectMemOutputs() whenever TFarsiteInterface::FarsiteSimulationLoop()
 * exits.
 * </p>
 *
 * <p>
 * Finally, the change to the use of std::map<> offloads the memory management
 * from this class to the C++ standard library.
 * </p>
 */
class OutputFile
{
	LandscapeData *ld;

	protected:
	FILE* otpfile;
	typedef std::pair<long, long> coordinate ;
	typedef struct
	{
		double x, y ;
		double Time;
		double Fli;
		double Ros;
		double Rcx;
		long Dir;
		bool Write;
	} RastData;
	typedef std::map<coordinate, RastData> RasterMap ;
	double x, y, t, f, l, r, h, rx;
	double North, South, East, West;
	long c, d;
	double convf1, convf2, convf3;
	long filepos;   				// record filepositions in
	long FileOutput;
	long numrows, numcols; // was int32 but code later assumes long
	long NumRastAlloc;
	long NumRastData;
	long HeaderType ;
	RasterMap rd;

	enum CalcType { FLAME_LENGTH, HEAT_PER_AREA, FL_AND_HPA,
			CROWNFIRE, FIRELINE_INTENSITY, REACTION_INTENSITY} ;

	void Calcs(CalcType TYPE);

	void FreeRastData();
	bool SetRastData(double xpt, double ypt, double time, double fli,
		double ros, double rcx, long dir);
	void WriteOptionalFile();
	void WriteRastMemFiles();
	int RastMemFile(long Type);
	Farsite5 *pFarsite;
	int FarsiteLayerTypeToThemeType(int farsiteType);
	int WriteBinaryHeader(FILE *trg, int outType);
	int WriteBinaryData(FILE *trg, int outType);
public:
	OutputFile(Farsite5 *_pFarsite);
	~OutputFile();

	bool SetRastData();
	int WriteFile(long Type);
	inline void setHeaderType(long type) { HeaderType = type ;}
	void SelectOutputs(long Type);
	void SelectMemOutputs(long Type);
	void OptionalOutput(bool FromMemory);
	void ConvF();
	void InitRasterFiles(long HeaderType);
	void GetRasterExtent();
	int WriteArrivalTimeBinary(const char *trgName);
	int WriteIntensityBinary(const char *trgName);
	int WriteFlameLengthBinary(const char *trgName);
	int WriteSpreadRateBinary(const char *trgName);
	int WriteSpreadDirectionBinary(const char *trgName);
	int WriteHeatPerUnitAreaBinary(const char *trgName);
	int WriteReactionIntensityBinary(const char *trgName);
	int WriteCrownFireBinary(const char *trgName);

};



class Vectorize : public OutputFile
{
	long CurFire, i, I;
	FILE* vectfile;
	Farsite5 *pFarsite;

public:
	Vectorize(Farsite5 *_pFarsite);
	~Vectorize();
	void VectData(long CurrentFire, double SimTime);
	void ArcFileFormat();
	void OptionalFileFormat();
	void GetXY();
	void GetRF();
};


class Rasterize : public APolygon, public OutputFile
{
	long count, ExNumPts;
	long parallel;
	long num;
	double W1, W2, W3, W4, Wr1, Wr2, Wr3, Wr4, Wf1, Wf2, Wf3, Wf4;
	double WT, WrT, WfT, WxT, WRT;
	double diffx, diffy, xmid, ymid;
	double PFWest, PFSouth, PFRes;
	double StartX, StartY;
	double box[4][7];
	double TempBox[2];
	double LastTime, CurTime;
	bool SamePoint;
	FILE* rastfile;

	void raster();
	long Overlap();
	bool Interpolate(bool WriteFile);
	void AllDistanceWeighting();
	void TriangleArea();
	void DistWt();
	void TimeWt();
	void AreaWt();
	//void Calcs(int TYPE);
	void FMaxMin(int Coord, double* ptmax, double* ptmin);
	double findmax(double pt1, double pt1n, double pt2, double pt2n);
	double findmin(double pt1, double pt1n, double pt2, double pt2n);
	void rastdata(double east, double north, double* eastf, double* northf,
		long MaxMIn);
	void PFrastdata(double east, double north, double* eastf, double* northf,
		long MaxMIn);
	bool Cross(double xpt1, double ypt1, double xpt2, double ypt2,
		double xpt1n, double ypt1n, double xpt2n, double ypt2n, double* newx,
		double* newy);
	Farsite5 *pFarsite;

public:

	Rasterize(Farsite5 *_pFarsite);
	~Rasterize();
	void rasterinit(long CurrentFire, long ExNumPts, double SIMTIME,
		double TimeIncrement, double CuumTimeIncrement);
	void RasterizePostFrontal(double Resolution, char* FileName,
		bool ViewPort, long ThemeNumber);
	void RasterReset();
};


class AreaPerimeter : public APolygon
{
	Farsite5 *pFarsite;
public:
	double area, perimeter, sperimeter;
	double cuumslope[2];

	AreaPerimeter(Farsite5 *_pFarsite);
	~AreaPerimeter();
	void arp(long count);
};



class Mechanix
{
	double lb_ratio, hb_ratio, rateo;			 // rateo is ROS w/o slope or wind
	double b, part1;							 // parameters for phiw
	//	void fuelmod(int fuel, double *depth);
	double headback(void);
	double CalcEffectiveWindSpeed();

protected:
	double m_ones, m_tens, m_hundreds, m_livew, m_liveh;				// local copies of FE data for use in spreadrate
	double phiw, phis, phiew, LocalWindDir;
	double FirePerimeterDist, slopespd;

	double accel(double RosE, double RosT, double A, double* avgros,
		double* cosslope);
	void TransformWindDir(long slope, double aspectf);
	double vectordir(double aspectf);
	double vectorspd(double* VWindSpeed, double aspectf, long FireType);
	Farsite5 *pFarsite;

public:
	double xpt, ypt, midx, midy, xt, yt, xptn, yptn, xptl, yptl, xdiff, ydiff;
	double vecspeed, ivecspeed, vecdir, m_windspd, m_winddir, m_twindspd;
	double ActualSurfaceSpread, HorizSpread, avgros, cros, fros, vecros,
		RosT1, RosT, R10;
	double fli, FliFinal, ExpansionRate;
	double timerem, react, savx, cosslope;
	double CurrentTime, FlameLength, CrownLoadingBurned, CrownFractionBurned;
	double head, back, flank;

	Mechanix(Farsite5 *_pFarsite);
	~Mechanix();
	void limgrow(void);							// limits growth to distance checking
	void grow(double ivecdir);					// Richards (1990) differential equation
	void distchek(long CurrentFire);				// checks perimeter and updates distance check
	void ellipse(double iros, double wspeed);		// calculates elliptical dimensions
	void scorrect(long slope, double aspectf);		// slope correction
	double spreadrate(long slope, double windspd, int fuel);	// Rothermel spread equation
	void GetEquationTerms(double *pphiw, double *pphis, double *bb, double *ppart1);
};




typedef struct
{
	double ones;
	double tens;
	double hundreds;
	double thousands;
	//double tenthous;
	double livew;
	double liveh;
	double windspd;
	double winddir;
	double tws;
	double temp;
	double humid;
	double solrad;
} EnvironmentData;


//------------------------------------------------------------------------------
//
// NEW MOISTURE MODEL AND
//
//------------------------------------------------------------------------------

#define FM_SLOPPY 	3
#define FM_LIBERAL 	2
#define FM_MODERATE 1
#define FM_HARDASS 	0

#define MAXNUM_FUEL_MODELS 256
#define NUM_FUEL_SIZES 4
#define MAX_NUM_STATIONS 5

#define SIZECLASS_1HR			0
#define SIZECLASS_10HR			1
#define SIZECLASS_100HR			2
#define SIZECLASS_1000HR			3


class FELocalSite : public LandscapeData
{
	long LW, LH;
	double onehour, tenhour, hundhour, thousands, tenthous;
	double twindspd, mwindspd, wwinddir, airtemp, relhumid, solrad;

	double XLocation, YLocation;

	celldata cell;

	crowndata crown;

	grounddata ground;

	void windreduct();
	void windadj(long date, double hours, long* cloud);
	void windadj_Old(long date, double hours, long* cloud);
    void WindGridAdjust(double SimTime);
    void  windadj_New(long date, double hours, long* cloud);
    int GetMthDay (int date, int *ai_Mth, int *ai_Day);


	void AtmWindAdjustments(long date, double hours, long* cloud);
	Farsite5 *pFarsite;

public:
	long StationNumber;
	double AirTemp, AirHumid, PtTemperature, PtHumidity;		   // fuel level temps and humidities at each point

	FELocalSite(Farsite5 *_pFarsite);
	void GetFireEnvironment( double SimTime, bool All);
	long GetLandscapeData(double xpt, double ypt, LandscapeStruct& ls);
	long GetLandscapeData(double xpt, double ypt);
	void GetEnvironmentData(EnvironmentData* ed2);
};


//------------------------------------------------------------------------------
//
//
//------------------------------------------------------------------------------


class MechCalls : public Mechanix
{
	Farsite5 *pFarsite;
public:
	double A, SubTimeStep;		// acceleration constant
	EnvironmentData gmw, lmw;   	// initialize global and local structures
	LandscapeStruct ld;

	MechCalls(Farsite5 *_pFarsite);
	~MechCalls();
	void GetPoints(long CurrentFire, long CurrentPoint);
	bool NormalizeDirectionWithLocalSlope();  // modify point orient'n with slope
	void VecSurf(); 						  // computes vectored surface spread rate
	void VecCrown();					  // computes vectored crown spread rate
	void SpreadCorrect();     			  // corrects crown spread rate from directional spread
	void GetAccelConst();   		   	  // retrieves acceleration constants
	void AccelSurf1();					  // performs 1st accel for surface fire
	void AccelSurf2();  					  // performs 2nd accel for surface fire
	void AccelCrown1();					  // performs 1st accel for crown fire
	void AccelCrown2(); 					  // performs 2nd accel for crown fire
	void SlopeCorrect(int spreadkind);  	  // corrects spread for slope
	void LoadGlobalFEData(FELocalSite* fe);
	void LoadLocalFEData(FELocalSite* fe);
};


typedef struct
{
	double x;
	double y;
	double PartDiam;
	double ZHeight;
	double StartElev;
	double CurrentTime;
	double ElapsedTime;
	void* next;				// pointer to next ember in sequence
} emberdata;


class Embers : public APolygon  	  // define Embers object, with ember and spot data
{
	emberdata TempEmber;
	emberdata* FirstEmber;
	emberdata* CurEmber;
	emberdata* NextEmber;
	emberdata* CarryEmber;
	emberdata* NextCarryEmber;

	double xdiffl, ydiffl, xdiffn, ydiffn, FrontDist;		// sstep is the spot time step, in minutes
	void fuelcoefs(int fuel, double* coef, double* expon);
	double torchheight(double partdiam, double zo);
	double pileheight(double partdiam);
	double VertWindSpeed(double HeightAboveGround, double TreeHt);
	double partcalc(double vowf, double z);
	MechCalls mech;
//	FireEnvironment2* env;
	FELocalSite* fe;

	struct fcoord
	{
		double x, y, xl, yl, xn, yn, e;
		long cover;
	};
	Farsite5 *pFarsite;
public:
	typedef struct
	{
		double x;
		double y;
		double TimeRem;
		void* next;
	} spotdata;

	spotdata* FirstSpot;
	spotdata* CurSpot;
	spotdata* NextSpot;
	spotdata* CarrySpot;

	int SpotSource;
	double SteadyHeight, Duration, SourceRadius, SourcePower;			// flame parameters for lofting
	long NumEmbers, CarryOverEmbers, NumSpots, SpotFires, CarrySpots;
	fcoord Fcoord;

	Embers(Farsite5 *_pFarsite);
	~Embers();
	void Loft(double CFlameLength, double CFBurned, double CrownHeight,
		double LoadingBurned, double ROS, double SubTimeStep, double curtime);
	void Plume(double CFlameLength, double CFBurned);
	void Flight(double CurTime, double EndofTimeStep);
	void Overlap();
	void EmberReset();
	void SpotReset(long numspots, spotdata* ThisSpot);
	void SetFireEnvironmentCalls( FELocalSite* Fe);

	emberdata ExtractEmber(long type);    // type=0, regular, type==1, carry
	void AddEmber(emberdata* ember);
};


class BurnThread : public MechCalls
{
	long CurrentFire, CurrentPoint, CuumPoint, TotalPoints;
	long Begin, End, turn;
	bool FireIsUnderAttack;
	double TimeIncrement, CuumTimeIncrement, SIMTIME, SimTimeOffset;
	unsigned ThreadID;
	X_HANDLE hBurnThread;
	Crown cf;
	FireRing* firering;

	void SurfaceFire();
	void CrownFire();
	void SpotFire(int SpotSource);
	void EmberCoords();
	unsigned RunBurnThread(void* burnthread);
	void PerimeterThread();
	unsigned RunSpotThread(void* burnthread);
	void SpotThread();
	Farsite5 *pFarsite;

public:
	long ThreadOrder;
	bool CanStillBurn, StillBurning, Started, DoSpots;
	double TimeMaxRem;  			 // maximum time remaining
	double EventTimeStep;

	X_HANDLE hBurnEvent;

	Embers embers;
	AreaPerimeter prod;
// 	FireEnvironment2* env;
	FELocalSite* fe;

	BurnThread( Farsite5 *_pFarsite);
	~BurnThread();
	X_HANDLE StartBurnThread(long ID);
	X_HANDLE GetThreadHandle();
	void SetRange(long CurrentFire, double SimTime, double CuumTimeIncrement,
		double TimeIncrement, double TimeMaxRem, long Begin, long End,
		long turn, bool FireIsUnderAttack, FireRing* ring);
};



class Burn : public Intersections
{
	long turn;
	long CurrentFire, CurrentPoint, NumInFires, CuumPoint, TotalPoints;
	double TimeIncrement, EventTimeStep, TimeMaxRem;
	long NumPerimThreads;
	bool FireIsUnderAttack;
	long SpotCount, CurThread, ThreadCount;

	FireRing* firering;
	BurnThread** burnthread;

	typedef struct
	{
		long FireNum;
		double TimeInc;
		void* next;
	} newinfire;
	newinfire* FirstInFire;		 // pointer to first inward fire
	newinfire* CurInFire;   	  // current inward fire
	newinfire* NextInFire;
	newinfire* TempInFire;		// temporary fire for allocation
	newinfire* TempInNext;

	void AllocNewInFire(long NewNum, double TimeInc);
	void GetNewInFire(long InFireNum);
	void FreeNewInFires();

	void PreBurn();
	void BurnMethod1();
	void BurnMethod2();
	bool AllocPerimThreads();
	void CloseAllPerimThreads();
	void FreePerimThreads();
	long StartPerimeterThreads_Equal();
	long StartPerimeterThreads_ActiveOnly();
	void ResumePerimeterThreads(long threadct);
	void ResumeSpotThreads(long threadct);
	Farsite5 *pFarsite;

public:
	bool CanStillBurn;
	bool StillBurning;
	long DistMethod, rastmake;
	double SIMTIME, CuumTimeIncrement;
	long* NumSpots;
	long TotalSpots, SpotFires;
	//Embers		embers;
	Rasterize *rast;
	AreaPerimeter prod;
//	FireEnvironment2* env;
	FELocalSite* fe;

	Burn(Farsite5 *_pFarsite);
	~Burn();
	void BurnIt(long CurrentFire);
	void EliminateFire(long FireNum);
	void DetermineSimLevelTimeStep();
	void ResetAllPerimThreads();
	void BurnSpotThreads();
	void SetSpotLocation(long Loc);
	Embers::spotdata* GetSpotData(double CurrentTimeStep);
};

#endif


