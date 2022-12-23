#include "stdafx.h"

int main()
{
	//threadType = eThreadType::MAIN;
	try {
		ClientServiceRef client = MakeShared<ClientService>(8081, MakeShared<GameSession>, 1);

		client->Run();

		client->Connect();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1; // 비정상 종료
	}

	return 0;
}
