// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EItemHoldingSocketDirection : uint8
{
	Nowhere			UMETA(DisplayName = "Nowhere"),
	LeftHand		UMETA(DisplayName = "LeftHand"),
	RightHand		UMETA(DisplayName = "RightHand")
}; 

UENUM(BlueprintType)
enum class EItemHoldingType : uint8
{
	NoHanded 			UMETA(DisplayName = "NoHanded"),
	OneHanded_Throw 	UMETA(DisplayName = "OneHanded_Throw"),
	OneHanded_Gun		UMETA(DisplayName = "OneHanded_Gun"),
	TwoHanded 			UMETA(DisplayName = "TwoHanded"),
	Terminal			UMETA(DisplayName = "Terminal")
};

UENUM(BlueprintType)
enum class EPlayerEmoteType : uint8
{
	Clapping				UMETA(DisplayName = "Clapping"),
	Dance_GangnamStyle		UMETA(DisplayName = "Dance_GangnamStyle"),
	Pointing				UMETA(DisplayName = "Pointing"),
	Salute					UMETA(DisplayName = "Salute"),
	Dance_SodaPop			UMETA(DisplayName = "Dance_SodaPop")
};