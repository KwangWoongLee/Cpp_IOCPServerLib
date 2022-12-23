#pragma once
#include "stdafx.h"

class SessionManager
{
public:
	SessionManager() {};
	virtual ~SessionManager() {};

	void Add(const std::shared_ptr<Session>&& session);


private:
	std::unordered_map<int32, std::shared_ptr<Session>> mSessionMap;
	int32 mSessionCount = 0;
};

