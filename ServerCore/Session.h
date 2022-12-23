#pragma once
#include "stdafx.h"

constexpr uint32 DATA_SIZE = 2;
constexpr uint32 BUFF_SIZE = 1024;
constexpr uint32 SEND_BUFF_SIZE = 4096;

enum class eDisConnectReason;

class Session : public IOCPObject //추상 클래스
{
public:
	Session()
		:mRecvBuff(1024)
	{
		mSocket = SocketUtils::CreateSocket();
	};
	virtual ~Session() = default;


	virtual SOCKET GetHandle() override;
	virtual void Dispatch(class Overlapped* iocpEvent, int32 numOfBytes = 0) override;

	NetAddress& GetNetAddress() { return mNetAddress; };
	void SetNetAddress(NetAddress&& netAddress) { mNetAddress = move(netAddress); };

	void Init(const SOCKET& socket);
	
	int32 GetKey() const { return mSessionKey; };
	void SetKey(int32 key) { mSessionKey = key; };

	bool IsConnected() const { return mConnected.load(); };
	void SetService(ServiceRef service) { mService = service; };
	ServiceRef GetService() { return mService.lock(); }

	void AsyncConnect();
	void AsyncDisconnect(eDisConnectReason reason);
	void AsyncRecv();
	void AsyncSend();
	
	void OnAcceptCompleted();
	void OnRecvCompleted(int32 transferred);
	void OnSendCompleted(int32 transferred);

	void Send(SendBufferRef sendBuffer);

	RecvBuffer& GetRecvBuffer() { return mRecvBuff; };

protected:
	virtual void OnConnected() = 0;
	virtual int OnRecv() = 0;
	virtual void OnSend(int32 numOfBytes) = 0;
	virtual void OnDisconnected(eDisConnectReason reason) = 0;


private:
	USE_LOCK;

	Atomic<bool> mConnected = false;
	SOCKET mSocket = INVALID_SOCKET;
	int32 mSessionKey{0};
	weak_ptr<Service> mService;
	NetAddress mNetAddress{};

	ConnectEvent mConnectEvent;

	RecvEvent mRecvEvent;
	RecvBuffer mRecvBuff;

	SendEvent	mSendEvent;
	Atomic<bool> mSendFlag = false;
	Queue<SendBufferRef> mSendQueue;
};

class PacketSession : public Session
{

public:
	PacketSession() {
	};
	virtual ~PacketSession() {};


protected:
	virtual void OnRecvPacket(BYTE* buffer) = 0;

private:

	virtual int OnRecv() override;
	virtual void OnSend(int32 numOfBytes) override;
	virtual void OnConnected() override;
	virtual void OnDisconnected(eDisConnectReason reason) override;
};
