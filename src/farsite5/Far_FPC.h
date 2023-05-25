/*****************************************************************************/
/*                        Farsite Progress Class                    */

#include <atomic>

#ifndef Far_FPC_h
#define Far_FPC_h

enum ProgressState { e_Idle, e_Starting, e_PreConditioning, e_Conditioning, e_FarsiteRunning, e_Done};

class FPC {
public:
    std::atomic<ProgressState> _state{e_Idle};
    std::atomic<int> _progress{0};

    FPC();
    ~FPC();
    void Init();

    int GetProgress() const;
    ProgressState GetProgressState() const;
    const char* ProgressStateString() const;
    void SetProgress(int progress);

    // states:
    void SetStarting();
    void SetPreConditioning ();
    void SetConditioning ();
    void SetFarsiteRunning ();
    void SetDone ();
 };

#endif
