// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FH_ASCDescriptor.generated.h"

UCLASS()
class FRANKHUNTER_API UFH_ASCDescriptor : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS, meta = (DisplayName = "Initial abilities to be granted to ASC"))
	TArray<TSubclassOf<class UGameplayAbility>> InitialAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS, meta = (DisplayName = "Initial effects to be applied to ASC"))
	TArray<TSubclassOf<class UGameplayEffect>> InitialEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAS, meta = (DisplayName = "Initial tags to be attached to ASC"))
	TArray<FGameplayTag> InitialTags;
};
