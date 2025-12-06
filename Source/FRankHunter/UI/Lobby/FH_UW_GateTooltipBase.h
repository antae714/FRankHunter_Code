// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FH_UW_GateTooltipBase.generated.h"

class UTextBlock;
class UDataTable;
class UButton;
enum class EGateRank : uint8;
class UFHGateTooltipRPCComponent;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_UW_GateTooltipBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void PostLoad() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> GateName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UButton> GateEnter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GateInfo")
	FName GateThema;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GateRank")
	EGateRank GateRank;

	UFUNCTION(BlueprintCallable)
	void UpdateMapData(const EGateRank& GateGrade);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateMapData(const EGateRank& GateGrade);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetEnterState(bool CanEnter);
};
