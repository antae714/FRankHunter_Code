// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameSavable.h"
#include "FRankHunterTypes.h"
#include "FHGateGameStateBase.generated.h"

class AFHPlayerController;
class AFHPlayerStateBase;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerTickDelegate, int32, ElapsedSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadRateReplicated, float, Rate);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoreCountChangedDelegate, int32, LeftCore);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerInfoOnPlayerStateChanged, int32, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuotaInfoChangedDelegate, int32, PlayerMoney, int32, RequiredMoney);

USTRUCT(BlueprintType)
struct FPlayerInfoArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FPlayerInfoOnPlayerStateChanged OnPlayerStateChangedDelegate;

	// Getter
	TArray<FPlayerInfo>& GetPlayerList() { return Players; }
	void GetAlivePlayerList(TArray<AFHPlayerStateBase*>& InPlayerArr);
	void GetDiedPlayerList(TArray<AFHPlayerStateBase*>& InPlayerArr);
	void GetNonAlivePlayerList(TArray<AFHPlayerStateBase*>& InPlayerArr);
	int32 GetAlivePlayerCount();
	FPlayerInfo& GetPlayerInfoByIndex(int32 Index) { return Players[Index]; }
	const EPlayerGateState GetPlayerGateStateByIndex(int32 Index);
	const EPlayerGateState GetPlayerGateStateByPlayerState(AFHPlayerStateBase* PlayerState);
	//
	
	// Server Only
	bool SetPlayerEscape(AFHPlayerStateBase* Player);
	bool SetPlayerDied(AFHPlayerStateBase* Player);
	void AddPlayer(FPlayerInfo& PlayerInfo);
	void ClearArray() { Players.Empty(); }
	void IncreaseMagicStoneValue(AFHPlayerStateBase* Player, float IncreaseValue);
	//

	// Client Only
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FPlayerInfo, FPlayerInfoArray>(Players, DeltaParams, *this);
	}

	UPROPERTY(VisibleAnywhere, Category = "PlayerInfoArray")
	TArray<FPlayerInfo> Players;
};

template<>
struct TStructOpsTypeTraits<FPlayerInfoArray> : public TStructOpsTypeTraitsBase2<FPlayerInfo>
{
	enum { WithNetDeltaSerializer = true };
};

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHGateGameStateBase : public AGameStateBase, public IGameSavable
{
	GENERATED_BODY()

public:
	AFHGateGameStateBase();

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	// Begin IGameSavable Implementation
	virtual FString GetSaveSlot() const override;
	virtual bool IsGlobal() const override;
	virtual void SerializeData(FArchive& Ar) override;
	// ~End IGameSavable Implementation

	void SaveState();

	TArray<FName> SelectedPawnKey;
	int32 CurrentReturnedCoreCount = 0;
public:
	UPROPERTY(BlueprintAssignable)
	FOnLoadRateReplicated OnLoadRateReplicatedDelegate;

	void AddDeathCount();

	void AddPlayer(FPlayerInfo& PlayerInfo);

	/** For CheatManager */
	void ClearPlayerList();

	FPlayerInfoArray& GetPlayerList() { return PlayerList; }

	UFUNCTION(BlueprintCallable)
	void GetAlivePlayerList(TArray<AFHPlayerStateBase*>& OutList);
	UFUNCTION(BlueprintCallable)
	void GetDiedPlayerList(TArray<AFHPlayerStateBase*>& OutList);
	UFUNCTION(BlueprintCallable)
	void GetNonAlivePlayerList(TArray<AFHPlayerStateBase*>& OutList);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnPlayerDied(APlayerState* DiedPlayerState);
	void MulticastOnPlayerDied_Implementation(APlayerState* DiedPlayerState);	
	UFUNCTION(NetMulticast, Reliable)
	void ShowExplorationResult();

	void AddLogOutPlayer(APlayerState* LogOutPlayer);



	// =======================================
	// ======= Client Repl Properties ========
	// =======================================
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="LoadingData", ReplicatedUsing="OnRep_LoadRate")
	float LoadRate{ 0.0f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GateTime", ReplicatedUsing="OnRep_TimeLimit")
	int32 TimeLimit;

	UPROPERTY(VisibleAnywhere, Category="GateData", ReplicatedUsing="OnRep_IsCollapsing")
	uint32 bIsCollapsing : 1;

	UPROPERTY(VisibleAnywhere, Category="GateData", ReplicatedUsing="OnRep_IsGateOpen")
	uint32 bIsGateOpen : 1;
	// =======================================

	// =======================================
	// ======= Client Environment ============
	// =======================================
	//void GateStart();
	void GateEnd();
	///void ChangeTimeLimit(float deltaTime);

	UFUNCTION()
	void OnRep_TimeLimit();

	UFUNCTION()
	void OnRep_LoadRate();

	FOnTimerTickDelegate OnTimerTickEvent;

	UFUNCTION()
	void OnRep_IsCollapsing();

	UFUNCTION()
	void OnRep_IsGateOpen();
	// =======================================


	FString GetModifiedStringFromTimeLimit();

private:

	UPROPERTY(Replicated)
	FPlayerInfoArray PlayerList;

	UPROPERTY()
	TArray<FString> LogOutPlayerSlotNameArray;

	UPROPERTY()
	TArray<TObjectPtr<USaveGame>> LogOutPlayerArray;

	int32 LoopCount;
	int32 DeathCount;
	uint32 bIsLAN : 1;
	uint32 bIsPublic : 1;
	uint32 bIsFriendOnly : 1;
	FString Password;
	int32 LoopCount_AtCurrentRank = 1;
	ELicenseRank TeamLicenseRank = ELicenseRank::None;

public:
	UPROPERTY(ReplicatedUsing = "OnRep_CurrentCoreCount")
	int32 CurrentCoreCount;
	UFUNCTION()
	void OnRep_CurrentCoreCount();
	FOnCoreCountChangedDelegate OnCoreCountChangedEvent;
	int32 GetCurrentCoreCount(){ return CurrentCoreCount; }

	FOnQuotaInfoChangedDelegate OnQuotaInfoChangedEvent;

	UPROPERTY(ReplicatedUsing = "OnRep_PlayerMoney")
	int32 PlayerMoney;

	UPROPERTY(ReplicatedUsing = "OnRep_RequiredMoney")
	int32 RequiredMoney;

	UPROPERTY(Replicated)
	float TotalMagicStoneValue;

	UFUNCTION(BlueprintCallable)
	float GetTotalMagicStoneValue() { return TotalMagicStoneValue; }

	int32 GetPlayerMoney(){ return PlayerMoney; }
	int32 GetRequiredMoney(){ return RequiredMoney; }

	UFUNCTION()
	void OnRep_PlayerMoney();

	UFUNCTION()
	void OnRep_RequiredMoney();

	// Minimap
public:
	UPROPERTY(Replicated)
	TArray<uint8> RoomDisclosedStates;
	bool IsRoomDisclosed(int64 RoomID);

	UFUNCTION(BlueprintCallable)
	void NotifyMagicStoneDestroyed(class AFHPlayerStateBase* PlayerState, float Value);

	UFUNCTION(Server, Reliable)
	void Server_NotifyRoomDisclosed(int64 RoomID);
	void Server_NotifyRoomDisclosed_Implementation(int64 RoomID);
};
DECLARE_LOG_CATEGORY_EXTERN(LogGateState, Log, All);