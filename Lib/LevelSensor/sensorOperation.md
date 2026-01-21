# Sensor Operation Overview

## Overview
The Sensor:
- Reports current tank level (e.g. 0–100 % or litres)
- Water level increases based on inlet valve opening
- Water level decreases based on outlet valve opening
- Level is bounded (e.g. 0–100%)
Bonus:
- Intermittent sensor dropouts
- Frozen sensor values

## Plan

To create a simulation of a water level sensor, I will need an update function to update the sensor with some important states like:
- Input valve state(0 - 100%)
- Output valve state(0 - 100%)
- input and output valve flow rates
- tank capacity

these parameters will allow me to calculate the time it takes to fill the tank, empty the tank
eg.
```
tankMaxCapacity = 2000L
inletMaxflowrate = 200L/min
outletMaxFlowrate = 200L/min

level = previousLevel + (flowIn - flowOut) x deltaTime
```
