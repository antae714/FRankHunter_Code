// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "UI/Foundation/FHTabListWidget.h"
#include "UI/Foundation/FHButtonBase.h"
#include "FHTabButton.generated.h"

class UCommonLazyImage;

UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class FRANKHUNTER_API UFHTabButton : public UFHButtonBase, public IFHTabButtonInterface
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	virtual void SetTabDescriptor_Implementation(const FFHTabDescriptor& TabDescriptor) override;

public:
	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	void SetIconBrush(const FSlateBrush& Brush);

private:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImageIcon;
};
