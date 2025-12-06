// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGA_Skill_Alchemy.h"
#include "Item/FHInventoryComponent.h"
#include "Player/FHPlayerBase.h"
#include "Core/FHPlayerController.h"
#include "Core/FHPlayerStateBase.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/Actors/FHPickupItemActor.h"
#include "FRankHunter.h"

UFHGA_Skill_Alchemy::UFHGA_Skill_Alchemy()
{
	SkillType = ESkillType::Active;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	bHasAnimation = true;
	bWaitForMontageEnd = true;
}

void UFHGA_Skill_Alchemy::ExecuteActiveAbility()
{
	AFHPlayerBase* Pawn = Cast<AFHPlayerBase>(GetCurrentActorInfo()->AvatarActor.Get());
	if (!Pawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC)
	{
		return;
	}

	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthority(&ActivationInfo))
	{
		UDataTable* LoadedTable = SkillTable.LoadSynchronous();

		AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
		const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

		struct FPickInfo
		{
			FName ItemID{};
			float Weight{};
		};
		TArray<FPickInfo> ItemPickWeight;
		float TotalWeight = 0.f;

		for (const TPair<FName, uint8*>& RowPair : LoadedTable->GetRowMap())
		{
			const FName RowName = RowPair.Key;
			const FFHSkillTable_Alchemist* RowData = reinterpret_cast<FFHSkillTable_Alchemist*>(RowPair.Value);

			if (!RowData)
				continue;

			if (RowName.ToString().StartsWith(TEXT("Weight")))
			{
				const float CurrentWeight = RowData->GetValueByLevel(CurrentLevel);
				if (CurrentWeight > 0)
				{
					TotalWeight += CurrentWeight;
					ItemPickWeight.Add({ RowData->ItemID, CurrentWeight });
				}
			}
		}

		if (TotalWeight <= 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Alchemist: Cannot Pick Item Because The Totalweight is Zero."));
			return;
		}

		const float RandomValue = FMath::FRandRange(0.f, TotalWeight);
		FName PickedItemID = NAME_None;
		float AccumulatedWeight = 0.f;

		for (const auto& PickWeight : ItemPickWeight)
		{
			AccumulatedWeight += PickWeight.Weight;
			if (RandomValue <= AccumulatedWeight)
			{
				PickedItemID = PickWeight.ItemID;
				break;
			}
		}

		if (!PickedItemID.IsNone())
		{
			UFHItemClassDataAsset* ItemAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
			if (ItemAsset)
			{
				PS->Client_OnSkillFeedback(true);

				UE_LOG(LogTemp, Log, TEXT("Alchemist Skill Activated. Picked Item : '%s' (RandomValue: %f, TotalWeight: %f)"), *PickedItemID.ToString(), RandomValue, TotalWeight);

				FVector CameraLocation;
				FRotator CameraRotation;
				PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

				const FVector TraceStart = CameraLocation;
				const FVector TraceEnd = TraceStart + (CameraRotation.Vector() * ItemSpawnDistance);

				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(Pawn);

				FVector SpawnLocation = TraceEnd;

				if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
				{
					// 벽에 맞은 경우 
					SpawnLocation = HitResult.Location - (CameraRotation.Vector() * 5.0f);
				}

				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = Pawn;
				SpawnParams.Instigator = Pawn;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				/* 아므나엘의 턴 */
				// 연금가마 카오스디스틸 소환 -> 카오스디스틸을 제외하고 함정카드 매크로 코스모스 발동 -> 원시태양 헬리오스 소환
				AFHPickupItemActor* RenkinKamaChaosDistill = GetWorld()->SpawnActor<AFHPickupItemActor>(ChaosDistill, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
				if (RenkinKamaChaosDistill)
				{
					UFHInventoryComponent* MacroCosmos = RenkinKamaChaosDistill->GetInventoryComponent();
					if (MacroCosmos)
					{
						TSubclassOf<UFHItemBase> GenshiTaiyoHelios = ItemAsset->GetItemClass(PickedItemID);
						if (GenshiTaiyoHelios)
						{
							MacroCosmos->Server_AddItem(GenshiTaiyoHelios, 1);
							Pawn->Multicast_PlayAlchemistSkillEffect(SpawnLocation);
						}
					}
				}
			}
		}
	}
}