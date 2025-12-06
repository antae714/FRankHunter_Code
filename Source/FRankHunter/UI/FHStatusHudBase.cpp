// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/FHStatusHudBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GAS\FHAttributeSet_PlayerStatus.h"
#include "GAS\FHAttributeSet_Health.h"
#include "GAS\FHAttributeSet_Stamina.h"
#include "Core/FHPlayerStateBase.h"
#include "Core/FHPlayerController.h"
#include "Item/Actors/FHBackpack.h"
#include "Item/FHInventoryComponent.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/ItemTypes.h"
#include "Item/FHItemBase.h"
#include "GAS/FHGameplayTags.h"
#include "GAS/Ability/Skill/FHGamePlaySkillAbility.h"


void UFHStatusHudBase::NativeConstruct()
{
	//InitAbilitySystem();
	//Super::NativeConstruct();

	//// Skill Hud Icon Change
	//AFHPlayerStateBase* PlayerState = GetOwningPlayerState<AFHPlayerStateBase>();
	//if (!IsValid(PlayerState))
	//{
	//	return;
	//}
	//SkillChanged(PlayerState->GetSkillID());

	Super::NativeConstruct();

	AFHPlayerStateBase* PlayerState = GetOwningPlayerState<AFHPlayerStateBase>();
	if (IsValid(PlayerState))
	{
		InitAbilitySystem();
		SkillChanged(PlayerState->GetSkillID());
	}
	else
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			PC->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPawnChanged);
		}
	}


	BP_SwitchHUDType(true);
}

void UFHStatusHudBase::InitAbilitySystem()
{
	AFHPlayerStateBase* PlayerState = GetOwningPlayerState<AFHPlayerStateBase>();
	if (!IsValid(PlayerState))
	{
		return;
	}

	IAbilitySystemInterface* IASI = Cast<IAbilitySystemInterface>(PlayerState);
	if (!IASI)
	{
		return;
	}

	UAbilitySystemComponent* ASC = IASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}


	PlayerState->OnLevelUp.AddUObject(this, &ThisClass::OnLevelChanged);
	PlayerState->OnExpGain.AddUObject(this, &ThisClass::OnExpChanged);
	PlayerState->OnRequiredExp.AddUObject(this, &ThisClass::OnMaxExpChanged);


	PlayerState->OnBackpackChange.AddUObject(this, &ThisClass::OnBackpackChange);

	UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(GetOwningPlayerState());
	if (InventoryComponent)
	{
		InventoryComponent->OnChangedFocus.AddDynamic(this, &ThisClass::OnFocusChange);
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &ThisClass::OnInventoryChanged);

		bIsHoldingItem = !!InventoryComponent->GetCurrentItem();
	}


	PlayerState->OnToggleTerminalDelegate.AddUObject(this, &ThisClass::OnHunterPhoneOpen);
	PlayerState->OnChangeInteractProgressPercentDelegate.AddUObject(this, &ThisClass::OnChangeInteractProgressPercent);
	{
		bIsEquipBackpack = !!PlayerState->GetCurrentBackpackActorCache();
	}
	{
		bIsHunterPhoneOpen = false;
	}
	OnChangeInteractProgressPercent(0.0f);

#define X(AttributeName, AttributeSetClass)                                                   \
        /* 값 변화 델리게이트 바인딩 */                                                      \
        ASC->GetGameplayAttributeValueChangeDelegate(                                       \
            AttributeSetClass::Get##AttributeName##Attribute()                          \
        ).AddUObject(this, &ThisClass::On##AttributeName##Changed);                                               \
                                                                                            \
        /* 초기값 읽어서 UI 갱신 */                                                          \
        {                                                                                   \
            bool bFound = false;                                                            \
            float Value = ASC->GetGameplayAttributeValue(                                   \
                AttributeSetClass::Get##AttributeName##Attribute(), bFound             \
            );                                                                              \
            ensure(bFound);                                                                 \
            Set##AttributeName(Value);                                                                   \
        }

	HUDSTATUS_ATTRIBUTES
#undef X
		;
	if (PlayerState->IsLoaded())
	{
		SetLevel(PlayerState->GetPlayerLevel());
		SetExp(PlayerState->GetExp());
		SetMaxExp(PlayerState->GetRequiredExp());
		bIsLoaded = true;
	}
	else
	{
		PlayerState->BindOrExecuteOnPlayerStateLoaded(FOnLoadEnd::FDelegate::CreateUObject(this, &ThisClass::OnLoadEnd));
	}

	ASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::BP_SetCoolTime);
	ASC->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::BP_OnActivateAbility);
	ASC->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::OnGameplayEffectApplied);
}

