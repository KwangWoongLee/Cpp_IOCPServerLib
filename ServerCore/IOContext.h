#pragma once
#include "stdafx.h"

enum class eIOType 
{
	SEND,
	RECV,
	ACCEPT,
	CONNECT,
	DISCONNECT
};

using SessionPtr = Session*;

enum class eDisConnectReason
{
	UNKNOWN,
	TRANS_ZERO,
	CLOSED_BY_CLIENT,
	RECV_SOCKET_DEALLOC,
	RECV_COMPLETE_ERROR,
	SEND_SOCKET_DEALLOC,
	SEND_COMPLETE_ERROR,
	IO_COMPLETE_ERROR,
};

class Overlapped : public OVERLAPPED
{
public:
	Overlapped() { Init(); };
	virtual ~Overlapped() = default;
	inline void Init() {
		OVERLAPPED::hEvent = 0;
		OVERLAPPED::Internal = 0;
		OVERLAPPED::InternalHigh = 0;
		OVERLAPPED::Offset = 0;
		OVERLAPPED::OffsetHigh = 0;
	};

	IOCPObjectRef GetOwner() const { return mOwner; };
	void	SetOwner(IOCPObjectRef iocpObject) { mOwner = iocpObject; };

	eIOType GetType() const { return mType; };

protected:
	eIOType mType;
	
private:
	IOCPObjectRef mOwner = nullptr;
}; 

class AcceptEvent : public Overlapped
{
public:
	AcceptEvent(): Overlapped() {
		mType = eIOType::ACCEPT;
	};

	SessionRef GetSession() { return mSession; };
	void SetSession(SessionRef session) { mSession = session; }

private:
	SessionRef mSession = nullptr;
};

class ConnectEvent : public Overlapped
{
public:
	ConnectEvent()
	{
		mType = eIOType::DISCONNECT;
	};
};


class DisConnectEvent : public Overlapped
{
public:
	DisConnectEvent():mDisConnectReason(eDisConnectReason::UNKNOWN)
	{
		mType = eIOType::DISCONNECT;
	};

	eDisConnectReason mDisConnectReason;
};

class RecvEvent : public Overlapped
{
public:
	RecvEvent() {
		mType = eIOType::RECV;
	};
};

class SendEvent : public Overlapped
{
public:
	SendEvent() {
		mType = eIOType::SEND;
	};

	Vector<SendBufferRef>& GetSendBuffers() { return sendBuffers; };

private:
	Vector<SendBufferRef> sendBuffers;
};

