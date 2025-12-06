// Copyright F Rank Hunter. All Rights Reserved.


#include "GameAnalyzeData.h"

GameAnalyzeData::GameAnalyzeData()
	: FBitWriter(1024, false),
	AnalyzeType(GameAnalyzeType::NoData),
	LeftGateTime(-1),
	GateGrade(-1),
	GateRound(-1),
	bIsPlayerLevelData(false),
	bIsPlayerDied(false),
	bIsCreatureDied(false),
	bIsPlayerStatUp(false),
	ItemID(-1),
	TrapID(-1),
	CreatureID(-1),
	StatType(-1),
	bIsGateEnd(false),
	bIsBoughtItem(false),
	bIsSoldItem(false),
	bPending(false),
	CollectedMagicStone(-1),
	BoughtItemID(-1),
	SoldItemID(-1),
	bIsCoreDestroyed(false),
	bIsCoreFound(false),
	bIsGateFailed(false),
	bIsGateCleared(false),
	TotalRoomCount(-1),
	EnteredRoomCount(-1)
{
}

GameAnalyzeData::~GameAnalyzeData()
{
}

const int64 GameAnalyzeData::Serialize()
{
	Reset();

	FBitWriter::Serialize(&AnalyzeType, sizeof(GameAnalyzeType));
	FBitWriter::Serialize(&LeftGateTime, sizeof(int16));
	FBitWriter::Serialize(&GateGrade, sizeof(int16));
	FBitWriter::Serialize(&GateRound, sizeof(int16));

	switch (AnalyzeType)
	{
	case GameAnalyzeType::Battle:
	{
		uint8 BitTemp = 0;
		BitTemp |= (bIsPlayerLevelData << 3);
		BitTemp |= (bIsPlayerDied << 2);
		BitTemp |= (bIsCreatureDied << 1);
		BitTemp |= (bIsPlayerStatUp << 0);

		FBitWriter::SerializeBits(&BitTemp, 4);

		switch (BitTemp)
		{
		case 0x08:
		{
			int8 PlayerCount = PlayerLevel.Num();
			FBitWriter::SerializeBits(&PlayerCount, 4);
			for (int16 Level : PlayerLevel)
			{
				FBitWriter::Serialize(&Level, sizeof(int16));
			}
		}
		break;
		case 0x04:
		{
			FBitWriter::Serialize(&ItemID, sizeof(int16));
			FBitWriter::Serialize(&TrapID, sizeof(int16));
			FBitWriter::Serialize(&CreatureID, sizeof(int16));
		}
		break;
		case 0x02:
		{
			FBitWriter::Serialize(&ItemID, sizeof(int16));
			FBitWriter::Serialize(&TrapID, sizeof(int16));
		}
		break;
		case 0x01:
		{
			FBitWriter::Serialize(&StatType, sizeof(int16));
		}
		break;
		default:
			break;
		}
	}
	break;

	case GameAnalyzeType::Economy:
	{
		uint8 BitTemp = 0;
		BitTemp |= (bIsGateEnd << 3);
		BitTemp |= (bIsBoughtItem << 2);
		BitTemp |= (bIsSoldItem << 1);
		BitTemp |= (bPending << 0);

		FBitWriter::SerializeBits(&BitTemp, 4);

		switch (BitTemp)
		{
		case 0x08:
		{
			FBitWriter::Serialize(&CollectedMagicStone, sizeof(int16));
		}
		break;
		case 0x04:
		{
			FBitWriter::Serialize(&BoughtItemID, sizeof(int16));
		}
		break;
		case 0x02:
		{
			FBitWriter::Serialize(&SoldItemID, sizeof(int16));
		}
		break;
		default:
			break;
		}
	}
	break;
	case GameAnalyzeType::MapAndGate:
	{
		uint8 BitTemp = 0;
		BitTemp |= (bIsCoreDestroyed << 3);
		BitTemp |= (bIsCoreFound << 2);
		BitTemp |= (bIsGateFailed << 1);
		BitTemp |= (bIsGateCleared << 0);

		FBitWriter::SerializeBits(&BitTemp, 4);

		switch (BitTemp)
		{
		case 0x08:
		{

		}
		break;
		case 0x04:
		{

		}
		break;
		case 0x02:
		{
			FBitWriter::Serialize(&TotalRoomCount, sizeof(int16));
			FBitWriter::Serialize(&EnteredRoomCount, sizeof(int16));
		}
		break;
		case 0x01:
		{
			FBitWriter::Serialize(&TotalRoomCount, sizeof(int16));
			FBitWriter::Serialize(&EnteredRoomCount, sizeof(int16));
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	return FBitWriter::GetNumBytes();
}
