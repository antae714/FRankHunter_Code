// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FHGateThemaAsset.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FFHGateThemaIcons
{
	GENERATED_BODY()

public:
	TSoftObjectPtr<UTexture2D> GetRandomThemaPreviewIcon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ThemaPreview")
	TArray<TSoftObjectPtr<UObject>> ThemaPreviewIcons;

};

UCLASS()
class FRANKHUNTER_API UFHGateThemaAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UTexture2D> GetThemaPreviewIcon(FName Key);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ThemaPreview")
	TMap<FName, FFHGateThemaIcons> ThemaPreviewMap;
};
