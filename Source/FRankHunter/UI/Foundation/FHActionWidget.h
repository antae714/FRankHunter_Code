// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CommonActionWidget.h"
#include "FHActionWidget.generated.h"

/**
 * CommonUI와 Enhanced Input 시스템을 연동. Icon을 UI에 표시
 */
UCLASS(BlueprintType, Blueprintable)
class FRANKHUNTER_API UFHActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()
	
public:
	virtual FSlateBrush GetIcon() const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	const TObjectPtr<UInputAction> AssociatedInputAction;

private:

	class UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};
