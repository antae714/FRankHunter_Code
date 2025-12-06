// Copyright F Rank Hunter.. All Rights Reserved.

#include "Core/FHPlayerStateBase.h"
#include "GameFramework/GameState.h"
#include "Net/UnrealNetwork.h"
#include "FRankHunter.h"
#include "GAS/FHGameplayTags.h"
#include "GameplayEffect.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "GAS/FHAttributeSet_Health.h"
#include "GAS/FHAttributeSet_Stamina.h"
#include "GAS/Attributes/FHAttributeSet_Movement.h"
#include "GAS/FHAttributeSet_PlayerStatus.h"
#include "GAS/Ability/Skill/FHGamePlaySkillAbility.h"
#include "Core/FHFRankHunterSettings.h"
#include "Player/FHLevelUpExpRow.h"
#include "CommonActivatableWidget.h"
#include "SimpleSaveKitFunctionLibrary.h"
#include "Player/FHPlayerBase.h"
#include "Item/Actors/FHBackpack.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHUIManager.h"
#include "UI/Voice/FHVoiceWidget.h"
#include "Item/FHInventoryComponent.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Core/FHGateGameStateBase.h"
#include "Core/FHPlayerController.h"
#include "UI/FHStatusHudBase.h"
#include "Data/FHSkillTable.h"
#include "Component/ActorRPC/FHActorRPCComponent.h"
#include "Core/GameDataSubsystem.h"
#include "Player/FHObserverPawn.h"

// Voice Chat
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/VoiceInterface.h"

// UE LOG CATEGORY
#include "FRankHunter.h"
#include "Kismet/GameplayStatics.h"


bool FPlayerSaveData::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);

	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) >= FFRankHunterCustomVersion::FirstVersion)
	{
		Ar << LicenseRank;
		Ar << Level;
		Ar << Exp;
		Ar << RequiredExp;
		Ar << StatPoints;

		Ar << Health;
		Ar << Vitality;
		Ar << Strength;
		Ar << Endurance;
		Ar << Agility;
		Ar << Willpower;

		Ar << SkillID;
		Ar << IsFirstPlay;
		Ar << PawnKey;

		Ar << SuitColor;
		Ar << SuitColorMultiplier;
	}

	return true;
}

bool FPlayerSaveData::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << LicenseRank;
	Ar.SerializeIntPacked(Level);
	Ar.SerializeIntPacked(Exp);
	Ar.SerializeIntPacked(RequiredExp);
	Ar.SerializeIntPacked(StatPoints);

	Ar << Health;
	Ar.SerializeIntPacked(Vitality);
	Ar.SerializeIntPacked(Strength);
	Ar.SerializeIntPacked(Endurance);
	Ar.SerializeIntPacked(Agility);
	Ar.SerializeIntPacked(Willpower);

	Ar << SkillID;
	Ar << IsFirstPlay;
	Ar << PawnKey;

	Ar << SuitColor;
	Ar << SuitColorMultiplier;

	return true;
}

USiInventoryComponent* AFHPlayerStateBase::GetInventoryComponent() const
{
	return InventoryComponent;
}

FString AFHPlayerStateBase::GetSaveSlot() const
{
	/*FString SlotName;

	SlotName = GetPlayerName();

#if WITH_EDITOR    
	SlotName = FString::FromInt(PIEInstanceId);
#endif

	return SlotName;*/


	const FString PlayerName = GetPlayerName();

#if WITH_EDITOR
	if (!PlayerName.IsEmpty())
	{
		return PlayerName;
	}
#endif

	const FUniqueNetIdRepl& PlayerUniqueId = GetUniqueId();
	if (PlayerUniqueId.IsValid())
	{
		// PlayerUniqueId.ToString() 쓰려면 플러그인 2개 추가해야됨...
		IOnlineIdentityPtr OnlineIdentity = Online::GetIdentityInterface(TEXT("Null"));
		if (OnlineIdentity.IsValid())
		{
			FTCHARToUTF8 UTF8Converter(*PlayerName);
			const uint8* Bytes = reinterpret_cast<const uint8*>(UTF8Converter.Get());
			int32 Size = UTF8Converter.Length();

			FString HexString = BytesToHex(Bytes, Size);
			return HexString;
		}

		const TSharedPtr<const FUniqueNetId> UniqueNetIdPtr = PlayerUniqueId.GetUniqueNetId();
		if (UniqueNetIdPtr.IsValid())
		{
			const uint8* Bytes = UniqueNetIdPtr->GetBytes();
			const int32 Size = UniqueNetIdPtr->GetSize();

			return BytesToHex(Bytes, Size);
		}
	}


	UE_LOG(LogTemp, Error, TEXT("GetSaveSlot: 유효한 플레이어 ID를 찾을 수 없습니다."));

	if (!PlayerName.IsEmpty())
	{
		return PlayerName;
	}

	return TEXT("");
}

