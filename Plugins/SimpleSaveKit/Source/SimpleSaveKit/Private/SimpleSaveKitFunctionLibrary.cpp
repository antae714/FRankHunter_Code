// Copyright F Rank Hunter. All Rights Reserved.


#include "SimpleSaveKitFunctionLibrary.h"
#include "GameSavable.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "MultiSaveGame.h"

void USimpleSaveKitFunctionLibrary::SaveGameObjects(const UObject* WorldContextObject, const FString& GameID, const FString& SlotName, const TArray<UObject*>& ObjectArray, bool bIsSpawn)
{
	UArchiveSaveGame* ArchiveSaveGame = Cast<UArchiveSaveGame>(UGameplayStatics::CreateSaveGameObject(UArchiveSaveGame::StaticClass()));

	ArchiveSaveGame->SaveObjectArray = ObjectArray;
	ArchiveSaveGame->bIsSpawn = bIsSpawn;
	ArchiveSaveGame->CustomSave(WorldContextObject);
	;
	FString SlotNameReal = GetGameID(GameID) / SlotName;
	UGameplayStatics::SaveGameToSlot(ArchiveSaveGame, SlotNameReal, 0);
}

void USimpleSaveKitFunctionLibrary::LoadGameObjects(const UObject* WorldContextObject, const FString& GameID, const FString& SlotName, const TArray<UObject*>& ObjectArray, bool bIsSpawn)
{
	FString SlotNameReal = GetGameID(GameID) / SlotName;

	UArchiveSaveGame* ArchiveSaveGame = Cast<UArchiveSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotNameReal, 0));
	if (ArchiveSaveGame)
	{
		ArchiveSaveGame->SaveObjectArray = ObjectArray;
		ArchiveSaveGame->bIsSpawn = bIsSpawn;
		ArchiveSaveGame->CustomLoad(WorldContextObject);
	}
}


void USimpleSaveKitFunctionLibrary::SaveGameFromObject(const FString& GameID, UObject* Object, bool bIsSpawn)
{
	FString SlotName = GetSlotName(GameID, Object);
	UArchiveSaveGame* ArchiveSaveGame = Cast<UArchiveSaveGame>(UGameplayStatics::CreateSaveGameObject(UArchiveSaveGame::StaticClass()));
	if (ArchiveSaveGame)
	{
		ArchiveSaveGame->SaveObjectArray.Add(Object);
		ArchiveSaveGame->bIsSpawn = bIsSpawn;
		ArchiveSaveGame->CustomSave(Object);
		UGameplayStatics::SaveGameToSlot(ArchiveSaveGame, SlotName, 0);
	}
}

USaveGame* USimpleSaveKitFunctionLibrary::GetSaveGameFromObject(const FString& GameID, UObject* Object, bool bIsSpawn)
{
	FString SlotName = GetSlotName(GameID, Object);
	UArchiveSaveGame* ArchiveSaveGame = Cast<UArchiveSaveGame>(UGameplayStatics::CreateSaveGameObject(UArchiveSaveGame::StaticClass()));
	if (ArchiveSaveGame)
	{
		ArchiveSaveGame->SaveObjectArray.Add(Object);
		ArchiveSaveGame->bIsSpawn = bIsSpawn;
		ArchiveSaveGame->CustomSave(Object);

	}
	return ArchiveSaveGame;
}

void USimpleSaveKitFunctionLibrary::LoadGameFromObject(const FString& GameID, UObject* Object, bool bIsSpawn)
{
	FString SlotName = GetSlotName(GameID, Object);
	LoadGameFromObjectToSlot(SlotName, Object, bIsSpawn);
}

void USimpleSaveKitFunctionLibrary::LoadGameFromObjectToSlot(const FString& SlotName, UObject* Object, bool bIsSpawn)
{
	UArchiveSaveGame* ArchiveSaveGame = SlotName.IsEmpty() ? nullptr : Cast<UArchiveSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (ArchiveSaveGame)
	{
		ArchiveSaveGame->SaveObjectArray.Add(Object);
		ArchiveSaveGame->bIsSpawn = bIsSpawn;
		ArchiveSaveGame->CustomLoad(Object);
	}
}

