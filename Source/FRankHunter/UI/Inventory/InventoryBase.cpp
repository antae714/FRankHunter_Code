// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Inventory/InventoryBase.h"
#include "Components/PanelWidget.h"
#include "Item\FHInventoryComponent.h"
#include "UI/Inventory/InventoryItemBase.h"
#include "Item/FHItemBase.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Blueprint/WidgetTree.h"

#include "Kismet/GameplayStatics.h"
#include "Lobby/FHLobbyGameMode.h"


void UInventoryLineBase::SetElementCount(int32 NewElementCount, TSubclassOf<UInventoryItemBase> ItemUIClass, float padding)
{
    if (NewElementCount <= 0)
    {
        return;
    }

    ItemsParent->ClearChildren();
    ItemArray.Empty();

    for (int32 i = 0; i < NewElementCount; ++i)
    {
        UInventoryItemBase* ItemWidget = CreateWidget<UInventoryItemBase>(this, ItemUIClass);

        if (ItemWidget)
        {
            USizeBox* SBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
            UPanelSlot* BaseSlot = SBox->AddChild(ItemWidget);
            UPanelSlot* BaseSlot2 = ItemsParent->AddChild(SBox);
            ItemArray.Add(ItemWidget);

            if (USizeBoxSlot* SBoxSlot = Cast<USizeBoxSlot>(BaseSlot))
            {
                SBoxSlot->SetPadding(padding);
                //SBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
                //SBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
            }


            if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(BaseSlot2))
            {
                // '채우기' 설정 (Fill)
                FSlateChildSize FillSize;
                FillSize.SizeRule = ESlateSizeRule::Fill;
                FillSize.Value = 1.0f; // 비율 설정 (여러 슬롯이 있으면 비율로 나눔)

                HBoxSlot->SetSize(FillSize);
            }
        }
    }
}

TObjectPtr<UInventoryItemBase> UInventoryLineBase::GetElementAt(int32 index) const
{
	if (ItemArray.IsValidIndex(index))
	{
		return ItemArray[index];
	}
    else
    {
        return nullptr;
    }
}



UInventoryBase::UInventoryBase()
{

}

void UInventoryBase::NativePreConstruct()
{
	Super::NativePreConstruct();
    ItemsLineArray.Empty();


#if WITH_EDITOR
	if (IsDesignTime())
	{
        InventorySize = InventorySizePreview;
        SetInventoryComponent(nullptr);
	}
#endif

}

void UInventoryBase::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

    FVector2D PixelSize = ItemsParent->GetCachedGeometry().GetLocalSize();
    if (CachedPixelSize != PixelSize)
    {
        CachedPixelSize = PixelSize;
        ReSize(CachedPixelSize);
    }
}

void UInventoryBase::Init()
{
    ItemsParent->ClearChildren();
    ItemsLineArray.Empty();

	UFHInventoryComponent* InventoryComponent = InventoryWeak.Get();

    for (size_t i = 0; i < InventorySize; i += InventoryRowCount)
    {
		TObjectPtr<UInventoryLineBase> ItemsLine = CreateWidget<UInventoryLineBase>(this, ItemLineClass);
		ItemsLine->SetElementCount(InventoryRowCount, ItemClass);
        ItemsLineArray.Add(ItemsLine);

        USizeBox* SBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		SBox->AddChild(ItemsLine);
        UPanelSlot* BaseSlot = ItemsParent->AddChild(SBox);
    }

    for (size_t i = 0; i < InventorySize; i++)
    {
        ItemChanged(InventoryComponent, i);
    }



#if WITH_EDITOR
    if (IsDesignTime())
    {
        ReSize(DesignTimeSize);
    }
    else
#endif
    {
        CachedPixelSize = ItemsParent->GetCachedGeometry().GetLocalSize();
        ReSize(CachedPixelSize);
    }
}

void UInventoryBase::ReSize(FVector2D NewSize)
{
    float ElementSize = NewSize.X / (float)InventoryRowCount;

    for (auto& item : ItemsParent->GetAllChildren())
    {
        if (USizeBox* SBox = Cast<USizeBox>(item))
        {
            SBox->SetWidthOverride(NewSize.X);
            SBox->SetHeightOverride(ElementSize);
        }
    }

}

void UInventoryBase::ItemChanged(USiInventoryComponent* InventoryComponent, int32 index)
{
    if (InventorySize <= index)
    {
        SetInventoryComponent(Cast<UFHInventoryComponent>(InventoryComponent));
    }

    FIntVector2 InventoryIndex = GetIndex(index);
    UFHItemBase* itemInstance;
    int32 itemStack;

    if(InventoryComponent)
	{
        itemInstance = InventoryComponent->GetItemInstance<UFHItemBase>(index);
        itemStack = InventoryComponent->GetItemStack(index);
	}
#if WITH_EDITOR
    else if(IsDesignTime())
    {
        itemInstance = AddItemPreview.IsValidIndex(index) ? AddItemPreview[index] : nullptr;
		itemStack = 1;
    }
#endif
    else
    {
        check(InventoryComponent);
		itemInstance = nullptr;
		itemStack = 0;
    }

    ItemsLineArray[InventoryIndex.Y]->GetElementAt(InventoryIndex.X)->SetItemInfo(InventoryComponent, itemInstance, itemStack, index);

}

FIntVector2 UInventoryBase::GetIndex(int32 index)
{
	int32 x = index % InventoryRowCount;
	int32 y = index / InventoryRowCount;
	return FIntVector2(x, y);
}

void UInventoryBase::SetInventoryComponent(UFHInventoryComponent* InventoryComponent)
{
    if (InventoryComponent)
    {
        if (InventoryWeak.IsValid())
        {
            if (InventoryWeak->OnInventoryChanged.IsAlreadyBound(this, &UInventoryBase::ItemChanged))
            {
                InventoryWeak->OnInventoryChanged.RemoveDynamic(this, &UInventoryBase::ItemChanged);
            }
        }

        InventoryWeak = InventoryComponent;
        InventorySize = InventoryWeak->GetItemNum();

        if (InventoryWeak.IsValid())
        {
            InventoryWeak->OnInventoryChanged.AddDynamic(this, &UInventoryBase::ItemChanged);
        }
        Init();
    }
    else
    {
        Init();
    }
}