bool AFHPlayerStateBase::IsGlobal() const
{
	return true;
}

void AFHPlayerStateBase::SerializeData(FArchive& Ar)
{
	check(HasAuthority());

	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);

	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) >= FFRankHunterCustomVersion::FirstVersion)
	{
		PlayerSaveData.Serialize(Ar);

		UClass* BackpackClass = CurrentBackpackActorCache.GetClass();
		AFHBackpack* Backpack = CurrentBackpackActorCache;
		Ar << BackpackClass;
		




		if (Ar.IsLoading())
		{
			Backpack = GetWorld()->SpawnActor<AFHBackpack>(BackpackClass);
		}

		if (Backpack)
		{
			USimpleSaveKitFunctionLibrary::SerializeActor(Ar, Backpack);
		}
		CurrentBackpackActorCache = Backpack;
	}
}


void AFHPlayerStateBase::OnRep_LicenseRank()
{
	OnLicenseRankUpdated.Broadcast(LicenseRank);
}

void AFHPlayerStateBase::SetCurrentBackpackActorCache(AFHBackpack* Backpack)
{
	CurrentBackpackActorCache = Backpack;
	OnBackpackChange.Broadcast(CurrentBackpackActorCache);
}

AFHBackpack* AFHPlayerStateBase::GetCurrentBackpackActorCache()
{
	return CurrentBackpackActorCache;
}

void AFHPlayerStateBase::BindOrExecuteOnPlayerStateLoaded(const FOnLoadEnd::FDelegate& Delegate)
{
	if (IsLoaded())
	{
		Delegate.ExecuteIfBound(PlayerSaveData);
	}
	else
	{
		OnLoadEndDelegate.Add(Delegate);
	}
}

void AFHPlayerStateBase::OnPlayerStateChanged(int32 ChangedIndex)
{
	NET_DEBUG_LOG(TEXT(""));

	/*
		상태가 바뀌는 경우는 탈출하거나 죽는 경우밖에 없다.
	*/
	AFHGateGameStateBase* GameState = Cast<AFHGateGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
		if (!OnlineSubsystem)
		{
			return;
		}
		IOnlineVoicePtr OnlineVoicePtr = OnlineSubsystem->GetVoiceInterface();
		if (!OnlineVoicePtr.IsValid())
		{
			return;
		}

		FPlayerInfoArray& InfoArray = GameState->GetPlayerList();
		FPlayerInfo& PlayerInfo = InfoArray.GetPlayerInfoByIndex(ChangedIndex);

		if (!PlayerInfo.PlayerState)
		{
			return;
		}

		// 플레이어가 최초로 게이트에 진입했을 때 체크하는 부분.
		if (PlayerInfo.PlayerGateState == EPlayerGateState::Alive)
		{
			PRINT_LOG(TEXT("Start UnmuteRemoteTalker. UnmutedPlayer: %s"), *PlayerInfo.PlayerState->GetPlayerName());
			OnlineVoicePtr->UnmuteRemoteTalker(0, *PlayerInfo.PlayerState->GetUniqueId(), false);
			return;
		}
		
		// 자신의 상태가 생존 상태면 변경된 타 플레이어를 음소거 시킴.
		const EPlayerGateState& MyStateInfo = InfoArray.GetPlayerGateStateByPlayerState(this);
		if (MyStateInfo == EPlayerGateState::Alive)
		{
			PRINT_LOG(TEXT("Start MuteRemoteTalker. MutedPlayer: %s"), *PlayerInfo.PlayerState->GetPlayerName());
			OnlineVoicePtr->MuteRemoteTalker(0, *PlayerInfo.PlayerState->GetUniqueId(), false);
		}
		// 자신의 상태가 관전 상태면 관전상태로 변한 타 플레이어를 음소거 해제 시킨다.
		else
		{
			TArray<AFHPlayerStateBase*> NonAlivePlayers;
			InfoArray.GetNonAlivePlayerList(NonAlivePlayers);

			for (AFHPlayerStateBase* PS : NonAlivePlayers)
			{
				if (!PS) continue;
				if (PS == this) continue;

				PRINT_LOG(TEXT("Start UnmuteRemoteTalker. UnmutedPlayer: %s"), *PS->GetPlayerName());
				OnlineVoicePtr->UnmuteRemoteTalker(0, *PS->GetUniqueId(), false);
			}
		}

		// send to observer hud.
		OnPlayerGateStateChangedDelegate.ExecuteIfBound(PlayerInfo.PlayerState);
	}
}

