#include "stdafx.h"


SOCKET Session::GetHandle()
{
	return mSocket;
}

void Session::Dispatch(Overlapped* iocpEvent, int32 numOfBytes)
{

	switch (eIOType type; type = iocpEvent->GetType())
	{
	case eIOType::CONNECT:
		OnConnected();
		break;
	case eIOType::RECV:
		OnRecvCompleted(numOfBytes);
		break;
	case eIOType::SEND:
		OnSendCompleted(numOfBytes);
		break;
	case eIOType::DISCONNECT:
		OnDisconnected(eDisConnectReason::UNKNOWN);
		break;


	default:
		break;
	}
}

void Session::Init(const SOCKET& socket)
{
	mSocket = socket;
}

void Session::AsyncConnect()
{
	if (IsConnected())
		return;


}

void Session::AsyncDisconnect(eDisConnectReason reason)
{
	//DisConnectOverlapped* disConnectOverlapped = new DisConnectOverlapped;
	//disConnectOverlapped->mDisConnectReason = reason;

	//if (FALSE == FnDisConnectEx(mSocket, (LPWSAOVERLAPPED)disConnectOverlapped, TF_REUSE_SOCKET, 0))
	//{
	//	if (WSAGetLastError() != WSA_IO_PENDING)
	//	{
	//		delete disConnectOverlapped;

	//		auto err = std::format("DisConnect Error {}", WSAGetLastError());
	//		std::cout << err << std::endl;
	//	}
	//}
}

void Session::AsyncRecv()
{
	if (IsConnected() == false)
		return;

	mRecvEvent.Init();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(mRecvBuff.GetWritePos());
	wsaBuf.len = mRecvBuff.GetFreeSize();

	DWORD flags = 0;
	DWORD recvBytes = 0;

	auto error = WSARecv(mSocket, &wsaBuf, 1, &recvBytes, &flags, &mRecvEvent, NULL);
	if (error == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// Pending 외에 다른 오류라면 잘못된것

			mRecvEvent.SetOwner(nullptr);

			auto err = std::format("Recv Error {}", WSAGetLastError());
			
		}
	}
}

void Session::AsyncSend()
{
	if (IsConnected() == false)
		return;

	mSendEvent.Init();
	mSendEvent.SetOwner(shared_from_this()); // ADD_REF

	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (mSendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = mSendQueue.front();

			writeSize += sendBuffer->GetWriteSize();
			// TODO : 예외 체크

			mSendQueue.pop();
			mSendEvent.GetSendBuffers().push_back(sendBuffer);
		}
	}

	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(mSendEvent.GetSendBuffers().size());
	for (SendBufferRef sendBuffer : mSendEvent.GetSendBuffers())
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->GetWriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD flags = 0;
	DWORD sendBytes;
	DWORD error = WSASend(mSocket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), &sendBytes, flags, &mSendEvent, NULL);

	if (error == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			mSendEvent.SetOwner(nullptr);
			mSendEvent.GetSendBuffers().clear();
			mSendFlag.store(false);

			auto err = std::format("Send Error {}", WSAGetLastError());
			std::cout << err << std::endl;
		}
	}
}

void Session::OnAcceptCompleted()
{
	mConnectEvent.SetOwner(nullptr);
	
	mConnected.store(true);

	// 세션 등록
	//GetService()->AddSession(GetSessionRef());

	// 컨텐츠 코드에서 재정의
	OnConnected();

	// 수신 등록
	this->AsyncRecv();
}

void Session::OnRecvCompleted(int32 transferred)
{
	mRecvEvent.SetOwner(nullptr);

	if (mRecvBuff.OnWrite(transferred) == false)
	{
		this->AsyncDisconnect(eDisConnectReason::IO_COMPLETE_ERROR);
		return;
	}

	int processLen = OnRecv();
	if (processLen < 0 || mRecvBuff.GetDataSize() < processLen)
	{
		this->AsyncDisconnect(eDisConnectReason::IO_COMPLETE_ERROR);
		return;
	}

	if (mRecvBuff.OnRead(processLen) == false)
	{
		this->AsyncDisconnect(eDisConnectReason::IO_COMPLETE_ERROR);
		return;
	}

	this->AsyncRecv();
}

void Session::OnSendCompleted(int32 transferred)
{
	{
		std::unique_lock<std::mutex>(mSendQueueLock);

		OnSend(transferred);

		if (!mSendQueue.empty())
			this->AsyncSend();
	}
}

void Session::Send(SendBufferRef sendBuffer)
{
	{
		std::unique_lock<std::mutex>(mSendQueueLock);

		//mSendQueue.push(packet);

		if (!mSendQueue.empty())
			this->AsyncSend();
	}

	if (IsConnected() == false)
		return;

	bool registerSend = false;

	// 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다
	{
		WRITE_LOCK;

		//mSendQueue.push(mSendBuffer);

		if (mSendFlag.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		AsyncSend();
}

int PacketSession::OnRecv()
{
	//패킷분석
	//완료 안되면 다시

	int processLen = 0;

	auto recvBuff = GetRecvBuffer();
	//패킷 분석을 위해 헤더크기 이상이 있는지 확인
	if (recvBuff.GetDataSize() < sizeof(PACKET_SIZE))
		return processLen;

	//패킷 조립
	int32 dataSize = 0;
	int offset = 0;
	memcpy(&dataSize, recvBuff.GetReadPos(), sizeof(PACKET_SIZE));

	if (recvBuff.GetDataSize() - sizeof(PACKET_SIZE) < dataSize)
		return processLen;

	// 패킷 하나 완성 가능 상태
	OnRecvPacket(recvBuff.GetReadPos());

	processLen += sizeof(PACKET_SIZE) + dataSize;

	return processLen;
}

void PacketSession::OnSend(int32 numOfBytes)
{
}

void PacketSession::OnConnected()
{
	
}

void PacketSession::OnDisconnected(eDisConnectReason reason)
{
}
