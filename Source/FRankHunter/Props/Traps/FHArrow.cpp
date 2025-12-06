// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/Traps/FHArrow.h"
#include "Components/StaticMeshComponent.h"
#include "FHArrowTrap.h"

AFHArrow::AFHArrow() :
	MoveRange(2000.0f),
	TotalMoveRange(2000.0f),
	MoveDir(FVector(0, 0, 0))
{

}

void AFHArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving)
	{
		return;
	}
}

void AFHArrow::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);

	if (StuddedTimeHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(StuddedTimeHandle);
	}
}

void AFHArrow::OnBeginOverlap(AActor* Actor)
{
	if (!Cast<AFHArrowTrap>(Actor))
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(StuddedTimeHandle, 
		FTimerDelegate::CreateLambda([this]()
	{
		Destroy();
	}),
		20.0f, false);

	MoveDir = FVector::ZeroVector;
	bIsMoving = false;
}
