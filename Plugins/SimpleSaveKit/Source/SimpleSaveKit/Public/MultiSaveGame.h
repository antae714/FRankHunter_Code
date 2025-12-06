

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MultiSaveGame.generated.h"


// Custom serialization version for all packages Inventory
struct FMultiSaveGameCustomVersion
{
	enum Type
	{
		// Before any version changes were made in the plugin
		BeforeCustomVersionWasAdded = 0,
		FirstVersion = 1,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FMultiSaveGameCustomVersion() {}
};

struct FSavableWrapper
{
	UObject* Object;


};
extern FArchive& operator<<(FArchive& Ar, FSavableWrapper Wrapper);


UCLASS()
class SIMPLESAVEKIT_API UArchiveSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	void CustomSave(const UObject* WorldContextObject);
	void CustomLoad(const UObject* WorldContextObject);

	// Data Only
	void CustomSaveData(UObject* WorldContextObject);
	void CustomLoadData(UObject* WorldContextObject);
	// ----------

	UPROPERTY(Transient)
	TArray<UObject*> SaveObjectArray;

	/** 로딩한 클래스가 액터일시 스폰 */
	UPROPERTY()
	uint32 bIsSpawn : 1;

protected:
	void SerializeCustom(const UObject* WorldContextObject, FArchive& Ar);

private:
	UPROPERTY()
	TArray<UClass*> SaveClassArray;

	UPROPERTY()
	TArray<uint8> SerializedData;
};
