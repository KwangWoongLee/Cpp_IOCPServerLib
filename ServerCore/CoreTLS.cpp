#include "stdafx.h"

thread_local uint32 LThreadId = 0;
thread_local stack<int32>	LLockStack;
thread_local SendBufferChunkRef	LSendBufferChunk;