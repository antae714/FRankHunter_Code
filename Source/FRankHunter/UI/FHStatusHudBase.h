// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "AttributeSet.h"
#include "FHStatusHudBase.generated.h"

class USiInventoryComponent;
struct FPlayerSaveData;
class UFHItemQuickSlot;
class AFHBackpack;
class UFHGamePlaySkillAbility;
enum class EItemType : uint8;

struct FOnAttributeChangeData;
DECLARE_DELEGATE(FOnAttributeChange);


#define HUDSTATUS_ATTRIBUTES \
    X(Health,			UFHAttributeSet_Health			) \
    X(MaxHealth,		UFHAttributeSet_Health			) \
    X(Stamina,			UFHAttributeSet_Stamina		    ) \
    X(MaxStamina,		UFHAttributeSet_Stamina		    ) \
    X(CarryWeight,      UFHAttributeSet_PlayerStatus    ) \
    X(MaxCarryWeight,   UFHAttributeSet_PlayerStatus    ) 





/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHStatusHudBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    void InitAbilitySystem();


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UFHItemQuickSlot> QuickSlotUI;


    UPROPERTY(BlueprintReadOnly)
    uint32 bIsLoaded : 1;

    UPROPERTY(BlueprintReadOnly)
    uint32 bIsHoldingItem : 1;

    UPROPERTY(BlueprintReadOnly)
    uint32 bIsHunterPhoneOpen : 1;

    UPROPERTY(BlueprintReadOnly)
    uint32 bIsEquipBackpack : 1;

    UPROPERTY(BlueprintReadOnly)
    EItemType CurrentItemTye;

    UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFHItemBase> CurrentFocusItem;

    UFUNCTION()
    void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);

    void OnLoadEnd(FPlayerSaveData saveData);

    void OnBackpackChange(AFHBackpack* Backpack);
    void OnHunterPhoneOpen(bool isOpen);
    void OnChangeInteractProgressPercent(float Percent);

    UFUNCTION()
    void OnFocusChange(int32 index);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnFocusChange"))
    void K2_OnFocusChange(UFHItemBase* CurrentItem);

    UFUNCTION()
    void OnInventoryChanged(USiInventoryComponent* InventoryComponent, int32 index);
    

    void OnLevelChanged(uint32 NewValue);
    void OnExpChanged(uint32 NewValue);
    void OnMaxExpChanged(uint32 NewValue);

#define X(AttributeName, AttributeSetClass)								\
        void On##AttributeName##Changed(const FOnAttributeChangeData& ChangeData);      \
	    int32 Origin_##AttributeName;

    HUDSTATUS_ATTRIBUTES
#undef X

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetLevel(int32 NewValue);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetExp(int32 NewValue);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetMaxExp(int32 NewValue);
        
    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetHealth(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetMaxHealth(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetStamina(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetMaxStamina(int32 NewValue);


    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetCarryWeight(float NewValue);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetMaxCarryWeight(float NewValue);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetInteractProgressPercent(float NewValue);



public:
    UFUNCTION()
    void OnGameplayEffectApplied(UAbilitySystemComponent* ASC, const struct FGameplayEffectSpec& Spec, struct FActiveGameplayEffectHandle Handle);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SkillChanged(FName SkillID);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void BP_SetCoolTime(UGameplayAbility* Ability);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void BP_OnActivateAbility(UGameplayAbility* Ability);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void BP_OnStartSkillCooldown(const UFHGamePlaySkillAbility* Ability, float Duration);
    

    UFUNCTION(Client, Reliable)
    void SwitchHUDType(bool isCommon);
    void SwitchHUDType_Implementation(bool isCommon);

    UFUNCTION(BlueprintImplementableEvent)
    void BP_SwitchHUDType(bool isCommon);

private:


};