bool USimpleSaveKitFunctionLibrary::IsExistSaveFile(const FString& GameID, UObject* object)
{
	FString SlotName = GetSlotName(GameID, object);
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

void USimpleSaveKitFunctionLibrary::SaveGameData(UObject* WorldContextObject, const FString& GameID)
{
	FString SlotName = GetSlotName(GameID, WorldContextObject);
	UArchiveSaveGame* ArchiveSaveGame = Cast<UArchiveSaveGame>(UGameplayStatics::CreateSaveGameObject(UArchiveSaveGame::StaticClass()));
	if (ArchiveSaveGame)
	{
		ArchiveSaveGame->CustomSaveData(WorldContextObject);
		UGameplayStatics::SaveGameToSlot(ArchiveSaveGame, SlotName, 0);
	}
}

void USimpleSaveKitFunctionLibrary::LoadGameData(UObject* WorldContextObject, const FString& GameID)
{
	FString SlotName = GetSlotName(GameID, WorldContextObject);

	UArchiveSaveGame* ArchiveSaveGame = Cast<UArchiveSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (ArchiveSaveGame)
	{
		ArchiveSaveGame->CustomLoadData(WorldContextObject);
	}
}


void USimpleSaveKitFunctionLibrary::SerializeActor(FArchive& Ar, UObject* object)
{
	if (!IsValid(object))
	{
		return;
	}
	Ar << FSavableWrapper(object);


	if (AActor* ItemActor = Cast<AActor>(object); ItemActor)
	{
		FTransform ActorTransform = ItemActor->GetActorTransform();
		Ar << ActorTransform;
		ItemActor->SetActorTransform(ActorTransform);

		TArray<FString> SavableComponentSloatArray;
		TMap<FString, UObject*> SavableComponentMap;
		// 나중에 서브오브젝트로

		ForEachObjectWithOuter(object,
							   [&Ar, &SavableComponentSloatArray, &SavableComponentMap](UObject* Sub)
							   {
								   if (IGameSavable* Savable = Cast<IGameSavable>(Sub))
								   {
									   FString SlotName = Savable->GetSaveSlot();

									   if (Ar.IsSaving())
									   {
										   SavableComponentSloatArray.Add(SlotName);
									   }
									   SavableComponentMap.Add(SlotName, Sub);
								   }
							   },
							   true
		);

		Ar << SavableComponentSloatArray;
		for (auto& SlotName : SavableComponentSloatArray)
		{
			UObject** FindObject = SavableComponentMap.Find(SlotName);
			if (FindObject)
			{
				Ar << FSavableWrapper(*FindObject);
			}
		}
	}

}

void USimpleSaveKitFunctionLibrary::GetSavableActorArray(const UObject* WorldContextObject, TArray<UObject*>& OutActorArray)
{
	//UGameplayStatics::GetAllActorsWithInterface 
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor->GetClass()->ImplementsInterface(UGameSavable::StaticClass()))
			{
				OutActorArray.Add(Actor);
			}
		}
	}
}

void USimpleSaveKitFunctionLibrary::GetSavableActorComponentArray(const UObject* WorldContextObject, TArray<UObject*>& ActorComponentArray)
{
	for (TObjectIterator<UActorComponent> It; It; ++It)
	{
		UActorComponent* Comp = *It;

		bool bIsWorldValid = !!Comp->GetWorld();
		bool bIsImplemented = Comp->GetClass()->ImplementsInterface(UGameSavable::StaticClass());
		if (bIsWorldValid && bIsImplemented)
		{
			ActorComponentArray.Add(Comp);
		}
	}
}

FString USimpleSaveKitFunctionLibrary::GetSlotName(const FString& GameName, UObject* object)
{
	FString GameID = GetGameID(GameName);

	if (object && object->GetClass()->ImplementsInterface(UGameSavable::StaticClass()))
	{
		IGameSavable* Savable = Cast<IGameSavable>(object);
		FString SaveName = Savable->GetSaveSlot();
		if (SaveName.IsEmpty())
		{
			return SaveName;
		}
		else if (Savable->IsGlobal())
		{
			return  GameID / SaveName;
		}
		else
		{
			return GameID / object->GetWorld()->GetMapName() / SaveName;
		}
	}
	return TEXT("");
}

FString USimpleSaveKitFunctionLibrary::GetGameID(const FString& GameName)
{
	FTCHARToUTF8 UTF8Converter(*GameName);
	const uint8* Bytes = reinterpret_cast<const uint8*>(UTF8Converter.Get());
	int32 Size = UTF8Converter.Length();

	return BytesToHex(Bytes, Size);
}
