/**
---------WORKING VERSION-----------
#pragma once 

#include <iostream>
#include <cstdio>
#include <thread>

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

const DWORD DEFAULT_BUFSIZE = 512;
const LPCTSTR DEFAULT_PIPE_NAME = TEXT("\\\\.\\pipe\\mynamedpipe");

class CustomServer final
{
public:
	CustomServer(const LPCTSTR l_pipe_name = nullptr);
	~CustomServer();

	void WaitConnection();

	std::wstring Recieve();
	void Send(const std::wstring& message);
private:
	HANDLE m_pipe = INVALID_HANDLE_VALUE;
	LPCTSTR m_pipe_name = nullptr;
	
	bool m_is_connected = false;
};
**/

#pragma once

#include <TestLib/CustomNetworkAgent.h>

class CustomServer final : public CustomNetworkAgent
{
public:
	CustomServer(const std::wstring& l_pipe_name);
	virtual ~CustomServer();

	virtual bool Connect() override;
	virtual bool Disconnect() override;
private:
};