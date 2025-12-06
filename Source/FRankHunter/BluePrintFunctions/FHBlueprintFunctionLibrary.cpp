// Copyright F Rank Hunter.. All Rights Reserved.


#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Kismet\GameplayStatics.h"
#include "Item/FHInventoryComponent.h"
#include "Interfaces\SiInventorySystemInterface.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "Core/FHFRankHunterSettings.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Core/FHPropManager.h"
#include "Core/FHUIManager.h"
#include "Core/FHSoundManagerActor.h"
#include "Core/FHVignetteEffectManager.h"
#include "FRankHunter.h"
#include "Creature/CreatureAIController.h"

#include "InputMappingContext.h"
#include "InputAction.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameDataSubsystem.h"
#include "SimpleSaveKitFunctionLibrary.h"
#include "EngineUtils.h"
#include "DataAsset/FHLevelAsset.h"
#include "Misc/SecureHash.h"
#include "Core/FHPlayerController.h"
#include "Core/FHGateGameModeBase.h"
#include "Core/FHGateGameStateBase.h"
#include "GenericTeamAgentInterface.h"
#include "Item/FHItemDropPointComponent.h"
#include "Item/Actors/FHWorldItemActor.h"
#include "DataAsset/FHSelectInfoIconDataAsset.h"
#include "DataAsset/FHGateThemaAsset.h"

#include "Interfaces/VoiceInterface.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Sound/SoundWaveProcedural.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/SCS_Node.h"


#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/evp.h"
THIRD_PARTY_INCLUDES_END
#undef UI


DECLARE_LOG_CATEGORY_CLASS(FHBlueprintFunctionLibrary, Log, All);


FSpawnItemParam::FSpawnItemParam() :
    ItemActorClass(AFHWorldItemActor::StaticClass()),
    index(-1),
	SpawnLocation(FVector::ZeroVector)
{
}



UFHInventoryComponent* UFHBlueprintFunctionLibrary::GetInventoryComponent(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}
	ISiInventorySystemInterface* InventorySystemInterface = Cast<ISiInventorySystemInterface>(Actor);
	if (!InventorySystemInterface)
	{
		return Actor->GetComponentByClass<UFHInventoryComponent>();
	}
	return Cast<UFHInventoryComponent>(InventorySystemInterface->GetInventoryComponent());
}

