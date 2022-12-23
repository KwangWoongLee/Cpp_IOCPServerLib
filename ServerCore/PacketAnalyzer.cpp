#include "stdafx.h"

std::shared_ptr<Packet> PacketAnalyzer::Analyze(const char* buffer, size_t size)
{
	//패킷 타입을 가져오고 그만큼 offset 증가.
	size_t offset = 0;

	ePacketType type[1] = { ePacketType::NONE };
	memcpy((void*)type, buffer, sizeof(PACKET_TYPE_SIZE));
	offset += sizeof(PACKET_TYPE_SIZE);

	if (type[0] == ePacketType::NONE)
		return nullptr;

	//std::shared_ptr<Packet> packet = PacketFactory::GetInstance().GetPacket(type[0]);


	//if (packet)
	//{
	//	if (offset < size)
	//	{
	//		InputStream stream((char*)(buffer + offset), size - offset);
	//		packet->Decode(stream);
	//	}
	//}

	//return packet;
	return nullptr;
}