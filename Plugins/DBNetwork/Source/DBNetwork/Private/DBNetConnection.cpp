// Copyright F Rank Hunter. All Rights Reserved.


#include "DBNetConnection.h"
#include "SocketSubsystemModule.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

DEFINE_LOG_CATEGORY(LogDBConnection);


Packet::Packet(const TArray<uint8>* InData, const uint8 NumBytes, const uint16 NumBits)
	:
	NumBytes(NumBytes),
	NumBits(NumBits)
{
	PacketSequence = ++StaticPacketSequence;
	Data = *InData;
}

DBNetConnection::DBNetConnection()
{

}

DBNetConnection::~DBNetConnection()
{
	if (Socket)
	{
		Socket->Shutdown(ESocketShutdownMode::ReadWrite);
		Socket->Close();

		FSocketSubsystemModule* SocketModule = FModuleManager::GetModulePtr<FSocketSubsystemModule>(TEXT("Sockets"));
		if (SocketModule)
		{
			ISocketSubsystem* SocketSubsystem = SocketModule->GetSocketSubsystem(TEXT("WINDOWS"));
			if (SocketSubsystem)
			{
				SocketSubsystem->DestroySocket(Socket);
			}
		}
	}

}

bool DBNetConnection::Initialize()
{
	FSocketSubsystemModule* SocketModule = FModuleManager::GetModulePtr<FSocketSubsystemModule>(TEXT("Sockets"));
	if (SocketModule)
	{
		ISocketSubsystem* SocketSubsystem = SocketModule->GetSocketSubsystem(TEXT("WINDOWS"));
		if (SocketSubsystem)
		{
			Socket = SocketSubsystem->CreateSocket(TEXT("Stream"), TEXT("DBSocket"));
			if (Socket)
			{
				Socket->SetNoDelay(true);
				UE_LOG(LogDBConnection, Warning, TEXT("Socket Create Success. Socket Type: WINDOWS."));
			}
			else
			{
				UE_LOG(LogDBConnection, Warning, TEXT("Socket Create Error."));
				return false;
			}

			Address = SocketSubsystem->CreateInternetAddr();
			bool bIsValid;
			FString DBServerIp = TEXT("127.0.0.1:8800");
			Address->SetIp(*DBServerIp, bIsValid);
			if (bIsValid)
			{
				UE_LOG(LogDBConnection, Warning, TEXT("Is Valid Ip Address. %s"), *DBServerIp);
			}
			else
			{
				UE_LOG(LogDBConnection, Warning, TEXT("InValid Ip Address. %s"), *DBServerIp);
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	if (!Socket->Connect(*Address))
	{
		return false;
	}

	return true;
}

int32 DBNetConnection::LogSend()
{
	int32 ByteSend;
	Packet* SendPacket = nullptr;
	if (!PacketQueue.Dequeue(SendPacket))
	{
		UE_LOG(LogDBConnection, Warning, TEXT("NoPackets."));
		return -1;
	}

	if (!SendPacket)
	{
		return -1;
	}
	uint8 TempValue;
	
	TempValue = SendPacket->GetPacketSequence();
	memcpy(&Buffer[BufferSize], &TempValue, sizeof(SendPacket->GetPacketSequence()));
	BufferSize += sizeof(SendPacket->GetPacketSequence());

	TempValue = SendPacket->GetSize();
	memcpy(&Buffer[BufferSize], &TempValue, sizeof(SendPacket->GetSize()));
	BufferSize += sizeof(SendPacket->GetSize());

	uint16 BitValue = SendPacket->GetBitSize();
	memcpy(&Buffer[BufferSize], &BitValue, sizeof(SendPacket->GetBitSize()));
	BufferSize += sizeof(SendPacket->GetBitSize());

	memcpy(&Buffer[BufferSize], SendPacket->GetData(), SendPacket->GetSize());
	BufferSize += SendPacket->GetSize();

	if (Socket->Send(Buffer, BufferSize, ByteSend))
	{
		BufferSize -= ByteSend;
		memcpy(&Buffer[0], &Buffer[ByteSend], BufferSize);
	}

	delete SendPacket;

	return ByteSend;
}

void DBNetConnection::SaveSendLogPacket(const TArray<uint8>* Data, const uint8 NumBytes, const uint16 NumBits)
{
	PacketQueue.Enqueue(new Packet(Data, NumBytes, NumBits));
}