void UFHBlueprintFunctionLibrary::SpawnItemToActor(AActor* AvatarActor, FSpawnItemParam Param)
{
    APawn* Pawn = Cast<APawn>(AvatarActor);
    if (!AvatarActor)
    {
        return;
    }

    FActorSpawnParameters SpawnParams{};
    SpawnParams.Instigator = Pawn;
    SpawnParams.Owner = AvatarActor;

    FRotator SpawnRotation = FRotator::ZeroRotator;
    if (Pawn)
    {
        SpawnRotation = Pawn->GetControlRotation();
    }


    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    AFHWorldItemActor* SpawnedActor = AvatarActor->GetWorld()->SpawnActor<AFHWorldItemActor>(
        Param.ItemActorClass,
        Param.SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
    if (!SpawnedActor)
    {
        return;
    }

    UFHInventoryComponent* Inventory = UFHBlueprintFunctionLibrary::GetInventoryComponent(AvatarActor);
    if (!ensure(Inventory))
    {
        return;
    }



    // ======== Play Sound =========
    AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(AvatarActor->GetWorld());
    if (SoundManager)
    {
        FName SoundName = Inventory->GetCurrentItem()->GetItemData().DropSoundID;
        SoundManager->PlaySoundAtLocationMulticastByName(SoundName, Param.SpawnLocation);
    }


    int32 CurrentItemIndex = Param.index == -1 ? Inventory->GetCurrentItemIndex() : Param.index;
    UFHInventoryComponent* ToInventory = SpawnedActor->GetInventoryComponent();
    Inventory->Server_GiveItemToInventory(CurrentItemIndex, 1, ToInventory);
}

FFHItemData UFHBlueprintFunctionLibrary::GetItemData(FName ItemID)
{
    const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
    UDataTable* ItemDataTable = ItemSettings->ItemDataTable.LoadSynchronous();
    FFHItemData* FindItem = ItemDataTable ? ItemDataTable->FindRow<FFHItemData>(ItemID, TEXT("GetItemData")) : nullptr;

    if (FindItem)
    {
        return *FindItem;
    }
    else
    {
        return FFHItemData();
    }
}

FFHItemData UFHBlueprintFunctionLibrary::GetItemDataFromClass(TSubclassOf<UFHItemBase> ItemClass)
{
    
    return GetItemData(ItemClass.GetDefaultObject()->GetItemData().ItemID);
}

FLinearColor UFHBlueprintFunctionLibrary::GetItemRarityColor(EItemRarity ItemRarity)
{
    const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
    check(ItemSettings);

    const FItemRarityColorArray& ItemRarityColorArray = ItemSettings->ItemRarityColors;
    FLinearColor ItemRarityColor = ItemRarityColorArray.GetColor(ItemRarity);
    return ItemRarityColor;
}

UFHItemClassDataAsset* UFHBlueprintFunctionLibrary::GetItemClassDataAsset()
{
	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
    check(ItemSettings);

    return CastChecked<UFHItemClassDataAsset>(ItemSettings->ItemClassDataAsset.LoadSynchronous());
}

int32 UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(const TArray<float>& Weights)
{
    if (Weights.Num() == 0)
    {
        return -1; // 유효하지 않은 인덱스
    }
    else if (Weights.Num() == 1)
    {
        if (Weights[0] <= 0.0f)
        {
            return -1;
        }
        return 0;
    }


    float TotalWeight = 0.0f;
    for (float Weight : Weights)
    {
        TotalWeight += Weight;
    }

    if (TotalWeight <= 0.0f)
    {
        return -1;
    }

    float RandomPoint = FMath::FRandRange(0.0f, TotalWeight);

    float AccumulatedWeight = 0.0f;
    for (int32 i = 0; i < Weights.Num(); ++i)
    {
        AccumulatedWeight += Weights[i];
        if (RandomPoint <= AccumulatedWeight)
        {
            return i;
        }
    }

    // 부동소수점 오차로 여기까지 올 수도 있음
    return Weights.Num() - 1;
}

//TArray<UFunction*> UFHBlueprintFunctionLibrary::GetFunctionAllByCategory(UClass* classPTR, const FString& Category)
//{
//    TArray<UFunction*> Functions;
//    if (!classPTR)
//    {
//        return Functions;
//    }
//    for (TFieldIterator<UFunction> FuncIt(classPTR); FuncIt; ++FuncIt)
//    {
//        UFunction* Function = *FuncIt;
//        if (Function && Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) && Function->GetMetaData(TEXT("Category")) == Category)
//        {
//            Functions.Add(Function);
//        }
//    }
//    
//	return Functions;
//}

TArray<FKey> UFHBlueprintFunctionLibrary::FindMatchingKeysInInputMappingContext(const UInputMappingContext* InputMappingContext, const UInputAction* InputAction)
{
    if (!InputMappingContext || !InputAction)
    {
        return {};
    }

    const TArray<FEnhancedActionKeyMapping>& mappings = InputMappingContext->GetMappings();
    TArray<FKey> matchingKeys;

    for (const FEnhancedActionKeyMapping& mapping : mappings)
    {
        if (mapping.Action == InputAction)
        {
            matchingKeys.Add(mapping.Key);
        }
    }

    return matchingKeys;
}

AFHPropManager* UFHBlueprintFunctionLibrary::GetPropManager(const UObject* WorldContextObject)
{
    if (WorldContextObject == nullptr) return nullptr;

    UManagerActorRegistrySubsystem* Subsystem = WorldContextObject->GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
    if (Subsystem)
    {
        AFHPropManager* PropManager = Subsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
        if (PropManager)
        {
            return PropManager;
        }
    }

    AActor* Thing = UGameplayStatics::GetActorOfClass(WorldContextObject, AFHPropManager::StaticClass());
    if (Thing)
    {
        return Cast<AFHPropManager>(Thing);
    }

    return nullptr;
}

AFHUIManager* UFHBlueprintFunctionLibrary::GetUIManager(const UObject* WorldContextObject)
{
    if (WorldContextObject == nullptr) return nullptr;

    UManagerActorRegistrySubsystem* Subsystem = WorldContextObject->GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
    if (Subsystem)
    {
        AFHUIManager* UIManager = Subsystem->GetManagerActor<AFHUIManager>(TEXT("UIManager"));
        if (UIManager)
        {
            return UIManager;
        }
    }

    AActor* Thing = UGameplayStatics::GetActorOfClass(WorldContextObject, AFHUIManager::StaticClass());
    if (Thing)
    {
        return Cast<AFHUIManager>(Thing);
    }

    return nullptr;
}

AFHSoundManagerActor* UFHBlueprintFunctionLibrary::GetSoundManager(const UObject* WorldContextObject)
{
    if (WorldContextObject == nullptr) return nullptr;

    UManagerActorRegistrySubsystem* Subsystem = WorldContextObject->GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
    if (Subsystem)
    {
        AFHSoundManagerActor* SoundManager = Subsystem->GetManagerActor<AFHSoundManagerActor>(TEXT("SoundManager"));
        if (SoundManager)
        {
            return SoundManager;
        }
    }

    AActor* Thing = UGameplayStatics::GetActorOfClass(WorldContextObject, AFHSoundManagerActor::StaticClass());
    if (Thing)
    {
        return Cast<AFHSoundManagerActor>(Thing);
    }

    return nullptr;
}

class AFHVignetteEffectManager* UFHBlueprintFunctionLibrary::GetVignetteEffectManager(const UObject* WorldContextObject)
{
    if (WorldContextObject == nullptr) return nullptr;

    UManagerActorRegistrySubsystem* Subsystem = WorldContextObject->GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
    if (Subsystem)
    {
        AFHVignetteEffectManager* VignetteManager = Subsystem->GetManagerActor<AFHVignetteEffectManager>(TEXT("VignetteManager"));
        if (VignetteManager)
        {
            return VignetteManager;
        }
    }

    AActor* Thing = UGameplayStatics::GetActorOfClass(WorldContextObject, AFHVignetteEffectManager::StaticClass());
    if (Thing)
    {
        return Cast<AFHVignetteEffectManager>(Thing);
    }

    return nullptr;
}

FString UFHBlueprintFunctionLibrary::GetMapName(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return TEXT("");
    }

    FString MapPackageName = WorldContextObject->GetWorld()->PersistentLevel->GetOutermost()->GetName();

#if WITH_EDITOR
    // PIE에서 중간에 삽입된 "UEDPIE_X_" 제거
    const FString PIEPattern = TEXT("UEDPIE_");

    if (MapPackageName.Find(PIEPattern, ESearchCase::IgnoreCase, ESearchDir::FromStart, /*StartPosition*/ 0) != INDEX_NONE)
    {
        // "UEDPIE_X_" 다음에 오는 부분 추출
        int32 PrefixStart = MapPackageName.Find(PIEPattern);
        int32 PrefixEnd = MapPackageName.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromStart, PrefixStart + PIEPattern.Len());
        if (PrefixStart != INDEX_NONE && PrefixEnd != INDEX_NONE)
        {
            // 실제 접두사: "UEDPIE_0_" 또는 "UEDPIE_12_" 등
            FString FullPrefix = MapPackageName.Mid(PrefixStart, PrefixEnd - PrefixStart + 1);
            MapPackageName = MapPackageName.Replace(*FullPrefix, TEXT(""));
        }
    }
