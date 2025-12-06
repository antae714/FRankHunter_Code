// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FHLobbyGameMode.generated.h"

enum class EGateRank : uint8;
struct FCachedSellItemInfo;
class AFH_GS_LobbyGameState;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFHLobbyGameMode();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override; 
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* PlayerController) override;
	virtual void Logout(AController* Exiting) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	virtual void PostSeamlessTravel() override;

	void DelayInit();
	void DelaySettingPostSeamlessTravel();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateSession();
	UFUNCTION(BlueprintImplementableEvent)
	void ClearAllVoice();
	UFUNCTION(BlueprintImplementableEvent)
	void CheckLicenseUpgrade();

	void PlayerEnterReady();
	int32 EnterReadyPlayerCount = 0;
	void ChangeMap();

	void InitShopItemList();
	void ResetShopItemList();
	UFUNCTION(Exec)
	void ResetGateSelect();
	bool CheckQuota();
	UFUNCTION(BlueprintCallable)
	void NotifyRest();

	UFUNCTION(BlueprintCallable)
	void OpenEnterRequestWidget(const FString& GateName, const EGateRank& EnterGateRank);


	UFUNCTION(Server, Reliable)
	void AnswerToRequest(bool Answer, APlayerController* PlayerController);
	void AnswerToRequest_Implementation(bool Answer, APlayerController* PlayerController);

	UFUNCTION(Server, Reliable)
	void SubmitPay(int32 Value);
	void SubmitPay_Implementation(int32 Value);

	UFUNCTION(Server, Reliable)
	void PlayerReady(APlayerController* Player);
	void PlayerReady_Implementation(APlayerController* Player);

	UFUNCTION(Server, Reliable)
	void PurchaseItem(APlayerController* Player, const TArray<struct FPurchaseData>& ItemData, int32 TotalPrice);
	void PurchaseItem_Implementation(APlayerController* Player, const TArray<struct FPurchaseData>& ItemData, int32 TotalPrice);

	UFUNCTION(Server, Reliable)
	void AddToSellItemCart(APlayerController* Player, const struct FSellItemInfo& ItemInfo);
	void AddToSellItemCart_Implementation(APlayerController* Player, const struct FSellItemInfo& ItemInfo);

	UFUNCTION(Server, Reliable)
	void ItemMoveToCart(APlayerController* Player, const struct FSellItemInfo& ItemInfo);
	void ItemMoveToCart_Implementation(APlayerController* Player, const struct FSellItemInfo& ItemInfo);

	UFUNCTION(Server, Reliable)
	void SellItem(const TArray<FSellItemInfo>& SellItems, int32 TotalPrice);
	void SellItem_Implementation(const TArray<FSellItemInfo>& SellItems, int32 TotalPrice);

	UFUNCTION(Server, Reliable)
	void AnswerRest(bool Answer, APlayerController* PlayerController);
	void AnswerRest_Implementation(bool Answer, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Session")
	void RequestChangePassword(const FString& NewPassword);
	void RequestChangePassword_Implementation(const FString& NewPassword);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataTable")
	TObjectPtr<UDataTable> AreaAdjacentData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataTable")
	TObjectPtr<UDataTable> QuotaIncreaseTable;

	UPROPERTY()
	TArray<uint8> SelectedGateInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnedActor")
	TSubclassOf<class AFHWorldItemActor> DropItemActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpawnedActor")
	TSubclassOf<class AFHBackpack> BackPackActorClass;

	UPROPERTY()
	TObjectPtr<AFH_GS_LobbyGameState> LobbyGameState;

	FTimerHandle TimerHandle;
	void TimerCountDown();

	UPROPERTY()
	TObjectPtr<class AFHPropManager> PropManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SelectGateCount")
	int32 GateCount = 6;
	UPROPERTY(BlueprintReadWrite)
	FString Password;

	const FName PASSWORD_KEY = TEXT("FRankHunter");

	// ================ Return Core ====================
	void ReturnCore(APlayerController* Player);

	// ================ Select Character and Skill =============
	void InitSelectInfo();
	void SlotSelected(int32 Index);
	void SlotReleaseSelect(int32 Index);

	UFUNCTION(BlueprintCallable)
	bool IsValidKey(FName ActorKey);

	UFUNCTION(BlueprintCallable)
	void PlayerSelectCompleted(APlayerController* Player, AActor* MeshPreviewActor, FName ActorKey, FName SkillID);

	// ================ Logic Test Area =====================

	UFUNCTION(Server, Reliable)
	void SetNextRound();
	void SetNextRound_Implementation();

	UFUNCTION(Server, Reliable)
	void IncreaseDeathCount();
	void IncreaseDeathCount_Implementation();

	UFUNCTION(Server, Reliable)
	void IncreaseMoney();
	void IncreaseMoney_Implementation();

	UFUNCTION(Exec)
	void SetGate();

	bool bIsMoveFromNight = false;
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FString> GetPlayerNames();

	void OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	TArray<FString> AnsweredPlayerNames;
	TArray<FString> RefusedPlayerNames;
};

DECLARE_LOG_CATEGORY_EXTERN(LogLobby, Log, All);