void AFHPlayerStateBase::OnRep_Level()
{
	OnLevelUp.Broadcast(Level);
}

void AFHPlayerStateBase::OnRep_Exp()
{
	OnExpGain.Broadcast(Exp);
}

void AFHPlayerStateBase::OnRep_RequiredExp()
{
	OnRequiredExp.Broadcast(RequiredExp);
}

void AFHPlayerStateBase::OnRep_StatPoints()
{
	OnUseStatPoints.Broadcast(StatPoints);
}

AFHPlayerStateBase::AFHPlayerStateBase()
{
	HealthAttributeSet = CreateDefaultSubobject<UFHAttributeSet_Health>(TEXT("HealthAttributeSet"));
	StaminaAttributeSet = CreateDefaultSubobject<UFHAttributeSet_Stamina>(TEXT("StaminaAttributeSet"));
	MovementAttributeSet = CreateDefaultSubobject<UFHAttributeSet_Movement>(TEXT("MovementAttributeSet"));
	PlayerAttributeSet = CreateDefaultSubobject<UFHAttributeSet_PlayerStatus>(TEXT("PlayerStatusAttributeSet"));

	abilitySystem = CreateDefaultSubobject<UFHAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	abilitySystem->SetIsReplicated(true);
	abilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	InventoryComponent = CreateDefaultSubobject<UFHInventoryComponent>(TEXT("FHInventory"));
	InventoryComponent->SetMaxItemCountBeforePlay(4);

	SetNetUpdateFrequency(100.0f);

	OnPawnSet.AddDynamic(this, &AFHPlayerStateBase::StartSetting);
}

void AFHPlayerStateBase::BeginPlay()
{
	Super::BeginPlay();

	NET_DEBUG_LOG(TEXT(""));

	UpdateRequiredExp(Level);

	//TryLoad();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InventoryComponent->Server_SetCurrentItemIndex(0);
	}
}

void AFHPlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);

	if (RetryHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RetryHandle);
	}

	NET_DEBUG_LOG(TEXT(""));

	bool isQuit = EndPlayReason == EEndPlayReason::Quit;
	AFH_GS_LobbyGameState* LobbyGameState = GetWorld()->GetGameState<AFH_GS_LobbyGameState>();
	bool isTravle = LobbyGameState ? LobbyGameState->bIsTravle : true;
	
	if (HasAuthority() && !isTravle)
	{
		TrySave();
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineVoicePtr OnlineVoicePtr = OnlineSubsystem->GetVoiceInterface();
		if (OnlineVoicePtr.IsValid())
		{

			PRINT_LOG(TEXT("[PS::EndPlay] OnlineVoice Is Valid."));
			APawn* Pawn = GetPawn();
			if (Pawn)
			{
				if (Pawn->IsLocallyControlled())
				{
					OnlineVoicePtr->StopNetworkedVoice(0);
					OnlineVoicePtr->UnregisterLocalTalkers();
					OnlineVoicePtr->RemoveAllRemoteTalkers();
				}
				else
				{
					OnlineVoicePtr->UnregisterRemoteTalker(*GetUniqueId());
				}
			}
		}
	}
}

void AFHPlayerStateBase::OnSetUniqueId()
{
	Super::OnSetUniqueId();
	//TryLoad();
}

void AFHPlayerStateBase::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);
	NewPlayerState->SetPlayerName(GetPlayerName());

	//TrySave();
	if (AFHPlayerStateBase* FHPlayerStateBase = Cast<AFHPlayerStateBase>(NewPlayerState); FHPlayerStateBase)
	{
		FHPlayerStateBase->TryLoad();
	}

}

UAbilitySystemComponent* AFHPlayerStateBase::GetAbilitySystemComponent() const
{
	return abilitySystem;
}

