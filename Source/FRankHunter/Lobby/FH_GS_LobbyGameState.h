// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameSavable.h"
#include "Core/FHGameDataBundle.h"
#include "Core/FRankHunterTypes.h"
#include "FH_GS_LobbyGameState.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInt32Event, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameDataLoadEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerCountEnd);


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFH_GS_LobbyGameState : public AGameStateBase, public IGameSavable
{
	GENERATED_BODY()
	
public:
	AFH_GS_LobbyGameState();


public:
	UPROPERTY(BlueprintAssignable)
	FOnInt32Event OnChangedPlayerMoney;

	FOnInt32Event OnChangedRequiredMoney;
	FOnInt32Event OnLeftTimeReplicatedDelegate;
	FOnTimerCountEnd OnTimerEndDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnGameDataLoadEnd OnLoadEnded;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Begin IGameSavable Implementation
	virtual FString GetSaveSlot() const override;
	virtual bool IsGlobal() const override;
	virtual void SerializeData(FArchive& Ar) override;
	// ~End IGameSavable Implementation

public:
	void LoadState();

	UFUNCTION(BlueprintCallable)
	void SaveState();
	
	void ResetCount();
	UFUNCTION(BlueprintCallable)
	void Rest();
	void SetPayment();
	void ApplyPenalty();
	UFUNCTION(BlueprintCallable)
	void GameOver();
	void RestartGame();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	int GetCurrentYear();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	int GetCurrentMounth();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	int GetRemainingExplorationChances();

	UFUNCTION(NetMulticast, Reliable)
	void PlayRestResult(int32 loopCount);
	void PlayRestResult_Implementation(int32 loopCount);
	UFUNCTION(NetMulticast, Reliable)
	void PlayGameOver();
	void PlayGameOver_Implementation();

	UFUNCTION(BlueprintCallable)
	int32 GetRequiredMoney() { return RequiredMoney; }
	UFUNCTION(BlueprintCallable)
	void AddMoney(int32 Amount);
	void SetMoney(int32 NewMoney);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 GetMoney();
	bool IsGameOver() { return bIsGameOver; }
	void SetLobbyState(bool State);
	UFUNCTION(BlueprintCallable)
	bool GetLobbyState() { return bIsMorning; }
	UFUNCTION(NetMulticast, Reliable)
	void DisablePreviewActor(const FName& PawnMeshKey);
	void DisablePreviewActor_Implementation(const FName& PawnMeshKey);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_DisablePreviewActor(const FName& PawnMeshKey);
public:

//private:

	UPROPERTY(VisibleAnywhere)
	int32 AcceptPlayerCount;
	
	UPROPERTY(VisibleAnywhere)
	int32 RefusePlayerCount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", ReplicatedUsing = "OnRep_LeftTime")
	int32 LeftTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", Replicated)
	int32 LoopCount;

	// NPC 대사 분기용, 현재 랭크에서 몇 번 돌았는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", Replicated)
	int32 LoopCount_AtCurrentRank;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", ReplicatedUsing = "OnRep_RequiredMoney")
	int32 RequiredMoney;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	int32 DeathCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	uint32 bIsLAN : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	uint32 bIsPublic : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	uint32 bIsFriendOnly : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataTable")
	TObjectPtr<UDataTable> AreaUIData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemTable")
	TObjectPtr<UDataTable> ItemTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Props")
	FText WhiteboardText;

	int32 PlayerCount = 0;

	UFUNCTION()
	void OnRep_LeftTime();

	UFUNCTION()
	void OnRep_RequiredMoney();

	uint32 bIsTravle : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TeamLicenseRank)
	ELicenseRank TeamLicenseRank = ELicenseRank::None;
	UFUNCTION()
	void OnRep_TeamLicenseRank();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentReturnedCoreCount = 0;


	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetLicenseRankAndReturnedCoreCount(ELicenseRank Rank, int32 ReturnedCoreCount, bool IsRankUp);
	void SetLicenseRankAndReturnedCoreCount_Implementation(ELicenseRank Rank, int32 ReturnedCoreCount, bool IsRankUp);

	TArray<FName> SelectedPawnKey;
private:

	UPROPERTY(VisibleAnywhere, Category = "Data", ReplicatedUsing = "OnRep_PlayerMoney")
	int32 PlayerMoney;

	UFUNCTION()
	void OnRep_PlayerMoney();


	UPROPERTY(EditAnywhere, Category = "Data")
	int32 InitRequireMagic;

	UPROPERTY(Replicated)
	uint32 bIsGameOver : 1;
	
	UPROPERTY(EditAnywhere, Category = "SaveData")
	FString PropSaveSlotName;

	UPROPERTY(Replicated)
	bool bIsMorning{ true };





protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnFirstPlayGameStarted();
};
