#include "stdafx.h"

Service::Service(uint16 port, SessionFactory sessionFactory, uint8 threadCount, uint32 maxSessionCount, eServiceType serviceType)
{
	mIOCP = MakeShared<IOCP>();
	mNetAddress = NetAddress(L"127.0.0.1", port);
	mServiceType = serviceType;
	mSessionFactory = sessionFactory;
	mMaxSessionCount = maxSessionCount;
	mThreadCount = threadCount;
}

Service::~Service() {
}

void Service::Run()
{
	for (int32 i = 0; i < mThreadCount; i++)
	{
		mThreadManager.Launch([&]()
			{
				while (true)
				{
					mIOCP->IOWorkerFunc();
				}
			});
	}


	mThreadManager.Join();

}

void Service::Stop()
{
	mThreadManager.Join();
}

SessionRef Service::CreateSession()
{
	auto session = mSessionFactory();
	session->SetService(shared_from_this());

	if (mIOCP->RegistCompletionPort(session) == false)
		return nullptr;

	return session;
}

bool Service::RegistCompletionPort(IOCPObjectRef iocpObject)
{
	return mIOCP->RegistCompletionPort(iocpObject);
}

ServerService::ServerService(uint16 port, SessionFactory sessionFactory, uint8 threadCount, uint32 maxSessionCount, eServiceType serviceType)
	: Service(port, sessionFactory, threadCount, maxSessionCount, serviceType)
{
}

void ServerService::Run()
{
	mListener = MakeShared<Listener>();

	if (mListener == nullptr)
		return;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	//ASSERT_CRASH(service != nullptr);

	mListener->Init(service, mNetAddress);

	Service::Run();
}

ClientService::ClientService(uint16 port, SessionFactory sessionFactory, uint8 threadCount, uint32 maxSessionCount, eServiceType serviceType)
	: Service(port, sessionFactory, threadCount, maxSessionCount, serviceType)
{
}

void ClientService::Run()
{
}
