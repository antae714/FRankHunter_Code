// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/FHItemBase.h"
#include "Item/FHInventoryComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GAS/FHGameplayAbility.h"
#include "Net\UnrealNetwork.h"
#include "GAS/FHGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/Abilities/FH_GE_ItemPassive.h"
#include "GameFramework\GameState.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"

#include "Core/FHGateGameModeBase.h"




UFHItemBase::UFHItemBase()
{
	PassiveEffect = UFH_GE_ItemPassive::StaticClass();
}

void UFHItemBase::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		if (const UFHItemBase* CDO = GetClass()->GetDefaultObject<UFHItemBase>())
		{
			SetItemData(CDO->GetItemData());
		}
	}
}

void UFHItemBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFHItemBase, lastUseTime);
	DOREPLIFETIME(UFHItemBase, CurrentCondition);

}

void UFHItemBase::SetOwnerComp(USiInventoryComponent* NewOwnerComp)
{
	Super::SetOwnerComp(NewOwnerComp);
	lastUseTime = 0;

	
	if (UAbilitySystemComponent* AbilitySystem = ASC.Get(); AbilitySystem)
	{
		if (PassiveEffectHandle.IsValid())
		{
			AbilitySystem->RemoveActiveGameplayEffect(PassiveEffectHandle);
		}
		if (CurrentHoldAbilitySpecHandle.IsValid())
		{
			//AbilitySystem->CancelAbilityHandle(CurrentHoldAbilitySpecHandle);
			AbilitySystem->ClearAbility(CurrentHoldAbilitySpecHandle);
		}
		if (PrePareUseAbilitySpecHandle.IsValid())
		{
			AbilitySystem->ClearAbility(PrePareUseAbilitySpecHandle);
		}
		if (UseAbilitySpecHandle.IsValid())
		{
			AbilitySystem->ClearAbility(UseAbilitySpecHandle);
		}
	}
	
	if (NewOwnerComp)
	{
		AActor* OwnerActor = nullptr;
		AController* OwnerController = OwnerComp->GetOwner<AController>();
		if (OwnerController)
		{
			OwnerActor = OwnerController->GetPawn();
		}
		else
		{
			OwnerActor = OwnerComp->GetOwner();
		}
		if (!OwnerActor)
		{
			UE_LOG(LogTemp, Error, TEXT("Owner is Null!"));
			return;
		}

		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerActor);
		if (AbilitySystemInterface)
		{
			ASC = AbilitySystemInterface->GetAbilitySystemComponent();
		}
	}
	else
	{
		ASC = nullptr;
	}


}

