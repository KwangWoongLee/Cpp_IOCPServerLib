#include "stdafx.h"

void Listener::Init(ServerServiceRef server, NetAddress& netAddress)
{
	try {
		mServer = server;
		
		mListenSocket = SocketUtils::CreateSocket();
		if (mListenSocket == INVALID_SOCKET)
			throw std::format("Listen Socket Create Failed {}", WSAGetLastError());

		// Set Socket Option
		if (SocketUtils::SetReuseAddress(mListenSocket, true) == false)
			throw std::format("Listen Socket Setting Reuse Address Failed {}", WSAGetLastError());

		if (SocketUtils::SetLinger(mListenSocket, 0, 0) == false)
			throw std::format("Listen Socket Setting Linger Option Failed {}", WSAGetLastError());

		if (SocketUtils::Bind(mListenSocket, netAddress) == false)
			throw std::format("Listen Socket Bind Failed {}", WSAGetLastError());

		if (SocketUtils::Listen(mListenSocket) == false)
			throw std::format("Listen Socket Listen Failed {}", WSAGetLastError());

		if (server->RegistCompletionPort(shared_from_this()) == false)
			throw std::format("Listen Socket IOCP Regist Failed");


		const int32 acceptCount = mServer->GetMaxSessionCount();
		for (int32 i = 0; i < acceptCount; i++)
		{
			AcceptEvent* acceptEvent = xnew<AcceptEvent>();
			acceptEvent->SetOwner(shared_from_this());
			mAcceptEvents.push_back(acceptEvent);
			asyncAccept(acceptEvent);
		}
	}
	catch (std::exception& e)
	{
		throw e;
	}
}


Listener::~Listener()
{
	SocketUtils::Close(mListenSocket);
}

SOCKET Listener::GetHandle()
{
	return mListenSocket;
}

void Listener::Dispatch(Overlapped* iocpEvent, int32 numOfBytes)
{
	//Accept 이벤트만 들어옴
	ASSERT_CRASH(iocpEvent->GetType() == eIOType::ACCEPT);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	registAccept(acceptEvent);
}


void Listener::registAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->GetSession();

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetHandle(), mListenSocket))
	{
		registAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetHandle(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		registAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));
	//session->ProcessConnect();
	registAccept(acceptEvent);
}

void Listener::asyncAccept(AcceptEvent* acceptEvent)
{
	auto session = mServer->CreateSession();

	acceptEvent->Init();
	acceptEvent->SetSession(session);

	DWORD bytes{ 0 };

	if (FALSE == AcceptEx(mListenSocket, session->GetHandle(), session->GetRecvBuffer().GetWritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// Pending 외에 다른 오류라면 잘못된것
			asyncAccept(acceptEvent);
		}
	}

}

