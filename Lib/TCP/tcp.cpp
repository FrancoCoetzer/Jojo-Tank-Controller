#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "tcp.h"
#include <ws2tcpip.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>

TcpServer server;

TcpServer::TcpServer()
    : serverSocket(INVALID_SOCKET),
      clientSocket(INVALID_SOCKET),
      rxPos(0),
      messageReady(false)
{
}

TcpServer::~TcpServer()
{
    stop();
}

bool TcpServer::init(uint16_t port)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return false;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
        return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr *)&addr, sizeof(addr)) != 0)
        return false;

    if (listen(serverSocket, 1) != 0)
        return false;

    // Non-blocking accept
    u_long mode = 1;
    ioctlsocket(serverSocket, FIONBIO, &mode);

    return true;
}

void TcpServer::stop()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    WSACleanup();
}

void TcpServer::poll()
{
    // Accept new client
    if (clientSocket == INVALID_SOCKET)
    {
        clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket != INVALID_SOCKET)
        {
            // Set client socket to non-blocking
            u_long mode = 1;
            ioctlsocket(clientSocket, FIONBIO, &mode);
        }
        return;
    }

    char c;
    int r = recv(clientSocket, &c, 1, 0);

    if (r == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {
            // No data available yet, just return
            return;
        }
        // Actual error - close connection
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        rxPos = 0;
        messageReady = false;
        return;
    }

    if (r == 0)
    {
        // Client disconnected gracefully
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        rxPos = 0;
        messageReady = false;
        return;
    }

    // Handle received character
    if (c == '\r')
    {
        return;
    }
    else if (c == '\n')
    {
        if (rxPos > 0)
        {
            rxBuffer[rxPos] = 0;
            messageReady = true;
        }
        rxPos = 0;
        return;
    }

    if (rxPos < sizeof(rxBuffer) - 1)
    {
        rxBuffer[rxPos++] = c;
    }
}

bool TcpServer::hasMessage()
{
    return messageReady;
}

const char *TcpServer::readMessage()
{
    messageReady = false;
    return rxBuffer;
}

void TcpServer::sendResponse(const char *text)
{
    if (clientSocket == INVALID_SOCKET)
        return;

    send(clientSocket, text, (int)strlen(text), 0);
    send(clientSocket, "\n", 1, 0);
}

void TcpServer::logInfo(const char *fmt, ...)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    char msg[256];
    snprintf(msg, sizeof(msg), "[%02d:%02d:%02d] [INFO] %s", t->tm_hour, t->tm_min, t->tm_sec, buf);
    sendResponse(msg);
}

void TcpServer::logError(const char *fmt, ...)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    char msg[256];
    snprintf(msg, sizeof(msg), "[%02d:%02d:%02d] [ERROR] %s", t->tm_hour, t->tm_min, t->tm_sec, buf);
    sendResponse(msg);
}
