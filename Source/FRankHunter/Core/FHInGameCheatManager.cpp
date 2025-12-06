// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHInGameCheatManager.h"
#include "Core/FHPlayerStateBase.h"
#include "Core/FHFRankHunterSettings.h"
#include "Item/FHItemBase.h"
#include "Item/FHInventoryComponent.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "CommonUserWidget.h"
#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/FHGameplayTags.h"
#include "GAS\FHAttributeSet_Health.h"
#include "Core/FHGateGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Core/FHPlayerController.h"
#include "Component/ActorRPC/FHActorRPCComponent.h"
#include "OnlineSubsystem.h"



DEFINE_LOG_CATEGORY_STATIC(LogFHCheatManager, Log, All);

void UFHInGameCheatManager::InitCheatManager()
{
	Super::InitCheatManager();
	if (GetPlayerController()->GetPawn())
	{
		InitializeCheatWidget(nullptr, nullptr);
	}
	else
	{
		GetPlayerController()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::InitializeCheatWidget);
	}
}

void UFHInGameCheatManager::InitializeCheatWidget(APawn* OldPawn, APawn* NewPawn)
{
	if (GetPlayerController()->IsLocalController())
	{
		InGameCheatWidgetInstance = CreateWidget<UCommonUserWidget>(GetPlayerController(), InGameCheatWidgetClass);
		if (InGameCheatWidgetInstance)
		{
			InGameCheatWidgetInstance->AddToViewport(255);
		}
	}
	if (GetPlayerController()->OnPossessedPawnChanged.IsAlreadyBound(this, &ThisClass::InitializeCheatWidget))
	{
		GetPlayerController()->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::InitializeCheatWidget);
	}
}

void UFHInGameCheatManager::GetDamageSelf(float amount)
{
	Server_GetDamageSelf(amount);
}

void UFHInGameCheatManager::Server_GetDamageSelf_Implementation(float amount)
{
	APlayerController* PlayerController = GetPlayerController();
	if (!PlayerController)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerController->PlayerState);
	if (ASC)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(UFH_GE_SimpleDamage::StaticClass(), 1, ASC->MakeEffectContext());
		EffectSpecHandle.Data->SetSetByCallerMagnitude(GET_GAMEPLAY_TAG_PLAYER_STATUS_HEALTH, -amount);
		ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UFHInGameCheatManager::ApplyEffect(FName EffectName)
{
}

void UFHInGameCheatManager::PrintNetMode()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrintNetMode: World is nullptr"));
		return;
	}

	FString ModeString;

	switch (World->GetNetMode())
	{
	case NM_Standalone:
		ModeString = TEXT("Standalone (싱글플레이 모드)");
		break;

	case NM_ListenServer:
		ModeString = TEXT("ListenServer (리슨 서버)");
		break;

	case NM_DedicatedServer:
		ModeString = TEXT("DedicatedServer (전용 서버)");
		break;

	case NM_Client:
		ModeString = TEXT("Client (클라이언트)");
		break;

	default:
		ModeString = TEXT("Unknown NetMode");
		break;
	}

	// 로그 출력
	UE_LOG(LogTemp, Log, TEXT("[PrintNetMode] Current NetMode: %s"), *ModeString);

	// 화면 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
										 FString::Printf(TEXT("[NetMode] %s"), *ModeString));
	}
}

void UFHInGameCheatManager::PrintOSSMode()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	FString OSSName = TEXT("None (No Online Subsystem Found)");

	if (OSS)
	{
		OSSName = OSS->GetSubsystemName().ToString();
	}

	UE_LOG(LogTemp, Log, TEXT("[PrintOSSMode] Current Online Subsystem: %s"), *OSSName);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
										 FString::Printf(TEXT("[OSS] %s"), *OSSName));
	}

}

void UFHInGameCheatManager::GetExp(float amount)
{
	APlayerController* PlayerController = GetPlayerController();
	AFHPlayerStateBase* PlayerState = PlayerController ? PlayerController->GetPlayerState<AFHPlayerStateBase>() : nullptr;
	if (PlayerState)
	{
		PlayerState->AddExp(amount);
	}
}

