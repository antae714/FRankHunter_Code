// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "FH_W_SelectGate.generated.h"

class UTextBlock;
class UButton;
class UImage;
class UFH_GateSelectButton;
class UFH_UW_GateTooltipBase;
class UCanvasPanel;


/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_W_SelectGate : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void PostLoad() override;

	UFUNCTION(BlueprintCallable)
	void ResetGateSelectPannel(const TArray<uint8>& SelectedGateInfo);
	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetOpened();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UFH_GateSelectButton> GateSelectE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UFH_GateSelectButton> GateSelectD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UFH_GateSelectButton> GateSelectC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UFH_GateSelectButton> GateSelectB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UFH_GateSelectButton> GateSelectA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UFH_GateSelectButton> GateSelectS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataTable")
	TObjectPtr<UDataTable> AreaUIData;

	UPROPERTY()
	TArray<TObjectPtr<UFH_GateSelectButton>> GateButtons;

private:
	void SetButtonLocation(TObjectPtr<UFH_GateSelectButton> GateButton, int32 AreaID);
};
