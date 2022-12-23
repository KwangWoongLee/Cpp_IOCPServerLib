#include "stdafx.h"
#include "Service.h"


int main()
{
	//threadType = eThreadType::MAIN;
	try {
		ServerServiceRef server = nullptr;
		//server.Init(8080, 3);
		
		server->Run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1; // 비정상 종료
	}	

	return 0;
}
