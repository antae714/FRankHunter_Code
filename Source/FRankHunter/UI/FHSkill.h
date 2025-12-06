// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHSkill.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSkill : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	
};
