#include "stdafx.h"

Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
PacketAnalyzer* GPacketAnalyzer = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GPacketAnalyzer = new PacketAnalyzer();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GMemory;
		delete GSendBufferManager;
		delete GDeadLockProfiler;
		delete GPacketAnalyzer;
		SocketUtils::Clear();
	}
} GCoreGlobal;