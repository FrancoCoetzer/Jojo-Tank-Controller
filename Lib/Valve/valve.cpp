#include "valve.h"
#include <algorithm>

void Valve::init(float inputValveInitialStroke_pct, float outputValveInitialStroke_pct, float inputFlowRate_lpm, float outputFlowRate_lpm)
{
    setInputStroke_pct(inputValveInitialStroke_pct);
    setOutputStroke_pct(outputValveInitialStroke_pct);
    setInputflowrate(inputFlowRate_lpm);
    setOutputflowrate(outputFlowRate_lpm);
}

float Valve::getOutputStroke_pct()
{
    return params.outputStroke_pct;
}

float Valve::getInputStroke_pct()
{
    return params.inputStroke_pct;
}

float Valve::getInputflowrate_lpm()
{
    return params.inputFlowRate_lpm;
}

float Valve::getOutputflowrate_lpm()
{
    return params.outputFlowRate_lpm;
}

Valve::ValveStatus Valve::setOutputStroke_pct(float stroke_pct)
{
    ValveStatus status = ValveStatus::OK;

    if (stroke_pct > MAX_STROKE_PCT || stroke_pct < MIN_STROKE_PCT)
    {
        status = ValveStatus::Clamped;
    }

    params.outputStroke_pct = std::clamp(stroke_pct, MIN_STROKE_PCT, MAX_STROKE_PCT);
    return status;
}

Valve::ValveStatus Valve::setInputStroke_pct(float stroke_pct)
{
    ValveStatus status = ValveStatus::OK;

    if (stroke_pct > MAX_STROKE_PCT || stroke_pct < MIN_STROKE_PCT)
    {
        status = ValveStatus::Clamped;
    }

    params.inputStroke_pct = std::clamp(stroke_pct, MIN_STROKE_PCT, MAX_STROKE_PCT);
    return status;
}

Valve::ValveStatus Valve::setInputflowrate(float flowRate)
{
    ValveStatus status = ValveStatus::OK;

    if (flowRate > MAX_FLOWRATE_LPM || flowRate < MIN_FLOWRATE_LPM)
    {
        status = ValveStatus::Clamped;
    }

    params.inputFlowRate_lpm = std::clamp(flowRate, MIN_FLOWRATE_LPM, MAX_FLOWRATE_LPM);
    return status;
}

Valve::ValveStatus Valve::setOutputflowrate(float flowRate)
{
    ValveStatus status = ValveStatus::OK;

    if (flowRate > MAX_FLOWRATE_LPM || flowRate < MIN_FLOWRATE_LPM)
    {
        status = ValveStatus::Clamped;
    }

    params.outputFlowRate_lpm = std::clamp(flowRate, MIN_FLOWRATE_LPM, MAX_FLOWRATE_LPM);
    return status;
}