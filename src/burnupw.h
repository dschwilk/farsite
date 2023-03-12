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
//------------------------------------------------------------------------------
//
//	burnupc.h
//
//------------------------------------------------------------------------------
#ifndef BURNUPPROGRAM
#define BURNUPPROGRAM

#define MAXNO 20
#define MAXTYPES 3
#define MAXKL  (MAXNO * ( MAXNO + 1 ) / 2 + MAXNO )
#define MXSTEP 20
#define SWITCHOUTSIDE 0
#define SWITCHINSIDE 1
#include "splinex.h"
//#ifdef WIN32
#include "fsxsync.h"
//#endif


typedef struct
{
	double wdry;
	double htval;
	double fmois;
	double dendry;
	double sigma;
	double cheat;
	double condry;
	double tpig;
	double tchar;
	double ash;
} FuelStruct;


typedef struct
{
	double time;
	double wdf;
	double ff;
} BurnStruct;


class BurnUp
{
	BurnStruct* bs;
	long nruns, now, ntimes, number, ma;
	double fi, ti, u, d, tamb, ak, r0, dr, dt, wdf, dfm;
	double FintSwitch;
	double wd0, wg0, aa[20];
	double tis, dfi, tdf, fid, fimin, fistart;
	long NumAllocRegrData;
	double* w, * x, * y, * sig;
	double** ux, ** vx;

	double wdry[MAXNO], ash[MAXNO], htval[MAXNO];
	double fmois[MAXNO], dendry[MAXNO], sigma[MAXNO];
	double cheat[MAXNO], condry[MAXNO], alfa[MAXNO];
	double tpig[MAXNO], tchar[MAXNO];
	double flit[MAXNO], fout[MAXNO], work[MAXNO];
	double elam[MAXNO][MAXNO], alone[MAXNO];
	double area[MAXNO], fint[MAXNO];
	double xmat[MAXKL], tdry[MAXKL];
	double tign[MAXKL], tout[MAXKL];
	double wo[MAXKL], wodot[MAXKL];
	double diam[MAXKL], ddot[MAXKL];
	double qcum[MAXKL], tcum[MAXKL];
	double acum[MAXKL];
	double qdot[MAXKL][MXSTEP];

	//-----------------------------
	// emissions stuff here
	//-----------------------------
	double Smoldering[MAXNO + 1];  // last element contains duff mass burning rate
	double Flaming[MAXNO];
	//-----------------------------
	//-----------------------------

	long key[MAXNO];
	//char parts[12][MAXNO], list[12][MAXNO] , infile[12], outfil[12];
	//bool nohist;

	void Arrays();
	long loc(long k, long l);
	double func(double h, double theta);
	double ff(double x, double tpfi, double tpig);
	bool Start(double tis, long now, long* ncalls);
	void OverLaps();
	double FireIntensity();
	double DryTime(double enu, double theta);
	void Stash(char* HistFile, char* SnapFile, double tis, long now);
	void Sorter();
	double TIgnite(double tpdr, double tpig, double tpfi, double cond,
		double chtd, double fmof, double dend, double hbar);
	double TempF(double q, double r);
	void HeatExchange(double dia, double tf, double ts, double* hfm,
		double* hbar, double cond, double* en);
	void DuffBurn(double wdf, double dfm, double* dfi, double* tdf);
	void Step(double dt, double tin, double fid, long* ncalls);
	long Nint(double input);
	bool SetBurnStruct(double tis, long now);
	bool AllocBurnStruct();
	void FreeBurnStruct();
	bool AllocRegressionData(long ndata);
	void FreeRegressionData(long ndata);
	//	long 	LinearModel(long ma, long ndata, double *Chisq);
	void ResetEmissionsData();
	//void 	polynom(double x, double a[], long n);

public:
	char Message[256];

	BurnUp();
	~BurnUp();

	bool Burnup();

	// functions added to allow interface with windows
	bool StartLoop();
	bool BurnLoop();
	bool GetDatFile(char* InFile, long Number);
	bool SetFuelInfo(long NumParts, double* datastruct);
	bool SetFuelStruct(long NumParts, FuelStruct* fs);
	bool SetFuelDat(long NumParts, double* drywt, double* ashes, double* hots,
		double* fms, double* dryd, double* sigs, double* cpd, double* cond,
		double* tigi, double* tchi);
	bool SetFireDat(long NumIter, double Fi, double Ti, double U, double D,
		double Tamb, double R0, double Dr, double Dt, double Wdf, double Dfm);
	bool CheckData();
	bool Summary(char* OutFile);
	//     long		GetRegressionCoefficients(double **coefs, long *NumCalcs);
	//     long		RunPolynomialRegression(long Flaming);
	long GetSamplePoints(long Flaming, long Number);//, double **xpts, double **ypts, long *numcalcs);
	void SetFintSwitch(double fint);
	long GetFintSwitch();
	bool GetSample(long num, double* xpt, double* ypt);
	long GetNumOutputData();
	bool GetBurnStruct(long Num, BurnStruct* bs);
};


void polynom(double x, double a[], long);



#endif
