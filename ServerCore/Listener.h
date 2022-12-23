#pragma once

class Listener : public IOCPObject
{
public:
	Listener() = default;
	virtual ~Listener();

	virtual SOCKET GetHandle() override;
	virtual void Dispatch(class Overlapped* iocpEvent, int32 numOfBytes = 0) override;

	void Init(ServerServiceRef server, NetAddress& netAddress);

private:
	SOCKET mListenSocket = INVALID_SOCKET;
	ServerServiceRef mServer = nullptr;
	Vector<AcceptEvent*> mAcceptEvents;

	void registAccept(AcceptEvent* acceptEvent);
	void asyncAccept(AcceptEvent* acceptEvent);
};