#endif

    return MapPackageName;
}

bool UFHBlueprintFunctionLibrary::IsInState(AController* Controller, FName InStateName)
{
    return Controller->IsInState(InStateName);
}

FName UFHBlueprintFunctionLibrary::GetSpectatorName()
{
    return NAME_Spectating;
}

FName UFHBlueprintFunctionLibrary::GetPlayerName()
{
    return NAME_Playing;
}

bool UFHBlueprintFunctionLibrary::IsEnenmy(AActor* TeamA, AActor* TeamB)
{
    // 유효성 검사
    if (!TeamA || !TeamB)
    {
        return false;
    }

	IGenericTeamAgentInterface* AgentA = Cast<IGenericTeamAgentInterface>(TeamA);
	IGenericTeamAgentInterface* AgentB = Cast<IGenericTeamAgentInterface>(TeamB);
    if(!AgentA || !AgentB)
    {
        return false;
	}

    // 팀 ID 가져오기 (Blueprint/C++ 구현 모두 지원)
    const FGenericTeamId TeamIdA = AgentA->GetGenericTeamId();
    const FGenericTeamId TeamIdB = AgentB->GetGenericTeamId();

    // 두 팀 간 태도(Attitude) 계산
    const ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(TeamIdA, TeamIdB);

    // 적(Hostile)일 경우 true 반환
    return (Attitude == ETeamAttitude::Hostile);
}

