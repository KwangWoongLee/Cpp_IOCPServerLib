#pragma once

class IOCPObject : public enable_shared_from_this<IOCPObject>
{
public:
	virtual SOCKET GetHandle() = 0;
	virtual void Dispatch(class Overlapped* iocpEvent, int32 numOfBytes = 0) = 0;
};


class IOCP
{
public:
	IOCP();
	virtual ~IOCP();

	bool RegistCompletionPort(IOCPObjectRef iocpObject);


	// IOWorker 쓰레드 함수
	void IOWorkerFunc();

private:
	HANDLE mCompletionPort = nullptr;

	void createCompletionPort();
};