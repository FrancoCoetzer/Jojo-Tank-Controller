#pragma once
#include <stdint.h>
#include <algorithm>

// void sensorUpdate(tankCapacity, valveInPercentage, valveOutPercentage, valveInMaxFlowrate, ValveOutMaxFlowrate);
// void getSensorCurrentLevel();
// void setSensorDropout_ms(); //if sensor drops out
constexpr float MAX_FLOWRATES_LPM = 1000;
constexpr float MIN_FLOWRATES_LPM = 0;

class LevelSensor
{
private:
    struct LEVELSENSORPARAMS
    {
        float tankMaxCapacity_liter;
        float tankPreviousLevel_liter;
        float tankCurrentLevel_liter;
    };

    LEVELSENSORPARAMS params;

public:
    void init(float tankMaxCapacity_liter,
              float tankInitialLevel_liter);

    float getTankMaxCapacity();
    float getTankCurrentLevel();
    void setTankMaxCapacity(float capacity_liter);

    void update(float valveInputStroke,
                float valveOutputStroke,
                float valveInputFlowrate_lpm,
                float valveOutputFlowrate_lpm,
                float updateTime_ms);
};