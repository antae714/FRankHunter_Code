// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Actors/ItemDropManagerActor.h"
#include "Core/FHFRankHunterSettings.h"
#include "Item/ItemTypes.h"
#include "Item/FHItemBase.h"
#include "Item/Actors/FHWorldItemActor.h"
#include "Item/Data/FHItemSubsystem.h"
#include "Item/FHInventoryComponent.h"
#include "EngineUtils.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "Core/FHGameInstance.h"





// Sets default values
AItemDropManagerActor::AItemDropManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;
}

void AItemDropManagerActor::SpawnManaStone(const FVector& SpawnPoint, TSubclassOf<AFHWorldItemActor> ActorClass)
{
	if (!HasAuthority())
	{
		return;
	}
	if (!ActorClass)
	{
		ActorClass = AFHWorldItemActor::StaticClass();
	} 

	TSubclassOf<UFHItemBase> ItemClass = GetSpawnManaStoneInfo();
	if (ItemClass)
	{
		FActorSpawnParameters SpawnParam{};
		SpawnParam.Owner = this;

		AFHWorldItemActor* FHWorldItemActor = GetWorld()->SpawnActor<AFHWorldItemActor>(ActorClass, SpawnPoint, FRotator::ZeroRotator, SpawnParam);
		FHWorldItemActor->GetInventoryComponent()->Server_AddItem(ItemClass, 1);
	}
}

TSubclassOf<UFHItemBase> AItemDropManagerActor::GetSpawnManaStoneInfo()
{
	UFHItemSubsystem* ItemSubsystem = GEngine->GetEngineSubsystem<UFHItemSubsystem>();

	int32 index = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(WeightArray);
	return ItemSubsystem->GetItemClass(ItemIDArray[index]);
}

// Called when the game starts or when spawned
void AItemDropManagerActor::BeginPlay()
{
	Super::BeginPlay();

	EGateRank CurrentGateGrade = TempGateRank;
	UFHGameInstance* GameInstance = Cast<UFHGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		CurrentGateGrade = GameInstance->GetGateRank();
	}
	if (CurrentGateGrade == EGateRank::Invalid)
	{
		CurrentGateGrade = TempGateRank;
	}
	if (CurrentGateGrade == EGateRank::Invalid)
	{
		CurrentGateGrade = EGateRank::E;
	}

	TObjectPtr<UDataTable> ManaStoneDropTable;
	const UFHFRankHunterSettings* ItemSettings = GetDefault<UFHFRankHunterSettings>();
	check(ItemSettings);
	if (ensure(!ItemSettings->ManaStoneDropTable.IsNull()))
	{
		ManaStoneDropTable = ItemSettings->ManaStoneDropTable.LoadSynchronous();
		check(ManaStoneDropTable->GetRowStruct() == FFHManaStoneDropTableRow::StaticStruct());
	}

	WeightArray.Empty();
	ItemIDArray.Empty();
	ManaStoneDropTable->ForeachRow<FFHManaStoneDropTableRow>(TEXT("Copy ManaStone Drop Table"),
															 [&](const FName& RowName, const FFHManaStoneDropTableRow& RowData)
															 {
																 ItemIDArray.Add(RowData.ItemID);
																 WeightArray.Add(RowData.GetWeight(CurrentGateGrade));
															 });
}


