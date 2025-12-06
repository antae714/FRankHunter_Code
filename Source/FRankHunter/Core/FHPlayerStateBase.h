// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameSavable.h"
#include "Interfaces\SiInventorySystemInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FRankHunterTypes.h"
#include "FHPlayerStateBase.generated.h"

class UCommonActivatableWidget;
class AFHBackpack;
class UFHInventoryComponent;
struct FPlayerSaveData;
enum class ELicenseRank : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatUpdateDelegate, uint32);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLicenseRankUpdated, ELicenseRank)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBackpackChange, AFHBackpack*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnToggleTerminal, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeInteractProgressPercent, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadEnd, FPlayerSaveData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillUIFeedback, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillUIDuringCooldown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlockSkill);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPlayerGateStateChanged, AFHPlayerStateBase*, Player);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSkillSetted, const FName&, SkillID);

UENUM(BlueprintType)
enum class EPrimaryAttribute : uint8
{
	Vitality,
	Strength,
	Agility,
	Endurance,
	Willpower
};


USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()
	UPROPERTY()
	ELicenseRank LicenseRank = ELicenseRank::None;

	UPROPERTY()
	uint32 Level = 0;

	UPROPERTY()
	uint32 Exp = 0;

	UPROPERTY()
	uint32 RequiredExp = 0;

	UPROPERTY()
	uint32 StatPoints = 0;


	UPROPERTY()
	float Health = 0.0f;

	UPROPERTY()
	uint32 Vitality = 0;

	UPROPERTY()
	uint32 Strength = 0;

	UPROPERTY()
	uint32 Endurance = 0;

	UPROPERTY()
	uint32 Agility = 0;

	UPROPERTY()
	uint32 Willpower = 0;

	UPROPERTY()
	FName SkillID = FName();

	UPROPERTY()
	bool IsFirstPlay = true;

	UPROPERTY()
	FName PawnKey = FName();

	UPROPERTY()
	FLinearColor SuitColor;
	UPROPERTY()
	float SuitColorMultiplier{ 0.0f };

	bool Serialize(FArchive& Ar);
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FPlayerSaveData> : public TStructOpsTypeTraitsBase2<FPlayerSaveData>
{
	enum
	{
		WithSerializer = true,
		WithNetSerializer = true
	};
};

/**
 * Role
 * 1. Initialize AbilitySystemComponent (including attach attribute sets)
 * 2. Manage lv, exp, statpoints
 */
UCLASS()
class FRANKHUNTER_API AFHPlayerStateBase : public APlayerState, 
	public IAbilitySystemInterface, 
	public ISiInventorySystemInterface, 
	public IGameSavable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UFHAbilitySystemComponent> abilitySystem;

protected:
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_Health> HealthAttributeSet;
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_Stamina> StaminaAttributeSet;
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_Movement> MovementAttributeSet;
	UPROPERTY()
	TObjectPtr<class UFHAttributeSet_PlayerStatus> PlayerAttributeSet;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UFHInventoryComponent> InventoryComponent;

public:
	TObjectPtr<class UFHAttributeSet_Health> GetHealthAttributeSet() const { return HealthAttributeSet; }
	TObjectPtr<class UFHAttributeSet_Stamina> GetStaminaAttributeSet() const { return StaminaAttributeSet; }
	TObjectPtr<class UFHAttributeSet_Movement> GetMovementAttributeSet() const { return MovementAttributeSet; }
	TObjectPtr<class UFHAttributeSet_PlayerStatus> GetPlayerAttributeSet() const { return PlayerAttributeSet; }

	virtual USiInventoryComponent* GetInventoryComponent() const override;

	// Begin IGameSavable Implementation
	virtual FString GetSaveSlot() const override;
	virtual bool IsGlobal() const override;
	virtual void SerializeData(FArchive& Ar) override;
	// ~End IGameSavable Implementation

	FOnBackpackChange OnBackpackChange;
	void SetCurrentBackpackActorCache(AFHBackpack* Backpack);
	AFHBackpack* GetCurrentBackpackActorCache();

	FOnToggleTerminal OnToggleTerminalDelegate;
	FOnChangeInteractProgressPercent OnChangeInteractProgressPercentDelegate;
	FOnLoadEnd OnLoadEndDelegate;

	bool IsLoaded() const { return bIsLoaded; }
	void BindOrExecuteOnPlayerStateLoaded(const FOnLoadEnd::FDelegate& Delegate);

	UFUNCTION()
	void OnPlayerStateChanged(int32 ChangedIndex);
	FOnPlayerGateStateChanged OnPlayerGateStateChangedDelegate;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UCommonActivatableWidget> StatusUIClass;

private:
	UPROPERTY(ReplicatedUsing = OnRep_LicenseRank)
	ELicenseRank LicenseRank{ ELicenseRank::F };

	UPROPERTY()
	TObjectPtr<AFHBackpack> CurrentBackpackActorCache;
	UPROPERTY(ReplicatedUsing = OnRep_Level)
	int32 Level{ 1 };

	UPROPERTY(ReplicatedUsing = OnRep_Exp)
	int32 Exp{ 0 };

	UPROPERTY(ReplicatedUsing = OnRep_RequiredExp)
	int32 RequiredExp{ 100 };

	UPROPERTY(ReplicatedUsing = OnRep_StatPoints)
	int32 StatPoints{ 0 };

	int32 AddStatPointsAmountPerLevel { 1 };

	FString NickName;
