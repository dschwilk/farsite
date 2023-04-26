/*****************************************************************************/
/*                        Farsite Progress Class                    */

#include "Farsite5.h" 

#ifndef Far_FPC_h
#define Far_FPC_h

class FPC {
public:

    enum ProgressState { e_Start, e_PreCond, e_Cond, e_Far};
    
    ProgressState i_State;


    float f_PreProg;
    float f_pcCond;     /* is approximated to run */
    float f_pcFar;

    FPC();
    ~FPC();
    void Init();

    float GetProgress(Farsite5 *a_F5, CFMC *a_cfmc, char cr[]);

//    int SetTimeSlice (int i_NumNinRun, Farsite5 *aFar);
    float GetCondTime (Farsite5 *a_F5);

    void Set_CondRunning ();
    void Set_FarsiteRunning ();

 };

#endif
