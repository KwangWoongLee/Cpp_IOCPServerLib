#include "stdafx.h"

IOCP::IOCP()
	:mCompletionPort(nullptr)
{
	createCompletionPort();
}

IOCP::~IOCP()
{
	::CloseHandle(mCompletionPort);
}

void IOCP::createCompletionPort()
{
	// ���ʿ� �ѹ��� CompletionPort �ʱ�ȭ
	mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (mCompletionPort == NULL)
		throw std::format("IOCP Create Failed {}", GetLastError());
}

bool IOCP::RegistCompletionPort(IOCPObjectRef iocpObject)
{
	ASSERT_CRASH(iocpObject->GetHandle() != INVALID_SOCKET);

	// Accept�� ���� ��, CompletionPort�� ���
	HANDLE handle = CreateIoCompletionPort((HANDLE)iocpObject->GetHandle(), mCompletionPort, 0, 0);

	if (handle != mCompletionPort)
		return false;

	return true;
}
//
//void IOCP::createThreadPool(short threadCount)
//{
//	assert(threadCount >= 0);
//
//	for (int i = 0; i < threadCount; ++i)
//		mIOWorkerThreadPool.push_back(std::move(std::thread(&IOCP::IOWorkerFunc, this)));
//}
//
//
//void IOCP::stopThreads()
//{
//	// PostQueuedCompletionStatus �� IOCP�� ���� ��ȣ�� ��
//	BOOL ret = PostQueuedCompletionStatus(mCompletionPort, 0, THREAD_DIE, NULL);
//
//	if (!ret)
//		throw std::format("Stop IOCP Failed {}", GetLastError());
//
//	//for (auto& thread : mIOWorkerThreadPool)
//	//{
//	//	if(thread.joinable())
//	//		thread.join();
//	//}
//}


void IOCP::IOWorkerFunc()
{
	//threadType = eThreadType::IO_WORKER;

	while (true)
	{
		Overlapped* event = nullptr;
		ULONG_PTR key = 0;
		DWORD dwTransferred = 0; 

		bool ret = GetQueuedCompletionStatus(mCompletionPort, &dwTransferred, &key, reinterpret_cast<LPOVERLAPPED*>(&event), INFINITE);
		// �����ϸ�  TRUE�� ��ȯ�ϰ� �׷��� ������ FALSE�� ��ȯ�մϴ�.
		
		if (ret == true) // ����
		{
			auto iocpObject = event->GetOwner();
			iocpObject->Dispatch(event, dwTransferred);

		}
		else { // ����
			auto err = WSAGetLastError();
			if (err == WAIT_TIMEOUT)
				return;

			else 
			{
				eIOType type = event->GetType();
				auto iocpObject = event->GetOwner();
				iocpObject->Dispatch(event, dwTransferred);
			
				break;
			}
		}
	}
}