void AFHPlayerStateBase::AddExp_Implementation(int32 amount)
{
	Exp += amount;


	FGameplayCueParameters param;
	abilitySystem->ExecuteGameplayCue(GET_GAMEPLAY_TAG("GameplayCue.Player.GainEXP"), param);

	int32 levelToUp = 0;
	while (Exp >= RequiredExp)
	{
		Exp -= RequiredExp;
		++levelToUp;
		UpdateRequiredExp(Level + levelToUp);
	}
	if (levelToUp > 0)
	{
		LevelUp(levelToUp);

		abilitySystem->ExecuteGameplayCue(GET_GAMEPLAY_TAG("GameplayCue.Player.LevelUp"), param);
	}

	FH_MARK_PROPERTY_DIRTY(Exp);
	OnRep_Exp();
}

void AFHPlayerStateBase::InitVoiceSetting()
{
	NET_DEBUG_LOG(TEXT(""));

	IOnlineVoicePtr OnlineVoicePtr = Online::GetVoiceInterface();
	if (!OnlineVoicePtr.IsValid())
	{
		return;
	}

	PRINT_LOG(TEXT("[PS::BeginPlay] OnlineVoice Is Valid."));
	APawn* Pawn = GetPawn();
	if (!Pawn) 
	{
		PRINT_LOG(TEXT("[PS::BeginPlay] Pawn is nullptr."));
		return;
	}

	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	UFHVoiceWidget* Widget = UIManager ? UIManager->GetWidget<UFHVoiceWidget>(TEXT("VoiceInterface")) : nullptr;
	if (Widget)
	{
		if (Pawn->IsLocallyControlled())
		{
			Widget->RegistLocalPlayer(GetUniqueId());
		}
		else
		{
			Widget->RegistRemotePlayer(GetUniqueId());
		}
	}

	if (Pawn->IsLocallyControlled())
	{
		IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicInputGain"))->Set(6.0f);
		IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicNoiseGateThreshold"))->Set(0.01f);
		IConsoleManager::Get().FindConsoleVariable(TEXT("voice.SilenceDetectionThreshold"))->Set(0.01f);

		PRINT_LOG(TEXT("[PS::BeginPlay] Local Client Regist Talker."));

		OnlineVoicePtr->RegisterLocalTalker(0);
		OnlineVoicePtr->StopNetworkedVoice(0);
		//OnlineVoicePtr->StartNetworkedVoice(0);

		AFHGateGameStateBase* GameState = Cast<AFHGateGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
		if (GameState)
		{
			GameState->GetPlayerList().OnPlayerStateChangedDelegate.AddDynamic(this, &AFHPlayerStateBase::OnPlayerStateChanged);
		}

		if (!IsFirstEnter)
		{
			PRINT_LOG(TEXT("Local Client Talker Start NetworkVoice."));
			OnlineVoicePtr->StartNetworkedVoice(0);
		}
	}
	else
	{
		PRINT_LOG(TEXT("[PS::BeginPlay] Remote Player Start Added."));
		OnlineVoicePtr->RegisterRemoteTalker(*GetUniqueId());

	}
	
}

void AFHPlayerStateBase::SetIsObserving(bool bIsObserve)
{
	if (HasAuthority())
	{
		bIsObserving = bIsObserve;

		OnRep_IsObserving();
	}
}

void AFHPlayerStateBase::OnRep_IsObserving()
{
	if (abilitySystem)
	{
		if (bIsObserving)
		{
			abilitySystem->AddLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISOBSERVING);
		}
		else
		{
			abilitySystem->RemoveLooseGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISOBSERVING);
		}
	}


	AFHPlayerController* PC = Cast<AFHPlayerController>(GetOwningController());
	if (PC && PC->IsLocalPlayerController())
	{
		PC->StatusHudInstance->SwitchHUDType(!bIsObserving);
	}
}

