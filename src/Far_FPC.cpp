/********************************************************************
* Name: Far_FPC.cpp
* Desc: Farsite Progress Class,
*       Handles the percent of completion for the program.
*
*********************************************************************/
#include "Far_FPC.h"
//#include "Farsite5.h"

// Return state as a string
constexpr const char* ProgressStateToString(ProgressState s)
{
    switch (s)
    {
    case e_Idle:            return "Idle";
    case e_Starting:        return "Starting";
    case e_PreConditioning: return "Preconditioning";
    case e_Conditioning:    return "Conditioning";
    case e_FarsiteRunning:  return "Burning";
    case e_Done:            return "Complete";
    default: return "ERROR:UNKNOWN STATE";      
    }
}



/*******************************************************************/
FPC::FPC()
{
  Init();
}

FPC::~FPC()
{

}

/**********************************************************************/
void FPC::Init ()
{
    _state = e_Starting;
    _progress = 0;
 // f_PreProg = 0;
 // f_pcCond = 0;     /* is approximated to run */
 // f_pcFar = 0;

}

/***********************************************************************/

/* Set states */
/***********************************************************************/
void FPC::SetStarting()
{
    this->_state = e_Starting;
}

void FPC::SetPreConditioning ()
{
    this->_state = e_PreConditioning;
}


void FPC::SetConditioning ()
{
   this->_state = e_Conditioning;
}

/***********************************************************************/
void FPC::SetFarsiteRunning ()
{
   this->_state = e_FarsiteRunning;
}

void FPC::SetDone ()
{
   this->_state = e_Done;
}

void FPC::SetProgress(int progress)
{
    this->_progress = progress;  // atomic so no mutex needed
}

ProgressState FPC::GetProgressState() const
{
    return this->_state; // atomic so no mutex needed
}

int FPC::GetProgress() const
{
    return _progress;  // atomic so no mutex needed
}

const char* FPC::ProgressStateString() const
{
    return ProgressStateToString(this->_state);
}

/**********************************************************************
* Name:
* Desc:
*
*  Out: cr.....which section of the program is currently running
*  Ret: percent to completeion
***********************************************************************/
//float  FPC::GetProgress (Farsite5 *a_F5, CWindNinja2 *a_WN2, CFMC *a_cfmc, char cr[])
// float  FPC::GetProgress (Farsite5 *a_F5, CFMC *a_cfmc, char cr[])
// {
// /
//    /float f,pc;

  //	EnterCriticalSection(&CriSec);

//   strcpy (cr,"");

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
//  return 0;
//}

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
// float FPC::GetCondTime (Farsite5 *a_F5)
// {
// int i_StrYr, i_MthStart, i_DayStart, i_HourStart,date, min;
// int EndMin, StrMin, MaxMin;
//     i_StrYr     = a_F5->icf.a_Wtr[1].i_Year;     /* Start Conditioning For Daily Weather */
//     i_MthStart  = a_F5->icf.a_Wtr[1].i_Mth;    /*  on the second day */
//     i_DayStart  = a_F5->icf.a_Wtr[1].i_Day;
//     i_HourStart = 0;


// //   cfmc->Set_DateStart (i_StrYr, i_MthStart, i_DayStart, i_HourStart);

//    date = GetMCDate (i_MthStart, i_DayStart, i_StrYr);
//    min = MilToMin (i_HourStart);              /* Military time to minutes */
//    StrMin = (date * e_MinPerDay) + min;       /* to total minutes */

//    date = GetMCDate (a_F5->icf.i_FarsiteEndMth, a_F5->icf.i_FarsiteEndDay, a_F5->icf.i_FarsiteEndYear);
//    min = MilToMin (a_F5->icf.i_FarsiteEndHour);
//    EndMin = (date * e_MinPerDay) + min;      /* to total minutes */

//    MaxMin = EndMin - StrMin;

//    MaxMin = MaxMin + (int) a_F5->actual;
// //   d = (double) MaxMin;

// //   d += this->actual;


//   return MaxMin;
// }
