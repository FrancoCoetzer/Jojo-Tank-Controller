#pragma once
#include <algorithm>

constexpr float MIN_STROKE_PCT = 0;
constexpr float MAX_STROKE_PCT = 100;
constexpr float MIN_FLOWRATE_LPM = 0;
constexpr float MAX_FLOWRATE_LPM = 5000;

class Valve
{
private:
    struct PARAM
    {
        float inputStroke_pct;
        float outputStroke_pct;
        float inputFlowRate_lpm;
        float outputFlowRate_lpm;
    };

    PARAM params;

public:
    enum class ValveStatus
    {
        OK,
        Clamped
    };
    void init(float inputValveInitialStroke_pct, float outputValveInitialStroke_pct, float inputFlowRate_lpm, float outputFlowRate_lpm);
    float getOutputStroke_pct();
    float getInputStroke_pct();
    float getInputflowrate_lpm();
    float getOutputflowrate_lpm();
    ValveStatus setOutputStroke_pct(float Stroke_pct);
    ValveStatus setInputStroke_pct(float Stroke_pct);
    ValveStatus setInputflowrate(float flowRate);
    ValveStatus setOutputflowrate(float flowRate);
};