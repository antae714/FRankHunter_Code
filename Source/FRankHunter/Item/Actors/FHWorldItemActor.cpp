// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Actors/FHWorldItemActor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Item/FHInventoryComponent.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AFHWorldItemActor::AFHWorldItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	InventoryComponent = CreateDefaultSubobject<UFHInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->OnInventoryChanged.AddDynamic(this, &ThisClass::OnInventoryChangedFunction);
	InventoryComponent->SetIsReplicated(true);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetIsReplicated(true);
	SetRootComponent(StaticMeshComponent);
}

// Called when the game starts or when spawned
void AFHWorldItemActor::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		StaticMeshComponent->SetSimulatePhysics(bIsSetSimulatePhysics);
	}

	// 클라는 세팅된상태로와서 OnInventoryChangedFunction가 발생안함...
	UFHItemBase* item = InventoryComponent->GetItemInstance<UFHItemBase>(0);
	if (item)
	{
		InventoryComponent->OnInventoryChangedFunction(0);
	}
}

void AFHWorldItemActor::OnInventoryChangedFunction(USiInventoryComponent* Inventory, int32 Index)
{
	UFHItemBase* item = Inventory->GetItemInstance<UFHItemBase>(Index);

	if (item)
	{
		UFHItemClassDataAsset* ItemDataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
		TSoftObjectPtr<UStaticMesh> ItemMeshSoftPtr = ItemDataAsset->GetItemMesh(item->GetItemData().MeshID);
		bool bIsPathValid = ItemMeshSoftPtr.ToSoftObjectPath().IsValid();
		bool bIsLoaded = ItemMeshSoftPtr.IsValid();

		if (bIsPathValid)
		{
			if (ItemMeshSoftPtr.IsValid())
			{
				SetItemMesh(ItemMeshSoftPtr);
			}
			else
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(
					ItemMeshSoftPtr.ToSoftObjectPath(),
					FStreamableDelegate::CreateUObject(this, &ThisClass::SetItemMesh, ItemMeshSoftPtr));
			}
		}
		else
		{
			TSoftObjectPtr<UTexture2D> ItemIconSoftPtr = ItemDataAsset->GetItemIcon(item->GetItemData().IconID);
			SetItemMesh(TestMesh);
			SetTempMaterial(TestMaterial);
			SetTempTexture(ItemIconSoftPtr);
		}
	}


}

void AFHWorldItemActor::SetItemMesh(TSoftObjectPtr<UStaticMesh> ItemMeshSoftPtr)
{
	StaticMeshComponent->SetStaticMesh(ItemMeshSoftPtr.LoadSynchronous());
	StaticMeshComponent->SetSimulatePhysics(bIsSetSimulatePhysics);
}

void AFHWorldItemActor::SetTempTexture(TSoftObjectPtr<UTexture2D> ItemIconSoftPtr)
{
	UMaterialInstanceDynamic* MaterialInstanceDynamic = StaticMeshComponent->CreateDynamicMaterialInstance(0);
	MaterialInstanceDynamic->SetTextureParameterValue(TEXT("TTT"), ItemIconSoftPtr.LoadSynchronous());
}

void AFHWorldItemActor::SetTempMaterial(TSoftObjectPtr<UMaterialInstance> ItemIconSoftPtr)
{
	StaticMeshComponent->SetMaterial(0, ItemIconSoftPtr.LoadSynchronous());
}
