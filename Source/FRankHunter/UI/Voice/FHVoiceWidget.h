// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHVoiceWidget.generated.h"

class UBorder;
class UTextBlock;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHVoiceWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void RegistLocalPlayer(const FUniqueNetIdRepl& UniqueNetId);
	UFUNCTION(BlueprintCallable)
	void RegistRemotePlayer(const FUniqueNetIdRepl& UniqueNetId);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UBorder> P1Border;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UBorder> P2Border;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> P1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> P2;

	UPROPERTY()
	TArray<TObjectPtr<UBorder>> PlayerBorders;
	
	UPROPERTY()
	TMap<FUniqueNetIdRepl, TObjectPtr<UBorder>> PlayerTalkMap;

	UFUNCTION(BlueprintCallable)
	void OnPlayerTalking(const FBPUniqueNetId& UniqueNetID, bool IsTalking);

	void OnPlayerTalking(TSharedRef<const FUniqueNetId> UniqueNetIdRef, bool IsTalking);
};
