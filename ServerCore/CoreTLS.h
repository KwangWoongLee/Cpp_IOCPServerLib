#pragma once
#include "stdafx.h"

extern thread_local uint32				LThreadId;
extern thread_local stack<int32>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;