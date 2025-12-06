// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FH_W_GateEnterRequest.generated.h"

class UButton;
class UTextBlock;
class USizeBox;
class UFHGateEnterRequestRPCComponent;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_W_GateEnterRequest : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;
	UFUNCTION()
	void SetLeftTime(int32 Time);
	void SetRefusePlayerName(const FString& PlayerName);
	UFUNCTION(BlueprintImplementableEvent)
	void OpenErrorWidget();	
	UFUNCTION(BlueprintImplementableEvent)
	void OpenAnswerWidget(const FString& GateName);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OpenTimeOutWidget();
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerAnswer(const FString& PlayerName);

	UFUNCTION(BlueprintCallable)
	void ResetRefusePlayerText();
	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetOpened();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<USizeBox> EnterErrorBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<USizeBox> EnterRequestBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<USizeBox> TimeOutBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> GateNameQuestion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> Player1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> Player2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> Player3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> Player4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TArray<TObjectPtr<UTextBlock>> PlayerTexts;

	int32 CurIdIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RPCComponent")
	TObjectPtr<UFHGateEnterRequestRPCComponent> RPCComponent;
};