void UFHBlueprintFunctionLibrary::SaveActors(const UObject* WorldContextObject, const FString& SlotName, const TArray<TSubclassOf<AActor>>& FilterActor)
{
    TArray<UObject*> FilteredActorArray;
    
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        for (FActorIterator It(World); It; ++It)
        {
            AActor* Actor = *It;
            for (auto& filterClass : FilterActor)
            {
                bool bIsFiltered = Actor->GetClass()->IsChildOf(filterClass);
                bool bExcludeFromSave = Actor->Tags.Contains(Name_ExcludeFromSave);
                if (bIsFiltered && !bExcludeFromSave)
                {
                    FilteredActorArray.Add(Actor);
                    break;
                }
            }
        }

        const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(WorldContextObject);

        USimpleSaveKitFunctionLibrary::SaveGameObjects(WorldContextObject, GameID, SlotName, FilteredActorArray, true);
    }
}

bool UFHBlueprintFunctionLibrary::IsGameOver(const UObject* WorldContextObject)
{
    AFH_GS_LobbyGameState* FH_GS_LobbyGameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(WorldContextObject->GetWorld()));
    return FH_GS_LobbyGameState ? FH_GS_LobbyGameState->IsGameOver() : false;
}

ACreatureAIController* UFHBlueprintFunctionLibrary::GetCreatureAIController(AActor* ControlledActor)
{
    APawn* AsPawn = Cast<APawn>(ControlledActor);
    if (AsPawn != nullptr)
    {
        return Cast<ACreatureAIController>(AsPawn->GetController());
    }
    return Cast<ACreatureAIController>(ControlledActor);
}

int32 UFHBlueprintFunctionLibrary::GetCurrentYear(int32 LoopCount, int32 offsetYear)
{
    return LoopCount / 2 / 12 + offsetYear;
}

int32 UFHBlueprintFunctionLibrary::GetCurrentMounth(int32 LoopCount)
{
    return LoopCount / 2 % 12 + 1;
}

FText UFHBlueprintFunctionLibrary::GetLocalizeText(const FTextKey& InKey)
{
    const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
    FName LocalizeName = ItemSettings->GetLocalizeStringTableID();
    FText text = FText::FromStringTable(LocalizeName, InKey);
    return text;
}

FString UFHBlueprintFunctionLibrary::GetGameID(const UObject* WorldContextObject)
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    UGameDataSubsystem* GameData = GameInstance ? GameInstance->GetSubsystem<UGameDataSubsystem>() : nullptr;
    return GameData ? GameData->GetGameID() : TEXT("0");
}

