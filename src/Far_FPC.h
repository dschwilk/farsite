/*****************************************************************************/
/*                        Farsite Progress Class                    */

#include "Farsite5.h" 

#ifndef Far_FPC_h
#define Far_FPC_h

class FPC {
public:
#define e_Start   0
#define e_Ninja   1
#define e_NinjaPrep 2    /* Ninja preparing to run */
#define e_PreCond 3
#define e_Cond    4
#define e_Far     5
   int i_State;


   float f_WNToDo;       /* How Many ninja runs are needed */
   float f_WNComplete;   /* How many competed so far */


  float f_PreProg;

  float f_pcNinja;    /* percentages of time that each section */
  float f_pcCond;     /* is approximated to run */
  float f_pcFar;

   FPC();
   ~FPC();
   void Init();
   //float GetProgress(Farsite5 *a_F5, CWindNinja2 *a_WN2, CFMC *a_cfmc, char cr[]);
   float GetProgress(Farsite5 *a_F5, CFMC *a_cfmc, char cr[]);
  // float WindNinja_Progress (CWindNinja2 *a_WN2);
   float LeaCriSec (float f);


   int SetTimeSlice (int i_NumNinRun, Farsite5 *aFar);
   float GetCondTime (Farsite5 *a_F5);

   void Set_NinjaRunning (float f);
   void Set_NinjaPrep ();
   void Set_CondRunning ();
   void Set_FarsiteRunning ();

   void Set_NinjaInc();

 };

#endif