void UFHItemBase::ChangeStackCount(int NewStack)
{
	if (UAbilitySystemComponent* AbilitySystem = ASC.Get(); AbilitySystem)
	{
		const float NewWeight = GetItemData().Weight * NewStack;
		bool bNeedsApply = false;

		// 1) PassiveEffectHandle이 유효한지, 그리고 실제로 ASC에 남아있는지 확인
		if (PassiveEffectHandle.IsValid())
		{
			// ASC에 이 핸들로 등록된 ActiveEffect가 있는지 조회
			const FActiveGameplayEffect* ActiveGE = AbilitySystem->GetActiveGameplayEffect(PassiveEffectHandle);
			if (ActiveGE)
			{
				// 남아있다면 SetByCaller 값만 업데이트
				AbilitySystem->UpdateActiveGameplayEffectSetByCallerMagnitude(
					PassiveEffectHandle,
					GET_GAMEPLAY_TAG_PLAYER_STATUS_CARRYWEIGHT,
					NewWeight
				);
			}
			else
			{
				// 유효한 핸들이지만 이미 제거되었다면, 새로 적용해야 함
				bNeedsApply = true;
			}
		}
		else
		{
			// 핸들이 없으면 당연히 새로 적용
			bNeedsApply = true;
		}

		// 2) 이펙트를 다시 붙일 필요가 있다면(첫 적용 혹은 제거 후 재적용)
		if (bNeedsApply && PassiveEffect)
		{
			FGameplayEffectContextHandle Context = AbilitySystem->MakeEffectContext();

			FGameplayEffectSpecHandle SpecHandle =
				AbilitySystem->MakeOutgoingSpec(PassiveEffect, 1, Context);

			SpecHandle.Data->SetSetByCallerMagnitude(
				GET_GAMEPLAY_TAG_PLAYER_STATUS_CARRYWEIGHT,
				NewWeight
			);

			PassiveEffectHandle =
				AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}



void UFHItemBase::SetItemData(const FFHItemData& data)
{
	ItemData = data;
	ItemID = ItemData.ItemID;
	//CurrentCondition = data.DefaultCondition;
	ItemMaxStack = data.MaxStack;

	UFHItemClassDataAsset* ItemDA = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	if (ItemDA)
	{
		UseAbility = ItemDA->GetItemAbility(data.UseAbility);
		PassiveEffect = ItemDA->GetItemPassiveEffect(data.PassiveEffect);
		CurrentHoldAbility = ItemDA->GetItemAbility(data.CurrentHoldAbility);
		PrePareUseAbility = ItemDA->GetItemAbility(data.PrePareUseAbility);
	}

}

bool UFHItemBase::CanUse()
{
	double currentTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	bool bCanUse = lastUseTime + GetItemData().Cooldown <= currentTime;

	return bCanUse;
}

void UFHItemBase::RefreshCooldown()
{

	lastUseTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void UFHItemBase::ConsumeCondition()
{
	if (!GetOwnerComp()) return;
	if (!GetOwnerComp()->GetOwner()->HasAuthority()) return;

	if (ItemData.Type == EItemType::Tool)
	{
		if (AFHGateGameModeBase* GateMode = Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode()); GateMode == nullptr) return;
	}

	CurrentCondition -= GetItemData().ConditionCost;
	bool bIsBroken = CurrentCondition <= 0;
	if (bIsBroken)
	{
		CurrentCondition = GetItemData().DefaultCondition;
		lastUseTime = 0;

		int32 CurrentItemIndex = GetOwnerComp()->GetItemIndex(this);
		GetOwnerComp()->Server_RemoveItemAtIndex(CurrentItemIndex, 1);
	}
}

float UFHItemBase::GetConditionPercent()
{
	return CurrentCondition / (float)GetItemData().DefaultCondition;
}

float UFHItemBase::GetCoolTime()
{
	if (!GetWorld())
	{
		return 0.0f;
	}
	if (!GetWorld()->GetGameState())
	{
		return 0.0f;
	}
	double currentTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	float currentCoolTime = currentTime - lastUseTime;
	return currentCoolTime / GetItemData().Cooldown;
}

void UFHItemBase::Server_SetEquipped_Implementation(bool bIsEquipped)
{
	UAbilitySystemComponent* AbilitySystem = ASC.Get();
	if (AbilitySystem)
	{
		if (bIsEquipped)
		{
			if (CurrentHoldAbility)
			{
				CurrentHoldAbilitySpecHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(CurrentHoldAbility, 1, 0, this));
				AbilitySystem->TryActivateAbility(CurrentHoldAbilitySpecHandle, true);
				
			}
			if (PrePareUseAbility)
			{
				//int32 InputID = static_cast<int32>(EFHPlayerAbilityInputID::PrePareUseItem);
				int32 InputID = static_cast<int32>(PrePareUseAbility.GetDefaultObject()->AbilityInputID);
				PrePareUseAbilitySpecHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(PrePareUseAbility, 1, InputID, this));
				
			}
			if (UseAbility)
			{
				//int32 InputID = static_cast<int32>(EFHPlayerAbilityInputID::UseItem);
				int32 InputID = static_cast<int32>(UseAbility.GetDefaultObject()->AbilityInputID);
				UseAbilitySpecHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UseAbility, 1, InputID, this));
			}
		}
		else
		{
			if (CurrentHoldAbilitySpecHandle.IsValid())
			{
				AbilitySystem->ClearAbility(CurrentHoldAbilitySpecHandle);
			}
			if (PrePareUseAbilitySpecHandle.IsValid())
			{
				AbilitySystem->ClearAbility(PrePareUseAbilitySpecHandle);
			}
			if (UseAbilitySpecHandle.IsValid())
			{
				AbilitySystem->ClearAbility(UseAbilitySpecHandle);
			}
		}
	}

}