FString UFHBlueprintFunctionLibrary::GetGameName(const UObject* WorldContextObject)
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    UGameDataSubsystem* GameData = GameInstance ? GameInstance->GetSubsystem<UGameDataSubsystem>() : nullptr;
    return GameData ? GameData->GetGameName() : TEXT("0");
}

void UFHBlueprintFunctionLibrary::LoadActors(const UObject* WorldContextObject, const FString& SlotName)
{
    const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(WorldContextObject);
    USimpleSaveKitFunctionLibrary::LoadGameObjects(WorldContextObject, GameID, SlotName, {}, true);
}

void UFHBlueprintFunctionLibrary::SaveObject(UObject* Object)
{
    const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(Object);
    USimpleSaveKitFunctionLibrary::SaveGameFromObject(GameID, Object, false);
}

void UFHBlueprintFunctionLibrary::LoadObject(UObject* Object)
{
    const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(Object);
    USimpleSaveKitFunctionLibrary::LoadGameFromObject(GameID, Object, false);
}

void UFHBlueprintFunctionLibrary::RemoveGame(const FString& GameID)
{
    FString SaveDirectory = FPaths::ProjectSavedDir() / TEXT("SaveGames") / GameID;

    // Ensure the directory exists
    if (FPaths::DirectoryExists(SaveDirectory))
    {
        // Recursively delete the directory and its contents
        bool bSuccess = IFileManager::Get().DeleteDirectory(*SaveDirectory, false, true);

        if (!bSuccess)
        {
            // 강제 삭제 실패 시 내부 파일 삭제 후 디렉토리 제거
            IFileManager::Get().DeleteDirectory(*SaveDirectory, true, true);
        }
    }
}

void UFHBlueprintFunctionLibrary::RemoveSaveGame(const FString& GameID, const FString& SlotName)
{
    FString SavePath = FPaths::Combine(
        FPaths::ProjectSavedDir(),
        TEXT("SaveGames"),
        GameID,
        SlotName + TEXT(".sav")
    );


    // Ensure the directory exists
    if (FPaths::FileExists(SavePath))
    {
        // Recursively delete the directory and its contents
        bool bSuccess = IFileManager::Get().Delete(*SavePath, false, true);

        if (!bSuccess)
        {
            IFileManager::Get().Delete(*SavePath, true, true);
        }
    }
}

UFHLevelAsset* UFHBlueprintFunctionLibrary::GetLevelAsset()
{
    const UFHFRankHunterSettings* FHSettings = GetDefault<UFHFRankHunterSettings>();
    check(FHSettings);

    return CastChecked<UFHLevelAsset>(FHSettings->LevelAsset.LoadSynchronous());
}

void UFHBlueprintFunctionLibrary::SaveData(UObject* Object)
{
    const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(Object);
    USimpleSaveKitFunctionLibrary::SaveGameData(Object, GameID);
}

void UFHBlueprintFunctionLibrary::LoadData(UObject* Object)
{
    const FString& GameID = UFHBlueprintFunctionLibrary::GetGameName(Object);
    USimpleSaveKitFunctionLibrary::LoadGameData(Object, GameID);
}

void UFHBlueprintFunctionLibrary::EscapeGate(AController* Controller)
{
    if (!IsValid(Controller))
    {
        UE_LOG(FHBlueprintFunctionLibrary, Warning, TEXT("Controller is not Valid"));
        return;
    }

    AFHPlayerController* FHController = Cast<AFHPlayerController>(Controller);
    if (!IsValid(FHController))
    {
        UE_LOG(FHBlueprintFunctionLibrary, Warning, TEXT("Controller is not AFHPlayerController"));
        return;
    }

    AFHGateGameModeBase* GameMode = Controller->GetWorld()->GetAuthGameMode<AFHGateGameModeBase>();
    if (!IsValid(GameMode))
    {
        UE_LOG(FHBlueprintFunctionLibrary, Warning, TEXT("Current Game Is Not Gate"));
        return;
    }

    GameMode->PlayerEscape(FHController);
}

