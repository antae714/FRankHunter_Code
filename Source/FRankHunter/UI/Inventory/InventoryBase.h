// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommonActivatableWidget.h"
#include "InventoryBase.generated.h"

class UInventoryItemBase;
class UPanelWidget;
class UHorizontalBox;
class USizeBox;
class UFHInventoryComponent;
class USiInventoryComponent;
class UFHItemBase;

UCLASS()
class FRANKHUNTER_API UInventoryLineBase : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetElementCount(int32 NewElementCount, TSubclassOf<UInventoryItemBase> ItemUIClass, float padding = 10);
	TObjectPtr<UInventoryItemBase> GetElementAt(int32 index) const;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> ItemsParent;

	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemBase>> ItemArray;

};


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UInventoryBase : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UInventoryBase();

public:
	
	void SetInventoryComponent(UFHInventoryComponent* InventoryComponent);

	UFUNCTION(CallInEditor)
	void ReSize(FVector2D NewSize);
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void Init();

	FVector2D CachedPixelSize;

	UFUNCTION()
	void ItemChanged(USiInventoryComponent* InventoryComponent, int32 index);


	FIntVector2 GetIndex(int32 index);

public:


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> ItemsParent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UInventoryLineBase>> ItemsLineArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventoryItemBase> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventoryLineBase> ItemLineClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 InventoryRowCount;
	int32 InventorySize;

#if WITH_EDITOR
#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryBase|Preview", Instanced)
	TArray<TObjectPtr<UFHItemBase>> AddItemPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryBase|Preview")
	int32 InventorySizePreview;

#endif // WITH_EDITORONLY_DATA
#endif // WITH_EDITOR

	UPROPERTY()
	TWeakObjectPtr<UFHInventoryComponent> InventoryWeak;
};
