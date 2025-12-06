// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class GameAnalyzeType : uint8
{
	NoData = 0,
	Battle,
	Economy,
	MapAndGate,

};


/**
 * 
 */
class DBNETWORK_API GameAnalyzeData : public FBitWriter
{
public:
	GameAnalyzeData();
	~GameAnalyzeData();

	const int64 Serialize();

	GameAnalyzeType AnalyzeType;
	int16 LeftGateTime;
	int16 GateGrade;
	int16 GateRound;
	TArray<int16> PlayerLevel;

	// ===== Battle Data =====

	// 4 - bits 
	bool bIsPlayerLevelData;
	bool bIsPlayerDied;
	bool bIsCreatureDied;
	bool bIsPlayerStatUp;


	int16 ItemID;
	int16 TrapID;
	int16 CreatureID;
	int16 StatType;

	// =======================

	// ==== Economy Data =====

	bool bIsGateEnd;
	bool bIsBoughtItem;
	bool bIsSoldItem;
	bool bPending;

	int16 CollectedMagicStone;
	int16 BoughtItemID;
	int16 SoldItemID;

	// =======================

	// == MapAndGate Data ====

	bool bIsCoreDestroyed;
	bool bIsCoreFound;
	bool bIsGateFailed;
	bool bIsGateCleared;

	int16 TotalRoomCount;
	int16 EnteredRoomCount;

	// =======================

private:

};