public:
	FOnLicenseRankUpdated OnLicenseRankUpdated;

	FOnStatUpdateDelegate OnLevelUp;
	FOnStatUpdateDelegate OnExpGain;
	FOnStatUpdateDelegate OnRequiredExp;
	FOnStatUpdateDelegate OnUseStatPoints;

protected:
	UFUNCTION()
	void OnRep_LicenseRank();

	UFUNCTION()
	void OnRep_Level();

	UFUNCTION()
	void OnRep_Exp();

	UFUNCTION()
	void OnRep_RequiredExp();

	UFUNCTION()
	void OnRep_StatPoints();

	
public:
	AFHPlayerStateBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnSetUniqueId() override;
	virtual void SeamlessTravelTo(class APlayerState* NewPlayerState) override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void AddExp(int32 amount);
	void AddExp_Implementation(int32 amount);
	
	UFUNCTION(BlueprintCallable)
	int32 GetExp() { return Exp; };
	UFUNCTION(BlueprintCallable)
	int32 GetRequiredExp() { return RequiredExp; }
	UFUNCTION(BlueprintCallable)
	int32 GetPlayerLevel() { return Level; }
	UFUNCTION(BlueprintCallable)
	int32 GetStatPoints() { return StatPoints; }
	UFUNCTION(BlueprintCallable)
	ELicenseRank GetLicenseRank() { return LicenseRank;}
	UFUNCTION(BlueprintCallable)
	void UpgradeRank();
	UFUNCTION(BlueprintCallable)
	void ResetRank();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void UseStatPointsToIncreaseStat(EPrimaryAttribute stat, int32 amount);

	UFUNCTION(BlueprintCallable)
	void InitVoiceSetting();

public:
	void SetIsObserving(bool bIsObserving);

	UFUNCTION(BlueprintPure, Category = "PlayerState")
	bool IsObserving() const { return bIsObserving; }

private:
	UPROPERTY(ReplicatedUsing = OnRep_IsObserving)
	bool bIsObserving;

protected:
	UFUNCTION()
	void OnRep_IsObserving();

	// Setting
public:
	UFUNCTION()
	void StartSetting(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	bool bIsStartSettingInProgress{ false };

	// Player Skill
public:
	UFUNCTION(Server, Reliable)
	void AddStatPoints(int32 Amount);
	void AddStatPoints_Implementation(int32 Amount);

	void SetPawnMesh(FName ActorKey);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetPawnMesh(FName ActorKey);
	void NetMulticast_SetPawnMesh_Implementation(FName ActorKey);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetPawnMesh(const FName& ActorKey);


	void SetSkillID(FName SkillID);
	void CallRPC_SetSkillID(FName SkillID);

	UFUNCTION(BlueprintCallable, Category = "SkillID")
	FName GetSkillID() { return PlayerSkillID; }
	UFUNCTION(BlueprintCallable, Category = "PawnID")
	FName GetPawnKey() { return PawnKey;	}
	void UpdateSkillWidget();
	FOnSkillSetted OnSkillSettedDelegate;
	FTimerHandle RetryHandle;
	//void GrantSkill();

	UFUNCTION(Client, Reliable)
	void StartFirstEnterProcess();
	void StartFirstEnterProcess_Implementation();

	bool IsFirstPlay() { return IsFirstEnter; }

	UFUNCTION(Client, Reliable)
	void Client_OnSkillFeedback(bool bSuccess);
	void Client_OnSkillFeedback_Implementation(bool bSuccess);

	UFUNCTION(Client, Reliable)
	void Client_OnSkillCooldown();
	void Client_OnSkillCooldown_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_OnBlockSkill();
	void Client_OnBlockSkill_Implementation();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSkillID)
	FName PlayerSkillID;
	UFUNCTION()
	void OnRep_PlayerSkillID();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSkillUIFeedback OnSkillUIFeedback;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSkillUIDuringCooldown OnSkillUIDuringCooldown;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBlockSkill OnBlockSkill;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PawnKey)
	FName PawnKey;

	UFUNCTION()
	void OnRep_PawnKey();


public:
	UPROPERTY(ReplicatedUsing = OnRep_SuitColor, BlueprintReadOnly)
	FLinearColor SuitColor{ 0, 0, 0, 1 };
	UFUNCTION()
	void OnRep_SuitColor();
	UPROPERTY(ReplicatedUsing = OnRep_SuitColorMultiplier)
	float SuitColorMultiplier;
	UFUNCTION()
	void OnRep_SuitColorMultiplier();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetSuitColor(FLinearColor Color, float ColorMultiplier);
	void SetSuitColor_Implementation(FLinearColor Color, float ColorMultiplier);
	void SetFirstEnterOff() { IsFirstEnter = false; }
private:
	bool IsFirstEnter = true;

protected:
	void LevelUp(int32 amount);
	void UpdateRequiredExp(int32 NewLevel);
	bool ApplyUpdateStatusEffect(TSubclassOf<class UGameplayEffect> effect, FGameplayTag tag, float amount);

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> IncreaseStrengthEffect;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> IncreaseAgilityEffect;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> IncreaseEnduranceEffect;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> IncreaseWillpowerEffect;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> IncreaseVitalityEffect;

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	bool TryLoad();
	void TrySave();
private:

	uint32 bIsLoaded : 1;
	uint32 bIsLoadReady : 1;
	uint32 bIsSave : 1;


	UPROPERTY(ReplicatedUsing = OnRepPlayerSaveData)
	FPlayerSaveData PlayerSaveData;

	UFUNCTION()
	void OnRepPlayerSaveData();

#if WITH_EDITORONLY_DATA 
	int32 PIEInstanceId = -1;
#endif
};
