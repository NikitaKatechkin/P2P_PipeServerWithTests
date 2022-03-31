#include "pch.h"

#include "../TestClient/CustomClient.h"

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
	m_pipe = CreateNamedPipe(
		DEFAULT_PIPE_NAME,
		PIPE_ACCESS_DUPLEX,
		(PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT),
		PIPE_UNLIMITED_INSTANCES,
		DEFAULT_BUFSIZE,
		DEFAULT_BUFSIZE,
		NMPWAIT_USE_DEFAULT_WAIT,
		nullptr
	);

	ConnectNamedPipe(m_pipe, nullptr);
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

TEST(ClientTestCase, ConstructorTest)
{
	try
	{
		CustomClient client(L"jglgefkuygear");
	}
	catch (const std::exception&)
	{
		FAIL() << "[CustomClient::CustomClient()] Constructor should not fail. \n";
	}

	try
	{
		CustomClient client(DEFAULT_PIPE_NAME);
	}
	catch (const std::exception&)
	{
		FAIL() << "[CustomClient::CustomClient()] Constructor should not fail. \n";
	}
}

TEST(ClientTestCase, ConnectionHandlingTest)
{
	CustomClient client(DEFAULT_PIPE_NAME);

	HANDLE m_pipe = INVALID_HANDLE_VALUE;
	std::thread th(TestToolbox::TestConnect, std::ref(m_pipe));
	std::this_thread::sleep_for(std::chrono::seconds(1));
	
	EXPECT_EQ(client.Connect(), true);

	th.join();

	DisconnectNamedPipe(m_pipe);
	EXPECT_EQ(client.Disconnect(), true);

	m_pipe = INVALID_HANDLE_VALUE;
	th = std::thread(TestToolbox::TestConnect, std::ref(m_pipe));
	std::this_thread::sleep_for(std::chrono::seconds(1));

	EXPECT_EQ(client.Connect(), true);

	th.join();

	DisconnectNamedPipe(m_pipe);
	EXPECT_EQ(client.Disconnect(), true);
	CloseHandle(m_pipe);
}

TEST(NetAgentTestCase, RecieveTest)
{
	const std::wstring message = L"TEST STRING";
	CustomClient client(DEFAULT_PIPE_NAME);

	HANDLE m_pipe = INVALID_HANDLE_VALUE;

	std::thread th(TestToolbox::TestConnect, std::ref(m_pipe));
	std::this_thread::sleep_for(std::chrono::seconds(1));

	client.Connect();

	th.join();

	th = std::thread(TestToolbox::TestWrite, std::ref(m_pipe), std::ref(message));

	std::wstring result;
	EXPECT_EQ(client.Recieve(result), true);

	th.join();

	client.Disconnect();
	CloseHandle(m_pipe);

	EXPECT_EQ(result, message);
}

TEST(NetAgentTestCase, SendTest)
{
	const std::wstring message = L"TEST STRING";
	CustomClient client(DEFAULT_PIPE_NAME);

	HANDLE m_pipe = INVALID_HANDLE_VALUE;

	std::thread th(TestToolbox::TestConnect, std::ref(m_pipe));
	std::this_thread::sleep_for(std::chrono::seconds(1));

	client.Connect();

	th.join();

	std::wstring result;
	th = std::thread(TestToolbox::TestRead, std::ref(m_pipe), std::ref(result));

	EXPECT_EQ(client.Send(message), true);

	th.join();
	client.Disconnect();
	CloseHandle(m_pipe);


	EXPECT_EQ(result, message);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}