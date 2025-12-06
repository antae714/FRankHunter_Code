// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FHSelectInfoIconDataAsset.generated.h"

/**
 * 
 */
class UObject;
class UTexture2D;


UCLASS()
class FRANKHUNTER_API UFHSelectInfoIconDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UTexture2D> GetIconTexture(FName Key);
	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UTexture2D> GetSkillIconTexture(FName Key);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<AActor> GetCharacterBP(FName Key);

	TSoftObjectPtr<UPhysicsAsset> GetNormalCharacterPhysicsAsset(FName Key);
	TSoftObjectPtr<UPhysicsAsset> GetRagdollCharacterPhysicsAsset(FName Key);


	UFUNCTION(BlueprintCallable)
	const TMap<FName, TSoftObjectPtr<UObject>>& GetIconMap() const { return IconMap; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
	TMap<FName, TSoftObjectPtr<UObject>> IconMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
	TMap<FName, TSoftObjectPtr<UObject>> SkillIconMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blueprints")
	TMap<FName, TSubclassOf<AActor>> CharacterBPMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blueprints")
	TSoftObjectPtr<UPhysicsAsset> NormalCharacterPhysicsAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blueprints")
	TSoftObjectPtr<UPhysicsAsset> RagdollCharacterPhysicsAsset;
};
