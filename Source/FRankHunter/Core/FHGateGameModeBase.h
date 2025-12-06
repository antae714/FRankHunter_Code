// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FRankHunterTypes.h"
#include "FHGateGameModeBase.generated.h"

class UUserWidget;
class AFHDungeonGenerator;

UENUM()
enum class EGateState : uint8
{
	Loading,
	InPlay
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGatePowerLevelChangeDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGateLoadingCompleteDelegate);

UCLASS()
class FRANKHUNTER_API AFHGateGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Generator")
	EBuildType DungeonGeneratorType;

	UPROPERTY(EditAnywhere, Category = "Generator")
	TSubclassOf<AFHDungeonGenerator> DefaultGeneratorClass;

	UPROPERTY(EditAnywhere, Category = "Generator")
	TMap<EBuildType, TSubclassOf<class AFHDungeonGenerator>> GeneratorClasses;

	// ======= Server Environment ============
	AFHGateGameModeBase();
	virtual void StartPlay() override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	virtual void PostSeamlessTravel() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	UFUNCTION()
	void SetLoadRate();

	void TryGateStart();
	void GateEnd();
	void IncreaseGatePowerLevel(bool bIsCoreDestroyed = false);

	UFUNCTION(BlueprintCallable)
	void PlayerEscape(class AFHPlayerController* Controller);

	UFUNCTION(BlueprintCallable)
	void PlayerDied(class AFHPlayerController* Controller);
	
	UFUNCTION(BlueprintCallable)
	void PossessObserverPawn(AFHPlayerController* Controller);

	UFUNCTION(BlueprintCallable)
	void InformCoreDestroyed();


	FTimerHandle TempTimer;
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void BackToLobby();
	void BackToLobby_Implementation();

public:
	UFUNCTION(Server, Reliable)
	void PlayerReady(APlayerController* PlayerController);
	void PlayerReady_Implementation(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ClearAllVoice();

	void PlayerEnterReady();
	int32 EnterReadyPlayerCount = 0;

	UFUNCTION()
	void OnGatePowerLevelChanged();

	/** For CheatManager*/
	void ForceClearGate();
	// ========================================


	// ========= Gate Generation ==========
	void InitGenerator(struct FFHGateSpecificationTableRow* GateSpecification);

	void ReplacePlayerCharacters();

	void MakePlayerReady();

	void GenerateCompleted();
	// ====================================


	// ========== Exit Gate ==============
	void UnloadDungeonAndTurnOffPhysics();
	// ===================================
	
public:
	UPROPERTY()
	FGatePowerLevelChangeDelegate OnGatePowerLevelChangeDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGateLoadingCompleteDelegate OnGateLoadingCompleteEvent;

	UPROPERTY(VisibleAnywhere, Category = "GateData")
	float AccumulatedSpawnPower;

	float GetGatePowerLevel() { return GatePowerLevel; }

	EGateState GetGateState() { return GateState; }
	AFHDungeonGenerator* GetGateGeneratorInstance() { return GateGeneratorInstance; }

	void PlayerLoadEnd(AController* LoadedPlayer);
private:
	float LastGateProgress{ 0.0 };
	int32 LoadEndPlayerCount = 0;

	UPROPERTY(VisibleAnywhere, Category = "GateData")
	int32 TotalPlayerCount;

	UPROPERTY(VisibleAnywhere, Category = "GateData")
	int32 ReadyPlayerCount;

	UPROPERTY(VisibleAnywhere, Category = "GateData")
	uint32 bIsGenerateComplete : 1;

	UPROPERTY()
	TArray<TObjectPtr<AController>> LoadedPlayers;

	UPROPERTY(VisibleAnywhere, Category = "GateData")
	float GatePowerLevel;

	UPROPERTY(VisibleAnywhere, Category = "GateData")
	uint32 PowerIncreaseStep;

	UPROPERTY(VisibleAnywhere)
	EGateState GateState{ EGateState::Loading };

	UPROPERTY(EditAnywhere, Category = "Generator")
	TObjectPtr<UDataTable> GateGenerateRuleTable;

	UPROPERTY()
	TObjectPtr<AFHDungeonGenerator> GateGeneratorInstance;

	UPROPERTY(EditAnywhere, Category="IncreaseTable")
	TObjectPtr<UDataTable> IncreasePowerTable;

	UPROPERTY()
	class UFHGameInstance* GateGameInstance;

	UPROPERTY()
	class AFHGateGameStateBase* GateGameState;

	FTimerHandle GateTimerHandle;
	FTimerHandle ExitTimerHandle;
	FTimerHandle PowerLevelIncreaseEventHandle;

	FTimerHandle GateStartDelayTimerHandle;
	FTimerHandle UpdateMinimapVisibilityHandle;
	FTimerHandle StartGameTimerHandle;

	FTimerHandle LogOutGateEndTimer;
	FTimerHandle EscapeGateEndTimer;
	FTimerHandle PlayerDiedGateEndTimer;
};

DECLARE_LOG_CATEGORY_EXTERN(LogGate, Log, All);