void AFHPlayerStateBase::StartSetting(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (NewPawn == nullptr || Cast<AFHObserverPawn>(NewPawn))
	{
		return;
	}

	if (AFHPlayerController* PC = Cast<AFHPlayerController>(GetPlayerController()))
	{
		if (PC->GetStateName() == NAME_Spectating)
		{
			return;
		}
	}

	NET_DEBUG_LOG(TEXT("This Function Is Call After PlayerReady."));
	NET_DEBUG_LOG(TEXT("Start Setting Pawn Mesh and Skill."));
	//BP_SetPawnMesh(PawnKey);
	//CallRPC_SetSkillID(PlayerSkillID);

	// TODO : 로드 했을 때 Ability와 Hud에 스킬 아이콘, 어빌리티가 부여되지 않는 현상이 존재함.
	
	if (!PlayerSkillID.IsNone())
	{
		CallRPC_SetSkillID(PlayerSkillID);
	}
	else
	{
		NET_DEBUG_LOG(TEXT("Skip SkillID RPC. PlayerSkillID=None"));
	}

	if (HasAuthority())
	{
		AFHPlayerBase* Pawn = GetPawn<AFHPlayerBase>();
		if (Pawn && !PlayerSkillID.IsNone())
		{
			NET_DEBUG_LOG(TEXT("Server re-applying skill to Pawn on pawn set. SkillKey: %s"), *PlayerSkillID.ToString());
			Pawn->ActivateSkill(PlayerSkillID);
		}
	}
	

	// 각 player state가 OnPawnSet Delegate를 받으면 이거 안해도 되나???
	//int32 NumPlayers = UGameplayStatics::GetNumPlayerStates(GetWorld());
	//for (int32 index = 1; index < NumPlayers; index++)
	//{
	//	AFHPlayerStateBase* PlayerState = Cast<AFHPlayerStateBase>(UGameplayStatics::GetPlayerState(GetWorld(), index));
	//	if (!PlayerState)
	//	{
	//		continue;
	//	}
	//	PlayerState->BP_SetPawnMesh(PlayerState->PawnKey);
	//}
}

void AFHPlayerStateBase::AddStatPoints_Implementation(int32 Amount)
{
	StatPoints += AddStatPointsAmountPerLevel * Amount;
	OnRep_StatPoints();
}

void AFHPlayerStateBase::SetPawnMesh(FName ActorKey)
{
	PawnKey = ActorKey;
	PlayerSaveData.PawnKey = PawnKey;
	OnRep_PawnKey();
	NetMulticast_SetPawnMesh(ActorKey);
	SetFirstEnterOff();
}