void UFHInGameCheatManager::GetItem(FName ItemID, int32 count)
{
	FString FailureReason;
	bool bIsSucess = false;

	const UFHFRankHunterSettings* Settings = GetDefault<UFHFRankHunterSettings>();
	UDataTable* ItemTable = Settings->ItemDataTable.LoadSynchronous();
	UFHItemClassDataAsset* ItemDataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	TSubclassOf<UFHItemBase> FindedItem = nullptr;

	if (ItemTable && ItemDataAsset)
	{
		FFHItemData* ItemData = ItemTable->FindRow<FFHItemData>(ItemID, TEXT("Cheat"));
		if (ItemData)
		{
			FindedItem = ItemDataAsset->GetItemClass(ItemData->ItemID);
		}
		else
		{
			FailureReason = FString::Printf(TEXT("Item with ID %s not found in ItemDataTable."), *ItemID.ToString());
			bIsSucess = false;
		}
	}
	else
	{
		FailureReason = TEXT("ItemDataTable or ItemClassDataAsset is null.");
		bIsSucess = false;
	}

	if (FindedItem)
	{
		AController* PlayerController = GetPlayerController();
		APlayerState* PlayerState = PlayerController ? PlayerController->PlayerState : nullptr;

		if (!PlayerState)
		{
			FailureReason = TEXT("PlayerState is null.");
			bIsSucess = false;
		}
		if (!PlayerController)
		{
			FailureReason = TEXT("PlayerController is null.");
			bIsSucess = false;
		}

		UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(PlayerState);
		InventoryComponent->Server_AddItem(FindedItem, count, INDEX_NONE, true);
		bIsSucess = true;
	}
	else
	{
		FailureReason = FString::Printf(TEXT("Item with ID %s not found."), *ItemID.ToString());
		bIsSucess = false;
	}


	if (!bIsSucess)
	{
		UE_LOG(LogFHCheatManager, Warning, TEXT("GetItem failed: %s"), *FailureReason);
	}
}

void UFHInGameCheatManager::GateClear()
{
	AFHGateGameModeBase* FHGateGameModeBase = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (FHGateGameModeBase)
	{
		//FHGateGameModeBase->GetPlayerList().Empty();

		//FHGateGameModeBase->BackToLobby();
		FHGateGameModeBase->ForceClearGate();	
	}
}

void UFHInGameCheatManager::GetMoney(int32 Amount)
{
	AFH_GS_LobbyGameState* FH_GS_LobbyGameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (FH_GS_LobbyGameState)
	{
		FH_GS_LobbyGameState->AddMoney(Amount);
	}

}

void UFHInGameCheatManager::SetSkill(FName SkillID)
{
	AFHPlayerController* Controller = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
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

void UCheatManageUI::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = GetOwningPlayer();



	if (PlayerController->CheatManager)
	{
		UClass* CheatManagerClass = PlayerController->CheatManager->GetClass();
		for (TFieldIterator<UFunction> Iter(CheatManagerClass); Iter; ++Iter)
		{
			UFunction* Function = *Iter;
			FString FuncCategory;
#if WITH_METADATA
			FuncCategory = Function->GetMetaData(TEXT("Category"));
#endif // WITH_METADATA
			if (FuncCategory == TEXT("InGame Cheat Manager"))
			{
				UCheatItemUI* NewWidget = CreateWidget<UCheatItemUI>(this, ItemClass);
				NewWidget->Setting(Function);
				InGameCheatManager->AddChild(NewWidget);
			}
			else if(Function->HasAnyFunctionFlags(FUNC_Exec))
			{
				UCheatItemUI* NewWidget = CreateWidget<UCheatItemUI>(this, ItemClass);
				NewWidget->Setting(Function);
				CheatManager->AddChild(NewWidget);
			}
		}

	}
}

void UCheatItemUI::NativeConstruct()
{
	Super::NativeConstruct();

	CheatButton->OnClicked.AddDynamic(this, &ThisClass::Cheat);
}

void UCheatItemUI::NativeDestruct()
{
	Super::NativeDestruct();

	CheatButton->OnClicked.RemoveDynamic(this, &ThisClass::Cheat);
}

void UCheatItemUI::Setting(UFunction* function)
{
	Cmd = function->GetName();
	NumParam = function->NumParms;
	if (NumParam > 0)
	{
		FString ParamHint = TEXT("");
		for (TFieldIterator<FProperty> It(function); It; ++It)
		{
			FProperty* Prop = *It;

			// 파라미터용(PropertyFlags에 CPF_Parm)이면서, 리턴 파라미터는 제외(CPF_ReturnParm)
			if (Prop->HasAnyPropertyFlags(CPF_Parm) &&
				!Prop->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				// 1) 파라미터 이름
				FString ParamName = Prop->GetName();

				ParamHint += FString::Printf(TEXT("%s "), *ParamName) ;
			}
		}

		ParamEditBox->SetHintText(FText::FromString(ParamHint));
	}
	else
	{
		ParamEditBox->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UCheatItemUI::Cheat()
{
	FString Command = Cmd;
	Command += TEXT(" ");
	if (NumParam > 0)
	{
		Command += ParamEditBox->GetText().ToString();
	}
	APlayerController* PlayerController = GetOwningPlayer();
	UKismetSystemLibrary::ExecuteConsoleCommand(PlayerController, Command);
}

UFH_GE_SimpleDamage::UFH_GE_SimpleDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ModInfo{};
	ModInfo.Attribute = UFHAttributeSet_Health::GetHealthAttribute();
	ModInfo.ModifierOp = EGameplayModOp::Additive;
	FSetByCallerFloat SetByCallerValue{};
	SetByCallerValue.DataName = FName(TEXT("Simple Damage"));
	SetByCallerValue.DataTag = GET_GAMEPLAY_TAG_PLAYER_STATUS_HEALTH;
	ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerValue);
	Modifiers.Add(ModInfo);
}
