#pragma once
#include <WinSock2.h>
#include <stdint.h>

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();

    bool init(uint16_t port);
    void stop();
    void poll();
    bool hasMessage();
    const char *readMessage();
    void sendResponse(const char *text);

    void logInfo(const char *fmt, ...);
    void logError(const char *fmt, ...);

private:
    SOCKET serverSocket;
    SOCKET clientSocket;

    char rxBuffer[256];
    size_t rxPos; // Changed from int to size_t to avoid warnings and runtime issues

    bool messageReady;
};

extern TcpServer server;