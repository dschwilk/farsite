/*****************************************************************************/
/*                        Farsite Progress Class                    */

#include <atomic>

#ifndef Far_FPC_h
#define Far_FPC_h

enum ProgressState { e_Idle, e_Starting, e_PreConditioning, e_Conditioning, e_FarsiteRunning};

class FPC {
public:
    std::atomic<ProgressState> _state{e_Idle};
    std::atomic<int> _progress{0};

    // float f_PreProg;
    // float f_pcCond;     /* is approximated to run */
    // float f_pcFar;

    FPC();
    ~FPC();
    void Init();

//    float GetProgress(Farsite5 *a_F5, CFMC *a_cfmc, char cr[]);
    int GetProgress() const;
    ProgressState GetProgressState() const;
    const char* ProgressStateString() const;
    void SetProgress(int progress);
//    int SetTimeSlice (int i_NumNinRun, Farsite5 *aFar);
//    float GetCondTime (Farsite5 *a_F5);

    // states:
    void SetStarting();
    void SetPreConditioning ();
    void SetConditioning ();
    void SetFarsiteRunning ();
 };

#endif
