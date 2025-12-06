// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECustomDepthValueType : uint8
{
	None			UMETA(DisplayName = "None"),
	Outline			UMETA(DisplayName = "Outline")
};

UENUM(BlueprintType)
enum class ECustomPrimitiveDataType : uint8
{
	OutlineOpacity			UMETA(DisplayName = "OutlineOpacity")
}; 