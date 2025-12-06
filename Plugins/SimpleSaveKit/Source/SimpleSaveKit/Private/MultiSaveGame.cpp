


#include "MultiSaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "GameSavable.h"
#include "SimpleSaveKitFunctionLibrary.h"

const FGuid FMultiSaveGameCustomVersion::GUID(0x8533F245, 0x47734CAD, 0xBD87663D, 0xDDF7907D);
FCustomVersionRegistration GRegisterMultiSaveGameCustomVersion(FMultiSaveGameCustomVersion::GUID, FMultiSaveGameCustomVersion::LatestVersion, TEXT("MultiSaveGameVer"));

FArchive& operator<<(FArchive& Ar, FSavableWrapper Wrapper)
{
	UObject* Object = Wrapper.Object;
	if (Object)
	{
		if (Object->GetClass()->ImplementsInterface(UGameSavable::StaticClass()))
		{
			if (IGameSavable* Savable = Cast<IGameSavable>(Object))
			{
				Savable->SerializeData(Ar);
			}
		}
	}


	return Ar;
}


void UArchiveSaveGame::CustomSave(const UObject* WorldContextObject)
{
	SerializedData.Reset();
	Algo::Transform(SaveObjectArray, SaveClassArray,
					[](const UObject* SaveObject)
					{
						return SaveObject ? SaveObject->GetClass() : nullptr;
					});

	FMemoryWriter Writer(SerializedData, true);
	FObjectAndNameAsStringProxyArchive Archive(Writer, false);
	Archive.ArIsSaveGame = true;
	SerializeCustom(WorldContextObject, Archive);
}

void UArchiveSaveGame::CustomLoad(const UObject* WorldContextObject)
{
	FMemoryReader MemoryReader(SerializedData, true);
	FObjectAndNameAsStringProxyArchive Archive(MemoryReader, false);
	Archive.ArIsSaveGame = false;
	SerializeCustom(WorldContextObject, Archive);
}

void UArchiveSaveGame::CustomSaveData(UObject* WorldContextObject)
{
	SerializedData.Reset();
	FMemoryWriter Writer(SerializedData, true);
	FObjectAndNameAsStringProxyArchive Archive(Writer, false);
	Archive.ArIsSaveGame = true;
	Archive << FSavableWrapper(WorldContextObject);
}

void UArchiveSaveGame::CustomLoadData(UObject* WorldContextObject)
{
	FMemoryReader MemoryReader(SerializedData, true);
	FObjectAndNameAsStringProxyArchive Archive(MemoryReader, false);
	Archive.ArIsSaveGame = false;
	Archive << FSavableWrapper(WorldContextObject);
}

void UArchiveSaveGame::SerializeCustom(const UObject* WorldContextObject, FArchive& Ar)
{
	Ar.UsingCustomVersion(FMultiSaveGameCustomVersion::GUID);


	if (bIsSpawn && Ar.IsLoading())
	{
		for (size_t i = 0; i < SaveClassArray.Num(); i++)
		{
			if (!IsValid(SaveClassArray[i]))
			{
				continue;
			}

			AActor* LoadActor = nullptr;
			if (SaveClassArray[i]->IsChildOf(AActor::StaticClass()))
			{
				LoadActor = WorldContextObject->GetWorld()->SpawnActor(SaveClassArray[i]);
			}
			SaveObjectArray.Add(LoadActor);
		}
	}

	for (auto& ItemObejct : SaveObjectArray)
	{
		USimpleSaveKitFunctionLibrary::SerializeActor(Ar, ItemObejct);
	}
}
