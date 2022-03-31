#include "pch.h"


#include "../TestServer/CustomServer.h"

#include <thread>

static const LPCTSTR DEFAULT_PIPE_NAME = L"\\\\.\\pipe\\mynamedpipe";

namespace TestToolbox
{
	void TestConnect(HANDLE& m_pipe);
	void TestWrite(HANDLE& m_pipe, const std::wstring& message);
	void TestRead(HANDLE& m_pipe, std::wstring& buffer);
}

void TestToolbox::TestConnect(HANDLE& m_pipe)
{
	for (unsigned int index = 0; index < 5; index++)
	{
		m_pipe = CreateFile(
			DEFAULT_PIPE_NAME,   // pipe name 
			GENERIC_READ | GENERIC_WRITE,  // read and write access ,
			NULL,              // no sharing 
			nullptr,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			NULL,              // default attributes 
			NULL);

		if (m_pipe != INVALID_HANDLE_VALUE)
		{
			DWORD pipe_mode = PIPE_READMODE_MESSAGE;
			SetNamedPipeHandleState(
				m_pipe,    // pipe handle 
				&pipe_mode,  // new pipe mode 
				NULL,     // don't set maximum bytes 
				NULL);

			break;
		}

		WaitNamedPipe(DEFAULT_PIPE_NAME, 5000);
	}
}

void TestToolbox::TestWrite(HANDLE& m_pipe, const std::wstring& message)
{
	DWORD bytes_processed = 0;
	bool writing_success = WriteFile(
		m_pipe,
		message.c_str(),
		DEFAULT_BUFSIZE * sizeof(TCHAR),
		&bytes_processed,
		nullptr);
}

void TestToolbox::TestRead(HANDLE& m_pipe, std::wstring& buffer)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	TCHAR read_buffer[DEFAULT_BUFSIZE] = { '\0' };
	DWORD bytes_processed = 0;

	bool reading_success = ReadFile(
		m_pipe,
		read_buffer,
		DEFAULT_BUFSIZE * sizeof(TCHAR),
		&bytes_processed,
		nullptr);

	buffer = read_buffer;
}

TEST(serverTestCase, ConstructorTest)
{
	try
	{
		CustomServer server(L"jglgefkuygear");
		FAIL() << "[CustomServer::CustomServer()] Constructor should fail. \n";
	}
	catch (const std::exception& e)
	{
		EXPECT_EQ(std::string(e.what()),
			"[CustomServer::CustomServer()] Failed to create a named pipe server");
	}

	try
	{
		CustomServer server(DEFAULT_PIPE_NAME);
	}
	catch (const std::exception&)
	{
		FAIL() << "[CustomServer::CustomServer()] Constructor should not fail. \n";
	}
}

TEST(serverTestCase, ConnectionHandlingTest)
{
	CustomServer server(DEFAULT_PIPE_NAME);

	HANDLE m_pipe = INVALID_HANDLE_VALUE;
	std::thread th(TestToolbox::TestConnect, std::ref(m_pipe));
	
	EXPECT_EQ(server.Connect(), true);

	th.join();

	EXPECT_EQ(server.Disconnect(), true);
	CloseHandle(m_pipe);

	m_pipe = INVALID_HANDLE_VALUE;
	th = std::thread(TestToolbox::TestConnect, std::ref(m_pipe));

	EXPECT_EQ(server.Connect(), true);

	th.join();

	EXPECT_EQ(server.Disconnect(), true);
	CloseHandle(m_pipe);
}

TEST(NetAgentTestCase, RecieveTest)
{
	const std::wstring message = L"TEST STRING";
	CustomServer server(DEFAULT_PIPE_NAME);

	HANDLE m_pipe = INVALID_HANDLE_VALUE;

	std::thread th(TestToolbox::TestConnect, std::ref(m_pipe));

	server.Connect();

	th.join();

	th = std::thread(TestToolbox::TestWrite, std::ref(m_pipe), std::ref(message));
	
	/**
	DWORD bytes_processed = 0;
	bool writing_success = WriteFile(
		m_pipe,
		message.c_str(),
		DEFAULT_BUFSIZE * sizeof(TCHAR),
		&bytes_processed,
		nullptr);
	std::cout << "here" << std::endl;
	**/

	std::wstring result;
	EXPECT_EQ(server.Recieve(result), true);

	th.join();

	server.Disconnect();
	CloseHandle(m_pipe);

	EXPECT_EQ(result, message);
}

TEST(NetAgentTestCase, SendTest)
{
	const std::wstring message = L"TEST STRING";
	CustomServer server(DEFAULT_PIPE_NAME);

	HANDLE m_pipe = INVALID_HANDLE_VALUE;

	std::thread th(TestToolbox::TestConnect, std::ref(m_pipe));

	server.Connect();

	th.join();

	std::wstring result;
	th = std::thread(TestToolbox::TestRead, std::ref(m_pipe), std::ref(result));
	/**
	DWORD bytes_processed = 0;
	bool writing_success = WriteFile(
		m_pipe,
		message.c_str(),
		DEFAULT_BUFSIZE * sizeof(TCHAR),
		&bytes_processed,
		nullptr);
	**/

	EXPECT_EQ(server.Send(message), true);

	th.join();
	server.Disconnect();
	CloseHandle(m_pipe);


	EXPECT_EQ(result, message);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}