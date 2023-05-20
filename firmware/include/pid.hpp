#ifndef __PID_H__
#define __PID_H__

#include <ArduPID.h>

class PID {
public:
    PID(
        double* pGain,
        double* iGain,
        double* dGain,
        double* iMax,
        double* iMin,
        double* setpoint,
        double* input,
        double* output
    );

    void debug();
    void begin();

private:
    double* pGain;
    double* iGain;
    double* dGain;
    double* iMax;
    double* iMin;
    double* setpoint;
    double* input;
    double* output;
    ArduPID pidController;
};

#endif
