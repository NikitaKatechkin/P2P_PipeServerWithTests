#include "CustomServer.h"

static const LPCTSTR DEFAULT_PIPE_NAME = L"\\\\.\\pipe\\mynamedpipe";

int main()
{
    CustomServer test_server(DEFAULT_PIPE_NAME);
    test_server.Connect();

    std::wstring buffer;
    std::wcout << L"[CLIENT]: " << L"{ STATUS - " << test_server.Recieve(buffer) << L" } ";
    std::wcout << buffer << std::endl;

    test_server.Send(L"Message from server.");

    test_server.Disconnect();
    test_server.Connect();

    buffer = L"";
    std::wcout << L"[CLIENT]: " << L"{ STATUS - " << test_server.Recieve(buffer) << L" } ";
    std::wcout << buffer << std::endl;

    test_server.Send(L"Message from server.");

    system("pause");
    return 0;
}

/**
#include <iostream>
#include <cstdio>
#include <thread>

#include <windows.h>
#include <tchar.h>

const LPCTSTR PIPE_NAME = TEXT("\\\\.\\pipe\\mynamedpipe");
const DWORD PIPE_OPEN_MODE = PIPE_ACCESS_DUPLEX;
const DWORD PIPE_MODE = (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT);
const DWORD PIPE_CAPACITY = PIPE_UNLIMITED_INSTANCES;
const DWORD PIPE_TIMEOUT = NMPWAIT_USE_DEFAULT_WAIT;
const DWORD BUFSIZE = 512;
const LPSECURITY_ATTRIBUTES PIPE_SECURITY_SETTINGS = nullptr;


int main()
{
    std::cout << "[SERVER]: Main thread awaiting client connection..." << std::endl;

    while (true)
    {
        HANDLE server_pipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_OPEN_MODE,
            PIPE_MODE,
            PIPE_CAPACITY,
            BUFSIZE,
            BUFSIZE,
            PIPE_TIMEOUT,
            PIPE_SECURITY_SETTINGS);

        if (server_pipe == INVALID_HANDLE_VALUE)
        {
            throw std::exception("[CreateNamedPipe()] Failed to create a named pipe");
        }

        bool connection_success = ConnectNamedPipe(server_pipe, nullptr);
        if (connection_success == false)
        {
            connection_success = (GetLastError() == ERROR_PIPE_CONNECTED);
        }

        if (connection_success)
        {
            std::cout << "[ConnectNamedPipe()] Client succesfully connected. Start ping-pong process...";
            std::cout << std::endl;

            TCHAR read_buffer[BUFSIZE];
            TCHAR write_buffer[BUFSIZE];

            DWORD bytes_processed = 0;

            while (true)
            {
                //READ TO BUFFER FROM PIPE
                bool reading_success = ReadFile(
                    server_pipe,
                    read_buffer,
                    BUFSIZE * sizeof(TCHAR),
                    &bytes_processed,
                    nullptr);

                if (!reading_success || bytes_processed == 0)
                {
                    //throw std::exception("[ReadFile()] Error while reading client message.");
                    break;
                }

                _tprintf(TEXT("[CLIENT]: %s\n"), read_buffer);

                //WRTIE FROM BUFFER TO PIPE
                bytes_processed = 0;
                wcscpy_s(write_buffer, TEXT("Message recieved"));

                bool writing_success = WriteFile(
                    server_pipe,
                    write_buffer,
                    BUFSIZE * sizeof(TCHAR),
                    &bytes_processed,
                    nullptr);

                if (!writing_success ||
                    (lstrlen(TEXT("Message recieved") + 1) * sizeof(TCHAR) != bytes_processed))
                {
                    //throw std::exception("[ReadFile()] Error while writing server message.");
                    break;
                }
            }
        }
        else
        {
            std::cout << "[ConnectNamedPipe()] Client connection did not succeed. Closing the pipe...";
            std::cout << std::endl;
            CloseHandle(server_pipe);
        }
    }


    return 0;
}
**/