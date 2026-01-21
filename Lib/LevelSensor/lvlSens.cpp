#include "lvlSens.h"

/// @brief Initialise Water level sensor with initial parameter can be changed via api's that check and clamp values
/// @param tankMaxCapacity
/// @param tankInitialLevel
/// @param inputFlowRate
/// @param outputFlowRate
void LevelSensor::init(float tankMaxCapacity_liter,
                       float tankInitialLevel_liter)
{
    setTankMaxCapacity(tankMaxCapacity_liter);
    params.tankPreviousLevel_liter = tankInitialLevel_liter;
}

float LevelSensor::getTankCurrentLevel()
{
    return params.tankCurrentLevel_liter;
}

float LevelSensor::getTankMaxCapacity()
{
    return params.tankMaxCapacity_liter;
}

void LevelSensor::setTankMaxCapacity(float capacity_liter)
{
    params.tankMaxCapacity_liter = capacity_liter;
}

/// @brief
/// @param valveInputStroke percentage
/// @param valveOutputStroke percentage
/// @param valveInputFlowrate_lpm Liter/min
/// @param valveOutputFlowrate_lpm Liter/min
/// @param updateTime_ms Milliseconds
void LevelSensor::update(float valveInputStroke,
                         float valveOutputStroke,
                         float valveInputFlowrate_lpm,
                         float valveOutputFlowrate_lpm,
                         float updateTime_ms)
{
    float inputFlow = valveInputFlowrate_lpm * valveInputStroke / 100;
    float outputFlow = valveOutputFlowrate_lpm * valveOutputStroke / 100;
    float netFlow = inputFlow - outputFlow;
    params.tankCurrentLevel_liter = params.tankPreviousLevel_liter + netFlow * (updateTime_ms / 60000);
    params.tankPreviousLevel_liter = params.tankCurrentLevel_liter;
}