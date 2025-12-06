// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FHPlayerHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHPlayerHUDBase : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;


};
