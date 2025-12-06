// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "AttributeSet.h"
#include "FHStatusBase.generated.h"

struct FPlayerSaveData;
struct FOnAttributeChangeData;
DECLARE_DELEGATE(FOnAttributeChange);

enum class EPrimaryAttribute : uint8;


// X(
//   AttributeName            : 속성 식별자 (GetAttributeNameAttribute() 의 Name 부분)
//   AttributeSetClass        : 속성이 정의된 AttributeSet 클래스
//   ChangeCallbackFunction   : 값이 바뀔 때 호출될 콜백 함수명
//   UIUpdateFunction         : 새로운 값을 UI에 반영할 함수명
//   ValueType                : 속성 값의 타입
// )

#define STATUS_ATTRIBUTES \
    X(Health,			UFHAttributeSet_Health			) \
    X(MaxHealth,		UFHAttributeSet_Health			) \
    X(Stamina,			UFHAttributeSet_Stamina		    ) \
    X(MaxStamina,		UFHAttributeSet_Stamina		    ) \
    X(CarryWeight,		UFHAttributeSet_PlayerStatus		    ) \
    X(MaxCarryWeight,	UFHAttributeSet_PlayerStatus		    ) \
    X(Strength,			UFHAttributeSet_PlayerStatus	) \
    X(Vitality,			UFHAttributeSet_PlayerStatus	) \
    X(Agility,			UFHAttributeSet_PlayerStatus	) \
    X(Endurance,		UFHAttributeSet_PlayerStatus	) \
    X(Willpower,		UFHAttributeSet_PlayerStatus	) \
    X(AttackPower,		UFHAttributeSet_PlayerStatus	) \
    X(MiningPower,		UFHAttributeSet_PlayerStatus	) \
    X(RunSpeed,			UFHAttributeSet_Movement		) \



UCLASS()
class FRANKHUNTER_API UFHStatusScreenBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void ToggleUI();
};

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHStatusBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;


protected:
	void InitAbilitySystem();

	UFUNCTION()
	void InitSkillName(const FName& SkillID);

	UPROPERTY(BlueprintReadOnly)
	uint32 bIsLoaded : 1;

	void OnLoadEnd(FPlayerSaveData saveData);

	int32 STR_Point;
	int32 VIT_Point;
	int32 DEX_Point;
	int32 END_Point;
	int32 WIL_Point;
	int32 DeltaStatPoint;
	int32 OriginStatPoint;


	UFUNCTION(BlueprintCallable)
	void StatusUp(EPrimaryAttribute type);

	UFUNCTION(BlueprintPure)
	int32 GetStatusPoint(EPrimaryAttribute type);

	UFUNCTION(BlueprintCallable)
	void StatusAccept();

	UFUNCTION(BlueprintCallable)
	void StatusCancel();

	UFUNCTION(BlueprintImplementableEvent)
	void SetSkillInfo(FName SkillID);


	void OnExpChanged(uint32 NewValue);
	void OnMaxExpChanged(uint32 NewValue);
	void OnLevelChanged(uint32 NewValue);
	void OnStatPointChanged(uint32 NewValue);

    #define X(AttributeName, AttributeSetClass)											\
        void On##AttributeName##Changed(const FOnAttributeChangeData& ChangeData);      \
																						\
	int32 Origin_##AttributeName;
    STATUS_ATTRIBUTES
    #undef X

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetLevel(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetExp(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetMaxExp(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetStatPoint(int32 NewValue);

    UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
    void SetHealth(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetMaxHealth(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetStamina(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetMaxStamina(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetCarryWeight(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetMaxCarryWeight(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetStrength(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetVitality(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetAgility(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetEndurance(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetWillpower(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetAttackPower(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetMiningPower(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetRunSpeed(int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "UserInterface")
	void SetSkillName(const FText& NewValue);
};
