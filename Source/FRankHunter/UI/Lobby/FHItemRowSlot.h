// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHItemRowSlot.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHItemRowSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	const TArray<UWidget*> GetChildArray();
	UWidget* GetItemSlot(int32 ItemIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> ItemRow;
};
