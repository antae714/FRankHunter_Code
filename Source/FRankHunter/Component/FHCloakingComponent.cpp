// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/FHCloakingComponent.h"
#include "Player/FHPlayerBase.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "GroomComponent.h"

UFHCloakingComponent::UFHCloakingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFHCloakingComponent::Initialize(const TArray<USkeletalMeshComponent*>& InOriginalMeshes, const TArray<USkeletalMeshComponent*>& InTwoPassMeshes, const TArray<UGroomComponent*>& InGrooms)
{
	if (bIsInitialized)
	{
		return;
	}

	OriginalMeshComponents = InOriginalMeshes;
	TwoPassMeshComponents = InTwoPassMeshes;
	GroomComponents = InGrooms;

	/** 머리카락만 추가 로직처리 **/
	for (UGroomComponent* GroomComp : InGrooms)
	{
		if (GroomComp && GroomComp->GetName().Contains(TEXT("Hair")))
		{
			HairGroom = GroomComp;
			break;
		}
	}

	OriginalMIDs.Empty();
	TwoPassMIDs.Empty();

	auto CreateMIDsForMeshes = [&](const TArray<USkeletalMeshComponent*>& Meshes, TArray<class UMaterialInstanceDynamic*>& MIDs)
		{
			for (USkeletalMeshComponent* MeshComp : Meshes) 
			{
				if (MeshComp) 
				{
					for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
					{
						UMaterialInstanceDynamic* MID = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
						if (MID) 
						{
							MIDs.Add(MID);
						}
					}
				}
			}
		};

	CreateMIDsForMeshes(OriginalMeshComponents, OriginalMIDs);
	CreateMIDsForMeshes(TwoPassMeshComponents, TwoPassMIDs);

	for (UGroomComponent* GroomComp : InGrooms)
	{
		if (GroomComp)
		{
			int32 NumMaterials = GroomComp->GetNumMaterials();
			for (int32 i = 0; i < NumMaterials; ++i)
			{
				UMaterialInstanceDynamic* MID = GroomComp->CreateAndSetMaterialInstanceDynamic(i);
				if (MID)
				{
					OriginalMIDs.Add(MID);
				}
			}
		}
	}

	CurrentCloakAmount = CloakAmountMin;
	TargetCloakAmount = CloakAmountMin;
	for (UMaterialInstanceDynamic* MID : OriginalMIDs)
	{
		MID->SetScalarParameterValue(TEXT("CloakingAmount"), CurrentCloakAmount);
	}
	for (UMaterialInstanceDynamic* MID : TwoPassMIDs)
	{
		MID->SetScalarParameterValue(TEXT("CloakingAmount"), CurrentCloakAmount);
	}

	bIsInitialized = true;
}

void UFHCloakingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!FMath::IsNearlyEqual(CurrentCloakAmount, TargetCloakAmount, 0.01f))
	{
		float OldAmount = CurrentCloakAmount;

		CurrentCloakAmount = FMath::FInterpTo(CurrentCloakAmount, TargetCloakAmount, DeltaTime, CloakingInterpSpeed);

		for (UMaterialInstanceDynamic* MID : OriginalMIDs)
		{
			MID->SetScalarParameterValue(TEXT("CloakingAmount"), CurrentCloakAmount);
		}

		for (UMaterialInstanceDynamic* MID : TwoPassMIDs)
		{
			MID->SetScalarParameterValue(TEXT("CloakingAmount"), CurrentCloakAmount);
		}

		if (HairGroom)
		{
			if (TargetCloakAmount == CloakAmountMax && CurrentCloakAmount > 0.98f && HairGroom->IsVisible())
			{
				HairGroom->SetVisibility(false);
			}
			else if (TargetCloakAmount == CloakAmountMin && OldAmount > 0.9f && CurrentCloakAmount <= 0.9f)
			{
				HairGroom->SetVisibility(true);
			}
		}
	}
	else
	{
		CurrentCloakAmount = TargetCloakAmount;

		SetComponentTickEnabled(false);

		if (CurrentCloakAmount == CloakAmountMin)
		{
			if (AFHPlayerBase* OwningPlayer = Cast<AFHPlayerBase>(GetOwner()))
			{
				OwningPlayer->SetTwoPassMeshesVisible(false);
			}

			for (UGroomComponent* GroomComp : GroomComponents) 
			{ 
				GroomComp->SetVisibility(true); 
			}
		}
	}
}

void UFHCloakingComponent::BeginCloaking()
{
	if (!bIsInitialized)
	{
		return;
	}

	if (AFHPlayerBase* OwningPlayer = Cast<AFHPlayerBase>(GetOwner()))
	{
		OwningPlayer->SetTwoPassMeshesVisible(true);
	}

	// 제로에서 1로
	TargetCloakAmount = CloakAmountMax;

	SetComponentTickEnabled(true);
}

void UFHCloakingComponent::EndCloaking()
{
	if (!bIsInitialized)
	{
		return;
	}

	// 1에서 제로로
	TargetCloakAmount = CloakAmountMin;

	SetComponentTickEnabled(true);
}

