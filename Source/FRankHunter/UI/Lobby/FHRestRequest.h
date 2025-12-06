// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "FHRestRequest.generated.h"

class UFHRestRequestRPCComponent;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHRestRequest : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetQuota();

	UFUNCTION(BlueprintImplementableEvent)
	void SetLeftTime(int32 TIme);

	UFUNCTION(BlueprintImplementableEvent)
	void OpenWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void CloseWidget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RPCComponent")
	TObjectPtr<UFHRestRequestRPCComponent> RPCComponent;
};
