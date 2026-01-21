CC = g++
CFLAGS = -Wall
LIBS = -lwinmm -lws2_32
SRC = main.cpp Lib/Log/logWin.cpp Lib/LevelSensor/lvlSens.cpp Lib/Valve/valve.cpp Lib/TCP/tcp.cpp
OUT = main.exe

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

clean:
	del $(OUT)