void UFHStatusHudBase::OnPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	AFHPlayerStateBase* PlayerState = GetOwningPlayerState<AFHPlayerStateBase>();
	if (IsValid(PlayerState))
	{
		InitAbilitySystem();
		SkillChanged(PlayerState->GetSkillID());

		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			PC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::OnPawnChanged);
		}
	}
}

void UFHStatusHudBase::OnLoadEnd(FPlayerSaveData saveData)
{
	SetLevel(saveData.Level);
	SetExp(saveData.Exp);
	SetMaxExp(saveData.RequiredExp);
	SetHealth(saveData.Health);
	

	AFHPlayerStateBase* PlayerState = GetOwningPlayerState<AFHPlayerStateBase>();
	if (!IsValid(PlayerState))
	{
		return;
	}

	IAbilitySystemInterface* IASI = Cast<IAbilitySystemInterface>(PlayerState);
	if (!IASI)
	{
		return;
	}

	UAbilitySystemComponent* ASC = IASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	bool bFound = false;
	SetHealth(ASC->GetGameplayAttributeValue(UFHAttributeSet_Health::GetHealthAttribute(), bFound));

	bIsLoaded = true;
}

void UFHStatusHudBase::OnBackpackChange(AFHBackpack* Backpack)
{
	bIsEquipBackpack = IsValid(Backpack);
}

void UFHStatusHudBase::OnFocusChange(int32 index)
{
	UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(GetOwningPlayerState());
	if (InventoryComponent)
	{
		UFHItemBase* CurrentItem = InventoryComponent->GetCurrentItem();
		if (CurrentItem)
		{
			bIsHoldingItem = true;
			CurrentItemTye = InventoryComponent->GetCurrentItem()->GetItemData().Type;
		}
		else
		{
			bIsHoldingItem = false;
			CurrentItemTye = EItemType::None;
		}
		CurrentFocusItem = CurrentItem;
		K2_OnFocusChange(CurrentItem);
	}
}

void UFHStatusHudBase::OnInventoryChanged(USiInventoryComponent* InventoryComponent, int32 index)
{
	OnFocusChange(index);
}

void UFHStatusHudBase::OnHunterPhoneOpen(bool isOpen)
{
	bIsHunterPhoneOpen = isOpen;
}

void UFHStatusHudBase::OnChangeInteractProgressPercent(float Percent)
{
	SetInteractProgressPercent(Percent);
}



#define X(AttributeName, AttributeSetClass)            \
void UFHStatusHudBase::On##AttributeName##Changed(const FOnAttributeChangeData& ChangeData) \
{                                                               \
	Origin_##AttributeName = ChangeData.NewValue;				\
    Set##AttributeName(ChangeData.NewValue);                    \
}

HUDSTATUS_ATTRIBUTES
#undef X



void UFHStatusHudBase::OnLevelChanged(uint32 NewValue)
{
	SetLevel(NewValue);
}

void UFHStatusHudBase::OnExpChanged(uint32 NewValue)
{
	SetExp(NewValue);
}

void UFHStatusHudBase::OnMaxExpChanged(uint32 NewValue)
{
	SetMaxExp(NewValue);
}

void UFHStatusHudBase::OnGameplayEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	FGameplayTagContainer GrantedTags;
	Spec.GetAllGrantedTags(GrantedTags);

	//if (GrantedTags.HasTag(GET_GAMEPLAY_TAG_PLAYER_COOLDOWN_ACTIVESKILL))
	//{
	//	const UGameplayAbility* SourceAbility = Spec.GetContext().GetAbility();
	//	if (SourceAbility)
	//	{
	//		const float Duration = Spec.GetDuration();
	//		// 임시_ 얼음척자가 없겠지만 어쩔수 없다 여러번 호출되는데 방어하기가 어려워서 그냥 유효한 duration값일때만 호출하기로
	//		if (Duration > 1.01f)
	//		{
	//			if (SourceAbility->GetAssetTags().HasTag(GET_GAMEPLAY_TAG_SKILL))
	//			{
	//				if (const UFHGamePlaySkillAbility* SkillAbility = Cast<UFHGamePlaySkillAbility>(Spec.GetContext().GetAbility()))
	//				{
	//					BP_OnStartSkillCooldown(SkillAbility, Duration);
	//				}
	//			}
	//		}
	//	}
	//}
}

void UFHStatusHudBase::SwitchHUDType_Implementation(bool isCommon)
{
	BP_SwitchHUDType(isCommon);
}
