// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/Traps/FHChalco.h"
#include "Net/UnrealNetwork.h"
#include "Component/FHInteractableComponent.h"
#include "Player/FHPlayerBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"
#include "GAS/FHGameplayTags.h"

AFHChalco::AFHChalco()
{
	InteractableComponent = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComponent"));
}

void AFHChalco::BeginPlay()
{
	Super::BeginPlay();

}

//void AFHChalco::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//}

void AFHChalco::LockCharacter(ACharacter* Target)
{
	LockedCharacter = Target;
	UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(LockedCharacter->GetMovementComponent());
	MoveComp->SetMovementMode(MOVE_None);

	bIsLocked = true;

	//InteractableComponent->SetIsInteractHoldingEnable(true);
	InteractableComponent->SetIsTooltipUIEnable(true);

	// ======== Play Sound =========
	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		SoundManager->PlaySoundAtLocationMulticastByTag(GET_GAMEPLAY_TAG("Sound.Trap.Active.Leghold"), GetActorLocation());
	}
}

void AFHChalco::ReleaseCharacter()
{
	if (bIsLocked == false || IsValid(LockedCharacter) == false)
	{
		Destroy();
		return;
	}

	UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(LockedCharacter->GetMovementComponent());
	MoveComp->SetMovementMode(MOVE_Walking);

	Destroy();
}