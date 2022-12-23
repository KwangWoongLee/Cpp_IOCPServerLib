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
			// Pending �ܿ� �ٸ� ������� �߸��Ȱ�

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
			// TODO : ���� üũ

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

	// ���� ���
	//GetService()->AddSession(GetSessionRef());

	// ������ �ڵ忡�� ������
	OnConnected();

	// ���� ���
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

	// ���� RegisterSend�� �ɸ��� ���� ���¶��, �ɾ��ش�
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
	//��Ŷ�м�
	//�Ϸ� �ȵǸ� �ٽ�

	int processLen = 0;

	auto recvBuff = GetRecvBuffer();
	//��Ŷ �м��� ���� ���ũ�� �̻��� �ִ��� Ȯ��
	if (recvBuff.GetDataSize() < sizeof(PACKET_SIZE))
		return processLen;

	//��Ŷ ����
	int32 dataSize = 0;
	int offset = 0;
	memcpy(&dataSize, recvBuff.GetReadPos(), sizeof(PACKET_SIZE));

	if (recvBuff.GetDataSize() - sizeof(PACKET_SIZE) < dataSize)
		return processLen;

	// ��Ŷ �ϳ� �ϼ� ���� ����
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
