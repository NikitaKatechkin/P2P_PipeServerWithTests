#include "CustomServer.h"

/**
static const LPCTSTR PIPE_NAME = TEXT("\\\\.\\pipe\\mynamedpipe");
static const DWORD PIPE_OPEN_MODE = PIPE_ACCESS_DUPLEX;
static const DWORD PIPE_MODE = (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT);
static const DWORD PIPE_CAPACITY = PIPE_UNLIMITED_INSTANCES;
static const DWORD PIPE_TIMEOUT = NMPWAIT_USE_DEFAULT_WAIT;
static const LPSECURITY_ATTRIBUTES PIPE_SECURITY_SETTINGS = nullptr;

CustomServer::CustomServer(const LPCTSTR l_pipe_name):
	m_pipe_name(l_pipe_name)
{
    m_pipe = CreateNamedPipe(
        m_pipe_name,
        PIPE_OPEN_MODE,
        PIPE_MODE,
        PIPE_CAPACITY,
        DEFAULT_BUFSIZE,
        DEFAULT_BUFSIZE,
        PIPE_TIMEOUT,
        PIPE_SECURITY_SETTINGS);


    if (m_pipe == INVALID_HANDLE_VALUE)
    {
        throw std::exception("[CustomServer::CustomServer()] Failed to create a named pipe server");
    }
}

CustomServer::~CustomServer()
{
    CloseHandle(m_pipe);
}

void CustomServer::WaitConnection()
{
    std::cout << "[SERVER]: Main thread awaiting client connection..." << std::endl;

    m_is_connected = ConnectNamedPipe(m_pipe, nullptr);
    if (m_is_connected == false)
    {
        m_is_connected = (GetLastError() == ERROR_PIPE_CONNECTED);
    }

    if (m_is_connected == true)
    {
        std::cout << "[ConnectNamedPipe()] Client succesfully connected.";
        std::cout << std::endl;
    }
    else
    {
        std::cout << "[ConnectNamedPipe()] Failed to connect client";
        std::cout << std::endl;
    }
}

std::wstring CustomServer::Recieve()
{
    std::wstring result = TEXT("");

    if (m_is_connected)
    {
        TCHAR read_buffer[DEFAULT_BUFSIZE] = { '\0' };
        DWORD bytes_processed = 0;

        bool reading_success = ReadFile(
            m_pipe,
            read_buffer,
            DEFAULT_BUFSIZE * sizeof(TCHAR),
            &bytes_processed,
            nullptr);

        if (!reading_success)
        {
            throw std::exception("[CustomServer::Recieve()] Error while reading client message.");
        }

        result = (read_buffer);
    }
    else
    {
        std::cout << "Pipe not connected. Can not read data from it.";
        std::cout << "Empty data was returned." << std::endl;
    }
    
    return result;
}

void CustomServer::Send(const std::wstring& message)
{
    if (m_is_connected)
    {
        DWORD bytes_processed = 0;

        bool writing_success = WriteFile(
            m_pipe,
            message.c_str(),
            DEFAULT_BUFSIZE * sizeof(TCHAR),
            &bytes_processed,
            nullptr);

        if (!writing_success)
        {
            throw std::exception("[CustomServer::Send()] Error while writing server message.");
        }
        else
        {
            std::cout << bytes_processed << " bytes of data was sent. ";
            std::wcout << "[TRANSFERED TEXT] = " << message << std::endl;
        }
    }
    else
    {
        std::cout << "Pipe not connected. Can not send data to it.";
    }
}

/**
void CustomServer::RegistrationLoop()
{
    std::cout << "[SERVER]: Main thread awaiting client connection..." << std::endl;
    
    while (true)
    {
        m_pipe = CreateNamedPipe(
            m_pipe_name,
            PIPE_OPEN_MODE,
            PIPE_MODE,
            PIPE_CAPACITY,
            BUFSIZE,
            BUFSIZE,
            PIPE_TIMEOUT,
            PIPE_SECURITY_SETTINGS);

        if (m_pipe == INVALID_HANDLE_VALUE)
        {
            throw std::exception("[CreateNamedPipe()] Failed to create a named pipe");
        }

        bool connection_success = ConnectNamedPipe(m_pipe, nullptr);
        if (connection_success == false)
        {
            connection_success = (GetLastError() == ERROR_PIPE_CONNECTED);
        }

        if (connection_success)
        {
            

            
            while (true)
            {
                if (!this->Recieve())
                {
                    break;
                }
                else
                {
                    this->ShowLastRecievedMessage();
                }


                if (!this->Send())
                {
                    break;
                }
            }
            

            std::thread new_client_ping_pong(&CustomServer::PingPongWithClient, this, this);
            new_client_ping_pong.join();
            //new_client_ping_pong.detach();
            //std::this_thread::yield();
            
        }
        else
        {
            std::cout << "[ConnectNamedPipe()] Client connection did not succeed. Closing the pipe...";
            std::cout << std::endl;
            CloseHandle(m_pipe);
        }
    }
}

void CustomServer::PingPongWithClient(CustomServer* l_server)
{
    std::cout << "[ConnectNamedPipe()] Client succesfully connected. Start ping-pong process...";
    std::cout << std::endl;

    while (true)
    {
        if (!l_server->Recieve())
        {
            break;
        }
        else
        {
            l_server->ShowLastRecievedMessage();
        }

        if (!l_server->Send())
        {
            break;
        }
    }

    std::cout << "[ConnectNamedPipe()] Client disconnected.";
    std::cout << std::endl;
}

void CustomServer::ShowLastRecievedMessage()
{
    //_tprintf(TEXT("[CLIENT]: %s\n"), read_buffer);

}
**/

