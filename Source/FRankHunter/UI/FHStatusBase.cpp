// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/FHStatusBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GAS\FHAttributeSet_PlayerStatus.h"
#include "GAS\FHAttributeSet_Health.h"
#include "GAS/Attributes/FHAttributeSet_Movement.h"
#include "GAS\FHAttributeSet_Stamina.h"
#include "Core/FHPlayerStateBase.h"


void UFHStatusBase::NativeConstruct()
{
	InitAbilitySystem();
	Super::NativeConstruct();
}

void UFHStatusBase::InitSkillName(const FName& SkillID)
{
	SetSkillInfo(SkillID);
	//static const TMap<FName, FString> SkillNameMap =
	//{
	//	{ "skill_001", TEXT("Tinker	(Toggle)") },
	//	{ "skill_002", TEXT("Alchemist (Active)") },
	//	{ "skill_003", TEXT("Breather (Active)") },
	//	{ "skill_004", TEXT("Breaker (Passive)") },
	//	{ "skill_005", TEXT("Balancer (Passive)") },
	//	{ "skill_006", TEXT("Seeker	(Active)") },
	//	{ "skill_007", TEXT("Stopper (Active)") },
	//};
	//if (const FString* SkillName = SkillNameMap.Find(SkillID))
	//{
	//	SetSkillName(FText::FromString(*SkillName));
	//}
}

void UFHStatusBase::InitAbilitySystem()
{
	AFHPlayerStateBase* PlayerState = GetOwningPlayerState<AFHPlayerStateBase>();
	IAbilitySystemInterface* IASI = Cast<IAbilitySystemInterface>(PlayerState);
	UAbilitySystemComponent* ASC = IASI ? IASI->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		return;
	}
	PlayerState->OnLevelUp.AddUObject(this, &ThisClass::OnLevelChanged);
	PlayerState->OnExpGain.AddUObject(this, &ThisClass::OnExpChanged);
	PlayerState->OnRequiredExp.AddUObject(this, &ThisClass::OnMaxExpChanged);
	PlayerState->OnUseStatPoints.AddUObject(this, &ThisClass::OnStatPointChanged);


	{
		float Value = PlayerState->GetStatPoints();
		SetStatPoint(Value);
		OriginStatPoint = Value;
	}


#define X(AttributeName, AttributeSetClass)												\
        /* 값 변화 델리게이트 바인딩 */												 	\
        ASC->GetGameplayAttributeValueChangeDelegate(									\
            AttributeSetClass::Get##AttributeName##Attribute()							\
        ).AddUObject(this, &ThisClass::On##AttributeName##Changed);			\
        /* 초기값 읽어서 UI 갱신 */														\
        {																				\
            bool bFound = false;														\
            float Value = ASC->GetGameplayAttributeValue(								\
                AttributeSetClass::Get##AttributeName##Attribute(), bFound				\
            );																			\
            ensure(bFound);																\
            Set##AttributeName(Value);													\
			Origin_##AttributeName = Value;												\
        }																				


	STATUS_ATTRIBUTES
#undef X


	StatusCancel();
	PlayerState->BindOrExecuteOnPlayerStateLoaded(FOnLoadEnd::FDelegate::CreateUObject(this, &ThisClass::OnLoadEnd));
	PlayerState->OnSkillSettedDelegate.BindDynamic(this, &UFHStatusBase::InitSkillName);
}

void UFHStatusBase::OnLoadEnd(FPlayerSaveData saveData)
{
	SetLevel(saveData.Level);
	SetExp(saveData.Exp);
	SetMaxExp(saveData.RequiredExp);
	SetHealth(saveData.Health);

	/** 스킬이름 표시 (거지같지만 하드코딩) **/
	
	if (!saveData.SkillID.IsNone())
	{
		SetSkillInfo(saveData.SkillID);
		//static const TMap<FName, FString> SkillNameMap = 
		//{
		//	{ "skill_001", TEXT("Tinker	(Toggle)") },
		//	{ "skill_002", TEXT("Alchemist (Active)") },
		//	{ "skill_003", TEXT("Breather (Active)") },
		//	{ "skill_004", TEXT("Breaker (Passive)") },
		//	{ "skill_005", TEXT("Balancer (Passive)") },
		//	{ "skill_006", TEXT("Seeker	(Active)") },
		//	{ "skill_007", TEXT("Stopper (Active)") },
		//};

		//if (const FString* SkillName = SkillNameMap.Find(saveData.SkillID))
		//{
		//	SetSkillName(FText::FromString(*SkillName));
		//}
	}
	//SetStamina(Origin_Stamina);

	bIsLoaded = true;
}

