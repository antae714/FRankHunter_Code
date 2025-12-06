// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHTestCart.h"
#include "UI/Inventory/InventoryBase.h"
#include "Component/FHInteractableComponent.h"
#include "Core/FHPlayerController.h"
#include "Player/FHPlayerBase.h"
#include "Kismet/GameplayStatics.h"

#include "Item/ItemTypes.h"
#include "Item/FHItemBase.h"
#include "Item/Data/FHItemSubsystem.h"
#include "Item/FHInventoryComponent.h"
#include "UI/Lobby/FHShopBase.h"
#include "Core/FHUIManager.h"

// Sets default values
AFHTestCart::AFHTestCart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	InteractableComponent = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComponent"));
	InventoryComponent = CreateDefaultSubobject<UFHInventoryComponent>(TEXT("InventoryComponent"));
}

// Called when the game starts or when spawned
void AFHTestCart::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		LockNotUsedSlot();
	}
	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AFHTestCart::LateInit);
}

// Called every frame
void AFHTestCart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHTestCart::Interact_Impl(AFHPlayerBase* Player)
{
	AFHPlayerController* PC = Cast<AFHPlayerController>(Player->GetController());
	if (PC)
	{
		PC->OpenKiosk();
	}
}

void AFHTestCart::AddItem(TSubclassOf<UFHItemBase> ItemClass, int32 ItemCount)
{
	//UFHInventoryComponent* InventoryComponent = GetComponentByClass<UFHInventoryComponent>();
	if (InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemAdded."));
		InventoryComponent->Server_AddItem(ItemClass, ItemCount);
	}
}

void AFHTestCart::LateInit()
{
	//AFHPlayerController* PC = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (PC && PC->IsLocalController())
	//{
	//	if (PC->UIManager)
	//	{
	//		UFHShopBase* ShopUI = PC->UIManager->GetWidget<UFHShopBase>(TEXT("Shop"));
	//		if (ShopUI)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("ShopUI Added."));
	//			//ShopUI->CartInventoryUI->SetInventoryComponent(GetComponentByClass<UFHInventoryComponent>());
	//			ShopUI->CartInventoryUI->SetInventoryComponent(InventoryComponent);
	//			UE_LOG(LogTemp, Warning, TEXT("Cart InventorySize: %d"), ShopUI->CartInventoryUI->InventorySize);
	//		}
	//	}
	//}
}

void AFHTestCart::LockNotUsedSlot()
{
	if (InventoryComponent)
	{
		for (int32 i = StartingSlotCount; i < InventoryComponent->GetItemNum(); i++)
		{
			InventoryComponent->ItemLock(i, true);
		}
	}
}

