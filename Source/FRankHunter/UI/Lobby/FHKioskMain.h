// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHKioskMain.generated.h"

enum class EUIType : uint8;
enum class EPurchaseError : uint8;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHKioskMain : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widgets", meta = (BindWidget))
	TObjectPtr<class UFHBuyUIBase> BuyUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widgets", meta = (BindWidget))
	TObjectPtr<class UFHSellUIBase> SellUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup")
	TSubclassOf<class UCommonUserWidget> PopupWidgetClass;

	UFUNCTION(BlueprintCallable)
	void OnExitButtonClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void BackToHome();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePurchaseError(const EPurchaseError& Error);
};
