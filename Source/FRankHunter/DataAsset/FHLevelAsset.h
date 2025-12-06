// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FHLevelAsset.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHLevelAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	TSoftObjectPtr<UWorld> GetTitleMap();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	TSoftObjectPtr<UWorld> GetStartLobbyMap();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	TSoftObjectPtr<UWorld> GetEndLobbyMap();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	TSoftObjectPtr<UWorld> GetGateMap();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	TSoftObjectPtr<UWorld> GetCreditMap();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	const FString& GetTitle()			{ return Title; }
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	const FString& GetStartLobby()		{ return MorningLobby; }
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	const FString& GetEndLobby()		{ return NightLobby; }
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	const FString& GetGate()			{ return Gate; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MorningLobby;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString NightLobby;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Gate;

private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> TitleMap;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MorningLobbyMap;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> NightLobbyMap;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> GateMap;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> CreditMap;
};
