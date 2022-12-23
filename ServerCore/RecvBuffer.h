#pragma once

/*--------------
	RecvBuffer
----------------*/

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(int32 numOfBytes);
	bool			OnWrite(int32 numOfBytes);

	BYTE* GetReadPos() { return &_buffer[_readPos]; }
	BYTE* GetWritePos() { return &_buffer[_writePos]; }
	int32			GetDataSize() { return _writePos - _readPos; }
	int32			GetFreeSize() { return _capacity - _writePos; }

private:
	int32			_capacity = 0;
	int32			_bufferSize = 0;
	int32			_readPos = 0;
	int32			_writePos = 0;
	Vector<BYTE>	_buffer;
};
