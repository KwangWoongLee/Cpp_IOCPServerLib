#include "stdafx.h"


int main()
{
	//threadType = eThreadType::MAIN;
	try {
		ServerServiceRef server = MakeShared<ServerService>(8080,MakeShared<GameSession>, 3, 1000);

		server->Run();

		server->Stop();

	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1; // 비정상 종료
	}

	return 0;
}
