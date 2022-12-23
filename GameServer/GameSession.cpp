#include "stdafx.h"
#include "GameSession.h"


void GameSession::OnRecvPacket(BYTE* buffer)
{
	int32 offset = 0;
	PACKET_SIZE packetSize[1]{ 0 };

	memcpy((void*)packetSize, (void*)buffer, sizeof(PACKET_SIZE));

	offset += sizeof(PACKET_SIZE);

	//std::shared_ptr<Packet> packet = GPacketAnalyzer->Analyze(buffer + offset, packetSize[0] - offset);

}