void UFHStatusBase::StatusUp(EPrimaryAttribute type)
{
	if(OriginStatPoint - DeltaStatPoint <= 0)
	{
		return;
	}
	++DeltaStatPoint;
	SetStatPoint(OriginStatPoint - DeltaStatPoint);
	switch (type)
	{
	case EPrimaryAttribute::Vitality:
		++VIT_Point;
		break;
	case EPrimaryAttribute::Strength:
		++STR_Point;
		break;
	case EPrimaryAttribute::Agility:
		++DEX_Point;
		break;
	case EPrimaryAttribute::Endurance:
		++END_Point;
		break;
	case EPrimaryAttribute::Willpower:
		++WIL_Point;
		break;
	default:
		break;
	}

}

int32 UFHStatusBase::GetStatusPoint(EPrimaryAttribute type)
{
	switch (type)
	{
	case EPrimaryAttribute::Vitality:
		return VIT_Point;
	case EPrimaryAttribute::Strength:
		return STR_Point;
	case EPrimaryAttribute::Agility:
		return DEX_Point;
	case EPrimaryAttribute::Endurance:
		return END_Point;
	case EPrimaryAttribute::Willpower:
		return WIL_Point;
	default:
		return 0;
	}
}

void UFHStatusBase::StatusAccept()
{
	AFHPlayerStateBase* PlayerState = GetOwningPlayerState<AFHPlayerStateBase>();
	if (!IsValid(PlayerState))
	{
		return;
	}
	PlayerState->UseStatPointsToIncreaseStat(EPrimaryAttribute::Strength, STR_Point);
	PlayerState->UseStatPointsToIncreaseStat(EPrimaryAttribute::Vitality, VIT_Point);
	PlayerState->UseStatPointsToIncreaseStat(EPrimaryAttribute::Agility, DEX_Point);
	PlayerState->UseStatPointsToIncreaseStat(EPrimaryAttribute::Endurance, END_Point);
	PlayerState->UseStatPointsToIncreaseStat(EPrimaryAttribute::Willpower, WIL_Point);

	StatusCancel();
}

void UFHStatusBase::StatusCancel()
{
	STR_Point = 0;
	VIT_Point = 0;
	DEX_Point = 0;
	END_Point = 0;
	WIL_Point = 0;
	DeltaStatPoint = 0;

	SetStrength(Origin_Strength);
	SetVitality(Origin_Vitality);
	SetAgility(Origin_Agility);
	SetEndurance(Origin_Endurance);
	SetWillpower(Origin_Willpower);
	SetStatPoint(OriginStatPoint);

}

void UFHStatusBase::OnExpChanged(uint32 NewValue)
{
	SetExp(NewValue);
}

void UFHStatusBase::OnMaxExpChanged(uint32 NewValue)
{
	SetMaxExp(NewValue);
}

void UFHStatusBase::OnLevelChanged(uint32 NewValue)
{
	SetLevel(NewValue);
}

void UFHStatusBase::OnStatPointChanged(uint32 NewValue)
{
	OriginStatPoint = NewValue;
	SetStatPoint(NewValue);
}

// OnChanged 콜백에서 그냥 Set 호출만 매크로로 정의
#define X(AttributeName, AttributeSetClass)            \
void UFHStatusBase::On##AttributeName##Changed(const FOnAttributeChangeData& ChangeData) \
{                                                               \
	Origin_##AttributeName = ChangeData.NewValue;				\
	Set##AttributeName(ChangeData.NewValue);                    \
}

STATUS_ATTRIBUTES
#undef X