//static const LPCTSTR PIPE_NAME = TEXT("\\\\.\\pipe\\mynamedpipe");
static const DWORD PIPE_OPEN_MODE = PIPE_ACCESS_DUPLEX;
static const DWORD PIPE_MODE = (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT);
static const DWORD PIPE_CAPACITY = PIPE_UNLIMITED_INSTANCES;
static const DWORD PIPE_TIMEOUT = NMPWAIT_USE_DEFAULT_WAIT;
static const LPSECURITY_ATTRIBUTES PIPE_SECURITY_SETTINGS = nullptr;
//static const DWORD DEFAULT_BUFSIZE = 512;

CustomServer::CustomServer(const std::wstring& l_pipe_name) :
    CustomNetworkAgent(l_pipe_name)
{
    m_pipe = CreateNamedPipe(
        m_pipe_name.c_str(),
        PIPE_OPEN_MODE,
        PIPE_MODE,
        PIPE_CAPACITY,
        DEFAULT_BUFSIZE,
        DEFAULT_BUFSIZE,
        PIPE_TIMEOUT,
        PIPE_SECURITY_SETTINGS);


    if (m_pipe == INVALID_HANDLE_VALUE)
    {
        throw std::exception("[CustomServer::CustomServer()] Failed to create a named pipe server");
    }
}

CustomServer::~CustomServer()
{
    Disconnect();
    CloseHandle(m_pipe);
}

bool CustomServer::Disconnect()
{
    if (m_is_connected == true)
    {
        m_is_connected = false;
        DisconnectNamedPipe(m_pipe);
    }
    
    return !m_is_connected;
}

bool CustomServer::Connect()
{
    if (m_is_connected == false)
    {
        std::cout << "[SERVER]: Main thread awaiting client connection..." << std::endl;

        m_is_connected = ConnectNamedPipe(m_pipe, nullptr);
        if (m_is_connected == false)
        {
            m_is_connected = (GetLastError() == ERROR_PIPE_CONNECTED);
        }

        if (m_is_connected == true)
        {
            std::cout << "[ConnectNamedPipe()] Client succesfully connected.";
            std::cout << std::endl;
        }
        else
        {
            std::cout << "[ConnectNamedPipe()] Failed to connect client";
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "[SERVICE INFO] Server already connected to client." << std::endl;
    }
    
    return m_is_connected;
}