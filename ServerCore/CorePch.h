#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dbghelp.lib")

#define NOMINMAX



// 윈도우 관련
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <mswsock.h>
#include <Mmsystem.h>
#include <Ws2spi.h>
#include <Mstcpip.h>
#include <windows.h>
#include <cassert>

// std
#include <iostream>
#include <stdexcept>
#include <format>
#include <memory>
#include <thread>
#include <mutex>

#include <functional>

//자료구조
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <atomic>
#include <queue>
#include <stack>


using namespace std;


#include "Types.h"
#include "Macro.h"
#include "Allocator.h"
#include "Container.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

#include "NetAddress.h"
#include "SocketUtils.h"

#include "Lock.h"
#include "DeadLockProfiler.h"
#include "ThreadManager.h"


#include "Allocator.h"
#include "Memory.h"
#include "MemoryPool.h"
#include "ObjectPool.h"

#include "RecvBuffer.h"
#include "SendBuffer.h"



#include "IOCP.h"
#include "IOContext.h"

#include "Session.h"
#include "SessionManager.h"



#include "Listener.h"


#include "Service.h"