#include "pid.hpp"

PID::PID(
    double* pGain,
    double* iGain,
    double* dGain,
    double* iMax,
    double* iMin,
    double* setpoint,
    double* input,
    double* output
) {
    this->pGain = pGain;
    this->iGain = iGain;
    this->dGain = dGain;
    this->iMax = iMax;
    this->iMin = iMin;
    this->setpoint = setpoint;
    this->input = input;
    this->output = output;

    this->started = false;

    this->pidController = ArduPID();
}

void PID::debug() {
    if (!this->started) {
        return;
    }
    this->pidController
        .debug(&Serial, "PID",  PRINT_INPUT    |
                                PRINT_OUTPUT   |
                                PRINT_SETPOINT |
                                PRINT_BIAS     |
                                PRINT_P        |
                                PRINT_I        |
                                PRINT_D);
}

void PID::begin() {
    this->pidController.begin(
        this->input,
        this->output,
        this->setpoint,
        *this->pGain,
        *this->iGain,
        *this->dGain
    );
    Serial.printf("PID: %f %f %f\n", *this->pGain, *this->iGain, *this->dGain);
    this->pidController.setWindUpLimits(*this->iMin, *this->iMax);
    this->pidController.setOutputLimits(0, 255);
    this->pidController.compute();
    this->started = true;
}

bool PID::isStarted() {
    return this->started;
}

void PID::compute() {
    if (!this->started) {
        return;
    }
    this->pidController.compute();
}

void PID::setIMin(double iMin) {
    this->pidController.setWindUpLimits(iMin, *this->iMax);
    *(this->iMin) = iMin;
}

void PID::setIMax(double iMax) {
    this->pidController.setWindUpLimits(*this->iMin, iMax);
    *(this->iMax) = iMax;
}

void PID::end() {
    this->pidController.stop();
    // Set the output to 0 to prevent the heater from turning on
    *(this->output) = 0;
    this->started = false;
}
