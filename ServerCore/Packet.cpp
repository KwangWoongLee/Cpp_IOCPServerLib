#include "stdafx.h"

void PK_CS_NOTIFY_HEARTBEAT::Encode(OutputStream& stream)
{
	stream << GetTypeToInt();
}

void PK_CS_REQ_EXIT::Encode(OutputStream& stream)
{
	stream << GetTypeToInt();
}

void PK_SC_RES_EXIT::Encode(OutputStream& stream)
{
	stream << GetTypeToInt();
}