bool UFHBlueprintFunctionLibrary::IsGate(const UObject* WorldContextObject)
{
    return  !!WorldContextObject->GetWorld()->GetGameState<AFHGateGameStateBase>();

}

FString UFHBlueprintFunctionLibrary::GetEncrptData(FString Input)
{
    FTCHARToUTF8 UTF8String(*Input);
    const unsigned char* Data = (const unsigned char*)UTF8String.Get();
    size_t DataLength = UTF8String.Length();

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return FString();

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return FString();
    }

    EVP_DigestUpdate(ctx, Data, DataLength);

    unsigned char Hash[EVP_MAX_MD_SIZE];
    unsigned int HashLen = 0;
    EVP_DigestFinal_ex(ctx, Hash, &HashLen);

    EVP_MD_CTX_free(ctx);

    FString OutHash;
    for (unsigned int i = 0; i < HashLen; i++)
    {
        OutHash += FString::Printf(TEXT("%02x"), Hash[i]);
    }

    return OutHash;
}

FString UFHBlueprintFunctionLibrary::ToUnicodeString(const FString& InString)
{
    FTCHARToUTF8 Utf8(*InString);
    return FString(UTF8_TO_TCHAR(Utf8.Get()));
}

void UFHBlueprintFunctionLibrary::ClearVoicePackets(UObject* WorldContextObject)
{
    IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();
    if (!VoiceInterface.IsValid())
    {
        PRINT_LOG(TEXT("VoiceInterface Not Valid."));
        return;
    }

    VoiceInterface->ClearVoicePackets();
}

void UFHBlueprintFunctionLibrary::CopyAndSetSound(UObject* WorldContectObject, USoundBase* Base, UAudioComponent* TargetAudioComponent)
{
    USoundBase* NewProcedural = NewObject<USoundBase>(GetTransientPackage(), USoundBase::StaticClass());
    

    TargetAudioComponent->SetSound(NewProcedural);
}

UFHSelectInfoIconDataAsset* UFHBlueprintFunctionLibrary::GetSelectInfoDataAsset()
{
    const UFHFRankHunterSettings* GameSettings = GetDefault<UFHFRankHunterSettings>();
    check(GameSettings);

    return CastChecked<UFHSelectInfoIconDataAsset>(GameSettings->SelectInfoIconDataAsset.LoadSynchronous());
}

#include "GameplayEffectTypes.h" 

AActor* UFHBlueprintFunctionLibrary::GetInstigatorFromEffectSpec(const FGameplayEffectSpec& Spec)
{
    if (Spec.GetContext().IsValid())
    {
        return Spec.GetContext().GetInstigator();
    }

    return nullptr;
}

TMap<FName, UObject*> UFHBlueprintFunctionLibrary::GetConstructionScriptData(UClass* AssetClass)
{
    TMap<FName, UObject*> Result;

    UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(AssetClass);
    if (USimpleConstructionScript* SCS = BPClass->SimpleConstructionScript)
    {
        for (USCS_Node* Node : SCS->GetAllNodes())
        {
            if (Node && Node->ComponentTemplate)
            {
                Result.Add(Node->GetVariableName(), Node->ComponentTemplate);
            }
        }
    }
    return Result;
}

void UFHBlueprintFunctionLibrary::ReferenceNullPtr() 
{
	int* Ptr = nullptr;
	*Ptr = 0;
}

UFHGateThemaAsset* UFHBlueprintFunctionLibrary::GetGateThemaAsset()
{
    const UFHFRankHunterSettings* GameSettings = GetDefault<UFHFRankHunterSettings>();
    check(GameSettings);

    return CastChecked<UFHGateThemaAsset>(GameSettings->GateThemaAsset.LoadSynchronous());
}

