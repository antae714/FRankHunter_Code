// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FHAbilityTypes.generated.h"

UCLASS()
class FRANKHUNTER_API UCollisionCheckResult : public UObject
{
	GENERATED_BODY()

public:

	
};

UENUM()
enum class EAbilityActive
{
	Tag,
	Class
};

UENUM()
enum class EAbilityActiveType
{
	Tag,
	Class
};
