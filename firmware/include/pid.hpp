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
    bool isStarted();
    void compute();
    void setIMin(double iMin);
    void setIMax(double iMax);
    void end();

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
    bool started;
};

extern PID pidController;

#endif