void AFHPlayerStateBase::NetMulticast_SetPawnMesh_Implementation(FName ActorKey)
{
	NET_DEBUG_LOG(TEXT(""));

	AFHPlayerController* Controller = Cast<AFHPlayerController>(GetPlayerController());
	if (!Controller)
	{
		return;
	}
	if (Controller->IsLocalController() && Controller->StatusHudInstance)
	{
		Controller->StatusHudInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

// Server Only
void AFHPlayerStateBase::SetSkillID(FName SkillID)
{
	// 이 함수가 호출될 때 Pawn 무조건 존재할 듯 이제.
	NET_DEBUG_LOG(TEXT("Start Set Skill. SkillKey: %s"), *SkillID.ToString());
	PlayerSkillID = SkillID;
	OnRep_PlayerSkillID();
	//GetWorld()->GetTimerManager().SetTimerForNextTick(
	//	[&]() {
	//		if (GetPawn())
	//		{
	//			Cast<AFHPlayerBase>(GetPawn())->ActivateSkill(PlayerSkillID);
	//		}
	//	});
}

void AFHPlayerStateBase::CallRPC_SetSkillID(FName SkillID)
{
	AFHPlayerController* Controller = Cast<AFHPlayerController>(GetPlayerController());
	if (!Controller)
	{
		return;
	}
	

	UFHActorRPCComponent* RPCComponent = Controller->GetComponentByClass<UFHActorRPCComponent>();

	if (!RPCComponent)
	{
		return;
	}

	RPCComponent->SetSkill(SkillID);
}

void AFHPlayerStateBase::Client_OnSkillFeedback_Implementation(bool bSuccess)
{
	OnSkillUIFeedback.Broadcast(bSuccess);
}

void AFHPlayerStateBase::Client_OnSkillCooldown_Implementation()
{
	OnSkillUIDuringCooldown.Broadcast();
}

void AFHPlayerStateBase::Client_OnBlockSkill_Implementation()
{
	OnBlockSkill.Broadcast();
}

void AFHPlayerStateBase::OnRep_PlayerSkillID()
{
	if (HasAuthority())
	{
		AFHPlayerBase* Pawn = GetPawn<AFHPlayerBase>();
		if (Pawn && !PlayerSkillID.IsNone())
		{
			NET_DEBUG_LOG(TEXT("Server: Activate skill on Pawn. SkillKey: %s"), *PlayerSkillID.ToString());
			Pawn->ActivateSkill(PlayerSkillID);
		}
	}

	if (!PlayerSkillID.IsNone())
	{
		UpdateSkillWidget();
	}

	OnSkillSettedDelegate.ExecuteIfBound(PlayerSkillID);
}

void AFHPlayerStateBase::OnRep_PawnKey()
{
	NET_DEBUG_LOG(TEXT("Try Set Pawn Mesh. PawnKey: %s"), *PawnKey.ToString());

	AFHPlayerBase* PlayerBase = GetPawn<AFHPlayerBase>();
	if (PlayerBase)
	{
		PlayerBase->ChangeMetahumanWithSelectInfo(PawnKey);
	}
}

void AFHPlayerStateBase::OnRep_SuitColor()
{
	AFHPlayerBase* Player = Cast<AFHPlayerBase>(GetPawn());
	if (Player != nullptr)
	{
		Player->ChangeSuitColor(SuitColor, SuitColorMultiplier);
	}
}

void AFHPlayerStateBase::OnRep_SuitColorMultiplier()
{
	AFHPlayerBase* Player = Cast<AFHPlayerBase>(GetPawn());
	if (Player != nullptr)
	{
		Player->ChangeSuitColor(SuitColor, SuitColorMultiplier);
	}
}

void AFHPlayerStateBase::SetSuitColor_Implementation(FLinearColor Color, float ColorMultiplier)
{
	SuitColor = Color;
	SuitColorMultiplier = ColorMultiplier;

	OnRep_SuitColor();
}

void AFHPlayerStateBase::UpdateSkillWidget()
{
	AFHPlayerController* PlayerController = Cast<AFHPlayerController>(GetPlayerController());
	if (PlayerController)
	{
		if (PlayerController->IsLocalController())
		{
			if (PlayerController->StatusHudInstance)
			{
				NET_DEBUG_LOG(TEXT("Start Set Skill Hud. SkillKey: %s"), *PlayerSkillID.ToString());
				PlayerController->StatusHudInstance->SkillChanged(PlayerSkillID);
			}
			else
			{
				NET_DEBUG_LOG(TEXT("No Status Hud Instance."));

				GetWorldTimerManager().SetTimer(RetryHandle,
					FTimerDelegate::CreateLambda([this]()
				{
					UpdateSkillWidget();
				}),
					0.2f, false
				);
			}
		}
	}
	else
	{
		NET_DEBUG_LOG(TEXT("No PlayerController."));
	}
}

//void AFHPlayerStateBase::GrantSkill()
//{
//	// Pawn이 바뀌는 등의 여러 상황에서 스킬이 작동 안되는 경우 서버에서 다시 스킬 부여
//	if (HasAuthority())
//	{
//		AFHPlayerBase* Pawn = GetPawn<AFHPlayerBase>();
//		if (Pawn && !PlayerSkillID.IsNone())
//		{
//			Pawn->ActivateSkill(PlayerSkillID);
//		}
//	}
//}

void AFHPlayerStateBase::StartFirstEnterProcess_Implementation()
{
	//if (!IsFirstEnter) // Has SaveData
	//{
	//	return;
	//}

	//IsFirstEnter = true;	
}

void AFHPlayerStateBase::LevelUp(int32 amount)
{
	Level += amount;
	StatPoints += AddStatPointsAmountPerLevel * amount;

	FH_MARK_PROPERTY_DIRTY(StatPoints);
	FH_MARK_PROPERTY_DIRTY(Level);
	OnRep_StatPoints();
	OnRep_Level();
}

void AFHPlayerStateBase::UpdateRequiredExp(int32 NewLevel)
{
	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	if (ItemSettings)
	{
		UDataTable* LevelUpExpTable = Cast<UDataTable>(ItemSettings->LevelUpExpTable.LoadSynchronous());
		if (LevelUpExpTable)
		{
			FFHLevelUpExpRow* ExpRow = LevelUpExpTable->FindRow<FFHLevelUpExpRow>(FName(*FString::Printf(TEXT("%d"), NewLevel + 1)), TEXT("Level Up Exp"));
			if (ExpRow)
			{
				RequiredExp = ExpRow->RequiredExp;
				FH_MARK_PROPERTY_DIRTY(RequiredExp);
				OnRep_RequiredExp();
			}
		}
	}
}

void AFHPlayerStateBase::UpgradeRank()
{
	if (LicenseRank == ELicenseRank::A)
	{
		return;
	}
	LicenseRank = (ELicenseRank)(((uint8)LicenseRank) + 1);
}

void AFHPlayerStateBase::ResetRank()
{
	LicenseRank = ELicenseRank::F;
}

void AFHPlayerStateBase::UseStatPointsToIncreaseStat_Implementation(EPrimaryAttribute stat, int32 amount)
{
	if (amount <= 0)
	{
		return;
	}

	if (StatPoints < amount)
	{
		PRINT_LOG(TEXT("Not enough minerals"));
		return;
	}

	bool Success = false;
	switch (stat)
	{
	case EPrimaryAttribute::Vitality:
	{
		Success = ApplyUpdateStatusEffect(IncreaseVitalityEffect, GET_GAMEPLAY_TAG_PLAYER_STATUS_VITALITY, amount);
		break;
	}
	case EPrimaryAttribute::Strength:
	{
		Success = ApplyUpdateStatusEffect(IncreaseStrengthEffect, GET_GAMEPLAY_TAG_PLAYER_STATUS_STRENGTH, amount);
		break;
	}
	case EPrimaryAttribute::Agility:
	{
		Success = ApplyUpdateStatusEffect(IncreaseAgilityEffect, GET_GAMEPLAY_TAG_PLAYER_STATUS_AGILITY, amount);
		break;
	}
	case EPrimaryAttribute::Endurance:
	{
		Success = ApplyUpdateStatusEffect(IncreaseEnduranceEffect, GET_GAMEPLAY_TAG_PLAYER_STATUS_ENDURANCE, amount);
		break;
	}
	case EPrimaryAttribute::Willpower:
	{
		Success = ApplyUpdateStatusEffect(IncreaseWillpowerEffect, GET_GAMEPLAY_TAG_PLAYER_STATUS_WILLPOWER, amount);
		break;
	}
	}
	if (Success)
	{
		StatPoints -= amount;
		FH_MARK_PROPERTY_DIRTY(StatPoints);
		OnRep_StatPoints();
	}
}

bool AFHPlayerStateBase::ApplyUpdateStatusEffect(TSubclassOf<UGameplayEffect> effect, FGameplayTag tag, float amount)
{
	FGameplayEffectSpecHandle effectSpec = abilitySystem->MakeOutgoingSpec(effect, 1, abilitySystem->MakeEffectContext());

	effectSpec.Data->SetSetByCallerMagnitude(tag, amount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(*effectSpec.Data.Get());

	return true;
}

void AFHPlayerStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(ThisClass, LicenseRank, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, Level, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, Exp, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, RequiredExp, Params);
	DOREPLIFETIME_WITH_PARAMS(ThisClass, StatPoints, Params);
	
	DOREPLIFETIME_WITH_PARAMS(ThisClass, PlayerSaveData, Params);
	
	DOREPLIFETIME(AFHPlayerStateBase, bIsObserving);
	DOREPLIFETIME(AFHPlayerStateBase, PlayerSkillID);
	DOREPLIFETIME(AFHPlayerStateBase, PawnKey);
	DOREPLIFETIME(AFHPlayerStateBase, SuitColor);
	DOREPLIFETIME(AFHPlayerStateBase, SuitColorMultiplier);
}

bool AFHPlayerStateBase::TryLoad()
{
	bool Result = false;
	if (!bIsLoaded && HasAuthority())
	{
		if (GetSaveSlot().IsEmpty())
		{
			return false;
		}
		
		NET_DEBUG_LOG(TEXT(""));

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GET_GAMEPLAY_TAG_PLAYER_ABILITY_INITATTRIBUTES, {});

		const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(this);
		Result = USimpleSaveKitFunctionLibrary::IsExistSaveFile(GameID, this);
		if (Result)
		{
			UFHBlueprintFunctionLibrary::LoadObject(this);
			PlayerSkillID = PlayerSaveData.SkillID;
			UE_LOG(LogTemp, Log, TEXT("세이브 파일 로드 성공. Level: %d"), PlayerSaveData.Level);
		}
		else
		{
			PlayerSaveData.LicenseRank = LicenseRank;
			PlayerSaveData.Level = Level;
			PlayerSaveData.Exp = Exp;
			PlayerSaveData.RequiredExp = RequiredExp;
			PlayerSaveData.StatPoints = StatPoints;
			PlayerSaveData.Health = HealthAttributeSet->GetHealth();
			PlayerSaveData.Vitality = PlayerAttributeSet->GetVitalityBase();
			PlayerSaveData.Strength = PlayerAttributeSet->GetStrengthBase();
			PlayerSaveData.Endurance = PlayerAttributeSet->GetEnduranceBase();
			PlayerSaveData.Agility = PlayerAttributeSet->GetAgilityBase();
			PlayerSaveData.Willpower = PlayerAttributeSet->GetWillpowerBase();
			PlayerSaveData.SkillID = PlayerSkillID;
			PlayerSaveData.PawnKey = PawnKey;

			PlayerSaveData.SuitColor = SuitColor;
			PlayerSaveData.SuitColorMultiplier = SuitColorMultiplier;

			PlayerSaveData.IsFirstPlay = IsFirstEnter;
			UE_LOG(LogTemp, Log, TEXT("세이브 파일 없음. 초기값으로 시작. Level: %d"), PlayerSaveData.Level);
		}

		OnRepPlayerSaveData();

	}
	return Result;
}

void AFHPlayerStateBase::TrySave()
{
	if (/*!bIsSave*/ true)
	{
		if (PawnKey.IsNone() || PlayerSkillID.IsNone())
		{
			return;
		}

		PlayerSaveData.LicenseRank = LicenseRank;
		PlayerSaveData.Level = Level;
		PlayerSaveData.Exp = Exp;
		PlayerSaveData.RequiredExp = RequiredExp;
		PlayerSaveData.StatPoints = StatPoints;
		
		PlayerSaveData.Health = HealthAttributeSet->GetHealth();
		PlayerSaveData.Vitality = PlayerAttributeSet->GetVitalityBase();
		PlayerSaveData.Strength = PlayerAttributeSet->GetStrengthBase();
		PlayerSaveData.Endurance = PlayerAttributeSet->GetEnduranceBase();
		PlayerSaveData.Agility = PlayerAttributeSet->GetAgilityBase();
		PlayerSaveData.Willpower = PlayerAttributeSet->GetWillpowerBase();
		PlayerSaveData.SkillID = PlayerSkillID;
		PlayerSaveData.IsFirstPlay = IsFirstEnter;
		PlayerSaveData.PawnKey = PawnKey;
		PlayerSaveData.SuitColor = SuitColor;
		PlayerSaveData.SuitColorMultiplier = SuitColorMultiplier;
		
		UFHBlueprintFunctionLibrary::SaveObject(this);
	}
	bIsSave = true;
}

void AFHPlayerStateBase::OnRepPlayerSaveData()
{
	UE_LOG(LogTemp, Log, TEXT("[OnRepPlayerSaveData] LicenseRank=%d Level=%d Health=%f SkillID=%s"),
		(int32)PlayerSaveData.LicenseRank, PlayerSaveData.Level, PlayerSaveData.Health, *PlayerSaveData.SkillID.ToString());

	NET_DEBUG_LOG(TEXT(""));

	LicenseRank = PlayerSaveData.LicenseRank;
	Level = PlayerSaveData.Level;
	Exp = PlayerSaveData.Exp;
	RequiredExp = PlayerSaveData.RequiredExp;
	StatPoints = PlayerSaveData.StatPoints;

	// 상태창에서 받기
	PlayerAttributeSet->SetVitality(PlayerSaveData.Vitality);
	PlayerAttributeSet->SetStrength(PlayerSaveData.Strength);
	PlayerAttributeSet->SetEndurance(PlayerSaveData.Endurance);
	PlayerAttributeSet->SetAgility(PlayerSaveData.Agility);
	PlayerAttributeSet->SetWillpower(PlayerSaveData.Willpower);
	HealthAttributeSet->SetMaxHealth(PlayerSaveData.Health);
	HealthAttributeSet->SetHealth(PlayerSaveData.Health);

	if (PlayerSkillID.IsNone())
	{
		PlayerSkillID = PlayerSaveData.SkillID;
	}
	OnRep_PlayerSkillID();
	UpdateSkillWidget();
	//CallRPC_SetSkillID(PlayerSaveData.SkillID);

	IsFirstEnter = PlayerSaveData.IsFirstPlay;
	PawnKey = PlayerSaveData.PawnKey;

	SuitColor = PlayerSaveData.SuitColor;
	SuitColorMultiplier = PlayerSaveData.SuitColorMultiplier;
	//OnRep_SuitColor();
	//OnRep_SuitColorMultiplier();
	OnRep_PawnKey();

	OnRep_LicenseRank();
	OnRep_Level();
	OnRep_Exp();
	OnRep_RequiredExp();
	OnRep_StatPoints();

	bIsLoaded = true;
	SetCurrentBackpackActorCache(CurrentBackpackActorCache);
	OnLoadEndDelegate.Broadcast(PlayerSaveData);
}
