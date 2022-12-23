#pragma once

using SessionFactory = function<SessionRef(void)>;



class Service : public enable_shared_from_this<Service>
{
public:

	enum class eServiceType
	{
		CLIENT,
		SERVER,
	};

	Service(uint16 port, SessionFactory sessionFactory, uint8 threadCount, uint32 maxSessionCount, eServiceType serviceType);
	virtual ~Service();


	virtual void Run();
	virtual void Stop();

	void SetServiceType(eServiceType serviceType) { mServiceType = serviceType; };
	eServiceType GetServiceType() const { return mServiceType; }

	SessionRef CreateSession();
	bool RegistCompletionPort(IOCPObjectRef iocpObject);
	
	uint32 GetMaxSessionCount() const { return mMaxSessionCount; };

protected:
	NetAddress mNetAddress = {};

private:
	USE_LOCK;
	eServiceType mServiceType;

	ThreadManager mThreadManager;
	IOCPRef mIOCP = nullptr;
	SessionFactory mSessionFactory = nullptr;

	uint8 mThreadCount {};
	uint32 mMaxSessionCount{};
};

class ServerService : public Service
{
public:
	ServerService(uint16 port, SessionFactory sessionFactory, uint8 threadCount, uint32 maxSessionCount = 1, eServiceType serviceType = eServiceType::SERVER);
	virtual ~ServerService() = default;

	virtual void Run() override;

private:
	ListenerRef mListener = nullptr;
};


class ClientService : public Service
{
public:
	ClientService(uint16 port, SessionFactory sessionFactory, uint8 threadCount, uint32 maxSessionCount = 1, eServiceType serviceType = eServiceType::CLIENT);
	virtual ~ClientService() = default;

	virtual void Run() override;
};