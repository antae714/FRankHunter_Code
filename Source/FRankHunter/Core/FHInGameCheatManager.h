// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "CommonUserWidget.h"
#include "GameplayEffect.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "FHInGameCheatManager.generated.h"

class UVerticalBox;
class UScrollBox;
class UCommonTextBlock;
class UButton;
class UEditableText;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHInGameCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCommonUserWidget> InGameCheatWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UCommonUserWidget> InGameCheatWidgetInstance;

private:
	virtual void InitCheatManager() override;
	
	UFUNCTION()
	void InitializeCheatWidget(APawn* OldPawn, APawn* NewPawn);


	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void GetDamageSelf(float amount);

	UFUNCTION(Server, Reliable)
	void Server_GetDamageSelf(float amount);


	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void ApplyEffect(FName EffectName);
	//UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	//virtual void RemoveEffect(FName EffectName);


	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void PrintNetMode();

	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void PrintOSSMode();

	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void GetExp(float amount);

	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void GetItem(FName ItemID, int32 count = 1);

	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void GateClear();

	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void GetMoney(int32 Amount);

	UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	virtual void SetSkill(FName SkillID);

	//UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	//virtual void SaveGame();

	//UFUNCTION(exec, BlueprintCallable, Category = "InGame Cheat Manager")
	//virtual void LoadGame();
};


UCLASS()
class FRANKHUNTER_API UCheatItemUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void Setting(UFunction* function);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Cmd;

	int32 NumParam;

	UFUNCTION()
	void Cheat();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> CheatButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableText> ParamEditBox;

	
};

UCLASS()
class FRANKHUNTER_API UCheatManageUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> InGameCheatManager;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> CheatManager;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCheatItemUI> ItemClass;
};

UCLASS()
class FRANKHUNTER_API UFH_GE_SimpleDamage : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UFH_GE_SimpleDamage();

};
