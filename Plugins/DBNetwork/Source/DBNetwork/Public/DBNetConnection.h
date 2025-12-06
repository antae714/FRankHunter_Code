// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSocket;
class FInternetAddr;
class Packet;

/**
 * 
 */
class DBNETWORK_API DBNetConnection
{
public:
	DBNetConnection();
	~DBNetConnection();

	bool Initialize();
	int32 LogSend();

	void SaveSendLogPacket(const TArray<uint8>* Data, const uint8 NumBytes, const uint16 NumBits);
private:
	FSocket* Socket = nullptr;
	TSharedPtr<FInternetAddr> Address;

	int32 BufferSize = 0;
	uint8 Buffer[1024]{};

	TQueue<Packet*> PacketQueue;
};

static uint8 StaticPacketSequence = 0;

class Packet
{
public:
	Packet(const TArray<uint8>* InData, const uint8 NumBytes, const uint16 NumBits);

	uint8 GetPacketSequence() const { return PacketSequence; }
	const uint8* GetData() const { return Data.GetData(); }
	uint8 GetSize() const { return NumBytes; }
	uint16 GetBitSize() const { return NumBits; }
private:

	uint8 PacketSequence;
	uint8 NumBytes;
	uint16 NumBits;
	TArray<uint8> Data;
};

DECLARE_LOG_CATEGORY_EXTERN(LogDBConnection, Log, All);