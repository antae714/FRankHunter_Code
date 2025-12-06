// Copyright F Rank Hunter. All Rights Reserved.


#include "Sound/FHSoundSystem.h"
#include "Core/FHFRankHunterSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Core/FHSoundManagerActor.h"

void UFHSoundSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const UFHFRankHunterSettings* settings = GetDefault<UFHFRankHunterSettings>();
    UDataTable* SoundTable = settings->SoundDataTable.LoadSynchronous();

    ensureMsgf(SoundTable, TEXT("There is no sound table in this game."));

    TArray<FFHSoundDataTableRow*> rows;
    FString ContextString;
    SoundTable->GetAllRows(ContextString, rows);

    for (FFHSoundDataTableRow* row : rows)
    {
        if (FName name = row->SoundName; name.IsNone() == false &&  NameMap.Contains(name) == false)
        {
            NameMap.Add({ name, *row });
        }
        if (FGameplayTag tag = row->SoundTag; tag.IsValid() == true && TagMap.Contains(tag) == false)
        {
            TagMap.Add({ tag, *row });
        }
    }

    UE_LOG(LogTemp, Log, TEXT("FHSoundSystem: Register all sound data successfully."));
}

FFHSoundDataTableRow UFHSoundSystem::GetSoundDataByTag(FGameplayTag SoundTag)
{
    if (TagMap.Contains(SoundTag))
    {
        return TagMap[SoundTag];
    }
    else
    {
        return FFHSoundDataTableRow{};
    }
}

FFHSoundDataTableRow UFHSoundSystem::GetSoundDataByName(FName SoundName)
{
    if (NameMap.Contains(SoundName))
    {
        return NameMap[SoundName];
    }
    else
    {
        return FFHSoundDataTableRow{};
    }
}

USoundBase* UFHSoundSystem::GetSoundAssetByTag(FGameplayTag SoundTag)
{
    if (TagMap.Contains(SoundTag))
    {
        return TagMap[SoundTag].SoundAsset.LoadSynchronous();
    }
    else
    {
        return nullptr;
    }
}

USoundBase* UFHSoundSystem::GetSoundAssetByName(FName SoundName)
{
    if (NameMap.Contains(SoundName))
    {
        return NameMap[SoundName].SoundAsset.LoadSynchronous();
    }
    else
    {
        return nullptr;
    }
}

USoundBase* UFHSoundSystem::GetSoundAssetFromSoundData(const FFHSoundDataTableRow& SoundData)
{
    return SoundData.SoundAsset.LoadSynchronous();
}

AFHSoundManagerActor* UFHSoundSystem::GetSoundManagerActor()
{
    UManagerActorRegistrySubsystem* Registry = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
    return Registry->GetManagerActor<AFHSoundManagerActor>(TEXT("SoundManager"));
}

//void UFHSoundSystem::PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, float VolumeMult /*= 1.0f*/, bool Multicast)
//{
//    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
//    if (GameInstance == nullptr)
//    {
//        return;
//    }
//
//    UFHSoundSystem* SoundSystem = GameInstance->GetSubsystem<UFHSoundSystem>();
//
//    USoundBase* Sound = SoundSystem->GetSoundAssetByTag(SoundTag);
//    UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Sound, Location, VolumeMult);
//}
