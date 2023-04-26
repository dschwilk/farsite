/********************************************************************
* Name: Far_FPC.cpp
* Desc: Farsite Progress Class,
*       Handles the percent of completion for the program.
*
*********************************************************************/
#include "Far_FPC.h"
#include "Farsite5.h"

/*******************************************************************/
FPC::FPC()
{
  Init();
}

FPC::~FPC()
{
 // DeleteCriticalSection(&CriSec);
}

/**********************************************************************/
void FPC::Init ()
{

 i_State = e_Start;
 f_PreProg = 0;

 f_pcCond = 0;     /* is approximated to run */
 f_pcFar = 0;

	//InitializeCriticalSection(&CriSec);

}

/***********************************************************************/


/***********************************************************************/
void FPC::Set_CondRunning ()
{
   this->i_State = e_Cond;
}

/***********************************************************************/
void FPC::Set_FarsiteRunning ()
{
	 // EnterCriticalSection(&CriSec);
   this->i_State = e_Far;
	//  LeaveCriticalSection(&CriSec);
}

/**********************************************************************
* Name:
* Desc:
*
*  Out: cr.....which section of the program is currently running
*  Ret: percent to completeion
***********************************************************************/
//float  FPC::GetProgress (Farsite5 *a_F5, CWindNinja2 *a_WN2, CFMC *a_cfmc, char cr[])
float  FPC::GetProgress (Farsite5 *a_F5, CFMC *a_cfmc, char cr[])
{
//float f,pc;

  //	EnterCriticalSection(&CriSec);

   strcpy (cr,"");

/* NOTE things run in this order Start, Cond, Farsite */

//   if ( this->i_State == e_Start ) {
//      strcpy (cr,"Startup");
//      return LeaCriSec(0); }


//   if ( f_pcFar == 0 ) {    /* this might occur, means ninja didn't start yet */
//      strcpy (cr,"Startup");
//      return LeaCriSec(0); }

//   if ( i_State == e_Cond ) {
//      strcpy (cr,"Conditiong");
//      pc = a_cfmc->Get_ProgressF();   /* Comes back 0 -> 1.0 */
//      pc = pc * this->f_pcCond;       /* allocted portion of total run time */
//      f = this->f_pcNinja + pc;       /* add it to previous time spent running */
//      return LeaCriSec (f); }

//   if ( i_State == e_Far ) {
//      strcpy (cr, "FarsiteBurning");
//      pc = a_F5->GetFarsiteProgress();
//      f = this->f_pcNinja + this->f_pcCond;
//      f = f + ( pc * this->f_pcFar );
//      return LeaCriSec (f); }

// /* Shouldn't get here, everthing should have been taken care of above */
//   strcpy (cr,"Unknown process state");
//  // LeaveCriticalSection(&CriSec);
  return 0;
}

/*******************************************************************************
* Convenient way for caller to leave crit section and return a value
*
*******************************************************************************/
// float FPC::LeaCriSec (float f)
// {
//   //LeaveCriticalSection(&CriSec);
//   return f;
// }



/************************************************************************
*
*
*
*************************************************************************/
float FPC::GetCondTime (Farsite5 *a_F5)
{
int i_StrYr, i_MthStart, i_DayStart, i_HourStart,date, min;
int EndMin, StrMin, MaxMin;
    i_StrYr     = a_F5->icf.a_Wtr[1].i_Year;     /* Start Conditioning For Daily Weather */
    i_MthStart  = a_F5->icf.a_Wtr[1].i_Mth;    /*  on the second day */
    i_DayStart  = a_F5->icf.a_Wtr[1].i_Day;
    i_HourStart = 0;


//   cfmc->Set_DateStart (i_StrYr, i_MthStart, i_DayStart, i_HourStart);

   date = GetMCDate (i_MthStart, i_DayStart, i_StrYr);
   min = MilToMin (i_HourStart);              /* Military time to minutes */
   StrMin = (date * e_MinPerDay) + min;       /* to total minutes */

   date = GetMCDate (a_F5->icf.i_FarsiteEndMth, a_F5->icf.i_FarsiteEndDay, a_F5->icf.i_FarsiteEndYear);
   min = MilToMin (a_F5->icf.i_FarsiteEndHour);
   EndMin = (date * e_MinPerDay) + min;      /* to total minutes */

   MaxMin = EndMin - StrMin;

   MaxMin = MaxMin + (int) a_F5->actual;
//   d = (double) MaxMin;

//   d += this->actual;


  return MaxMin;
}
