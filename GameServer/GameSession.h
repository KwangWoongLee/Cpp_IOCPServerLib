#pragma once
class GameSession : public PacketSession
{
public:
	GameSession() {};
	virtual ~GameSession() {};


	virtual void OnRecvPacket(BYTE* buffer) override;
};

