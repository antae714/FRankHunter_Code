// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Lobby/FHAdvancedFriendsGameInstance.h"
#include "DungeonGeneration/FHGateDefines.h"
#include "Core/FRankHunterTypes.h"
#include "FHGameInstance.generated.h"

class UFHFadeInOut;
class UFHLoading;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGameInstance : public UFHAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	

public:
	UFHGameInstance();

	bool bIsReturningToTitle{ false };

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);

	void ReturnToTitle();

	void SetGateRank(EGateRank Rank);
	EGateRank GetGateRank();
	FString GetGateRankString();

	void SetPlayerCount(int32 Count) { PlayerCount = Count; };
	const int32 GetPlayerCount() { return PlayerCount; }
	void SetLobbyState(bool State);

	UFUNCTION(BlueprintCallable)
	bool GetLobbyState() { return bIsMorning; }

	// 임시 _ 던전 입장시 갱신. 관전모드 HUD 전용
	void SetCurrentMoneyForGate(int32 money) { CurrentMoneyForGate = money; }
	int32 GetCurrentMoneyForGate() const { return CurrentMoneyForGate; }
	void SetRequiredMoneyForGate(int32 money) { RequiredMoneyForGate = money; }
	int32 GetRequiredMoneyForGate() const { return RequiredMoneyForGate; }

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void PlayFadeIn(FOnFadeEnd FadeEndDelegate);
	UFUNCTION(BlueprintCallable)
	void PlayFadeOut(FOnFadeEnd FadeEndDelegate);

	void InitLoadingWidget();
	UFUNCTION(BlueprintCallable)
	void ShowLoadingWidget();
	UFUNCTION(BlueprintCallable)
	void HideLoadingWidget();

	UFUNCTION()
	void ShowFadeWidget(UWorld* World);
	UFUNCTION(BlueprintCallable)
	void HideFadeWidget();

	FString GetErrorMsg() const { return ErrorMsg; }
	bool HasNetworkError() const { return bHasNetworkError; }
	void ErrorNotified() { bHasNetworkError = false; }
protected:
	UPROPERTY()
	TObjectPtr<UFHFadeInOut> FadeInOutInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FadeWidget")
	TSubclassOf<UFHFadeInOut> FadeInOutWidgetClass;
	UPROPERTY()
	TObjectPtr<UFHLoading> LoadingInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LoadingWidget")
	TSubclassOf<UFHLoading> LoadingWidgetClass;

private:
	UPROPERTY(VisibleAnywhere, Category="GateRank")
	FString GateRank;

	int32 PlayerCount;
	bool bIsMorning{ true };


	int32 CurrentMoneyForGate;
	int32 RequiredMoneyForGate;

	bool bHasNetworkError = false;
	FString ErrorMsg;
};
