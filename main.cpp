#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <mmsystem.h>
#include <string.h>

// Custom Libraries
#include "Lib/Log/logWin.h"
#include "Lib/TCP/tcp.h"
#include "Lib/LevelSensor/lvlSens.h"
#include "Lib/Valve/valve.h"

// Timer state
volatile uint64_t systemTimer = 0;

void CALLBACK TimerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    systemTimer++;
}

uint64_t millis()
{
    return systemTimer;
}

// Configuration constants
constexpr float TANK_MAX_CAPACITY = 10000;                     // Liters
constexpr float TANK_MAX_THRESHOLD = TANK_MAX_CAPACITY * 0.99; // Liters
constexpr float TANK_MIN_THRESHOLD = TANK_MAX_CAPACITY * 0.05; // Liters
constexpr float VALVE_INPUT_FLOW = 5000;                       // liter/min
constexpr float VALVE_OUTPUT_FLOW = 5000;                      // liter/min
constexpr uint16_t TANK_INITIAL_LEVEL = 5000;                  // liters

// Global instances
LevelSensor levelSensor;
Valve valve;
TcpServer reportServer;

// Function declarations
void processTcpMessage();

// Event timing constants
constexpr uint8_t EVENT1_TIME_MS = 10;
constexpr uint8_t EVENT2_TIME_MS = 100;
constexpr uint16_t EVENT3_TIME_MS = 5000;

// Event timing state
uint64_t event1PrevMilli = 0;
uint64_t event2PrevMilli = 0;
uint64_t event3PrevMilli = 0;

int main()
{
    Log.enable();

    // Request 1ms timer resolution
    timeBeginPeriod(1);
    MMRESULT timerID = timeSetEvent(1, 0, TimerCallback, 0, TIME_PERIODIC);

    if (timerID == 0)
    {
        Log.error("Failed to create timer");
        return 1;
    }

    Log.info("Timer running...");

    valve.init(100, 50, VALVE_INPUT_FLOW, VALVE_OUTPUT_FLOW);
    levelSensor.init(TANK_MAX_CAPACITY, TANK_INITIAL_LEVEL);

    if (!server.init(5000))
    {
        Log.error("Command server start failed");
        return -1;
    }
    Log.info("Command server running localhost:5000");

    if (!reportServer.init(5050))
    {
        Log.error("Report server start failed");
        return -1;
    }
    Log.info("Report server running localhost:5050");

    // Main event loop
    while (1)
    {
        // Event 1: TCP polling (10ms)
        if (millis() - event1PrevMilli > EVENT1_TIME_MS)
        {
            event1PrevMilli = millis();
            server.poll();
            reportServer.poll();

            if (server.hasMessage())
            {
                processTcpMessage();
            }
        }

        // Event 2: Tank level updates and control logic (100ms)
        if (millis() - event2PrevMilli > EVENT2_TIME_MS)
        {
            event2PrevMilli = millis();
            levelSensor.update(
                valve.getInputStroke_pct(),
                valve.getOutputStroke_pct(),
                valve.getInputflowrate_lpm(),
                valve.getOutputflowrate_lpm(),
                EVENT2_TIME_MS);

            // Auto-close input valve at high threshold
            if (levelSensor.getTankCurrentLevel() >= TANK_MAX_THRESHOLD &&
                valve.getInputStroke_pct() >= 100)
            {
                valve.setInputStroke_pct(0);
                server.logInfo("Input valve closed to prevent overflow. Tank Level: %.2f L",
                               levelSensor.getTankCurrentLevel());
                Log.info("Input valve closed");
            }
            // Auto-open input valve at low threshold
            else if (levelSensor.getTankCurrentLevel() <= TANK_MIN_THRESHOLD &&
                     valve.getInputStroke_pct() <= 0)
            {
                valve.setInputStroke_pct(100);
                server.logInfo("Input valve opened, tank below threshold. Tank Level: %.2f L",
                               levelSensor.getTankCurrentLevel());
                Log.info("Input valve opened");
            }
        }

        // Event 3: Status logging and telemetry (5000ms)
        if (millis() - event3PrevMilli > EVENT3_TIME_MS)
        {
            event3PrevMilli = millis();

            // Console logging
            Log.info("Tank level: %.2f L", levelSensor.getTankCurrentLevel());
            Log.info("Input valve stroke: %.2f %%", valve.getInputStroke_pct());
            Log.info("Output valve stroke: %.2f %%", valve.getOutputStroke_pct());
            Log.info("Input valve flow: %.2f L/min", valve.getInputflowrate_lpm());
            Log.info("Output valve flow: %.2f L/min", valve.getOutputflowrate_lpm());
            Log.println("\n");

            // Telemetry to report server
            reportServer.logInfo("Tank level: %.2f L", levelSensor.getTankCurrentLevel());
            reportServer.logInfo("Input valve stroke: %.2f %%", valve.getInputStroke_pct());
            reportServer.logInfo("Output valve stroke: %.2f %%", valve.getOutputStroke_pct());
            reportServer.logInfo("Input valve flow: %.2f L/min", valve.getInputflowrate_lpm());
            reportServer.logInfo("Output valve flow: %.2f L/min\n", valve.getOutputflowrate_lpm());
        }
    }
}

void processTcpMessage()
{
    float target = 0;
    const char *msg = server.readMessage();

    if (strcmp(msg, "GET LEVEL") == 0)
    {
        char resp[256];
        sprintf(resp, "LEVEL %.2f L", levelSensor.getTankCurrentLevel());
        server.logInfo(resp);
    }
    else if (sscanf(msg, "SET TARGET %f", &target) == 1)
    {
        if (valve.setOutputStroke_pct(target) == Valve::ValveStatus::Clamped)
        {
            Log.error("Output stroke out of bounds, value clamped");
            server.logError("Out of bounds, value clamped");
        }
        server.logInfo("Target set to: %.2f %%", valve.getOutputStroke_pct());
    }
    else if (sscanf(msg, "SET OUTPUT FLOW %f", &target) == 1)
    {
        if (valve.setOutputflowrate(target) == Valve::ValveStatus::Clamped)
        {
            Log.error("Output flow rate out of bounds, value clamped");
            server.logError("Out of bounds, value clamped");
        }
        server.logInfo("Output flow set to: %.2f L/min", valve.getOutputflowrate_lpm());
    }
    else if (sscanf(msg, "SET INPUT FLOW %f", &target) == 1)
    {
        if (valve.setInputflowrate(target) == Valve::ValveStatus::Clamped)
        {
            Log.error("Input flow rate out of bounds, value clamped");
            server.logError("Out of bounds, value clamped");
        }
        server.logInfo("Input flow set to: %.2f L/min", valve.getInputflowrate_lpm());
    }
    else if (strcmp(msg, "GET VALVES") == 0)
    {
        char resp[256];
        sprintf(resp, "Input valve stroke: %.2f %% Output valve stroke: %.2f %%",
                valve.getInputStroke_pct(), valve.getOutputStroke_pct());
        Log.info(resp);
        server.logInfo(resp);
    }
    else if (strcmp(msg, "GET FLOW") == 0)
    {
        char resp[256];
        sprintf(resp, "Input valve flow rate: %.2f L/min Output valve flow rate: %.2f L/min",
                valve.getInputflowrate_lpm(), valve.getOutputflowrate_lpm());
        Log.info(resp);
        server.logInfo(resp);
    }
    else
    {
        server.logError("UNKNOWN_COMMAND");
        Log.error("Unknown command received from TCP client: %s", msg);
    }
}
