#include "stdafx.h"
#include "SessionManager.h"

void SessionManager::Add(const std::shared_ptr<Session>&& session)
{
	//TODO : Lock 추가
	session->SetKey(++mSessionCount);

	//TODO : insert 동작원리 확인하고 insert로 할 지, []로 할 지 결정
	mSessionMap.insert({ mSessionCount, session });
}
