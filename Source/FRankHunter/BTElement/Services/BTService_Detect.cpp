// Copyright F Rank Hunter.. All Rights Reserved.


#include "BTElement/Services/BTService_Detect.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"


UBTService_Detect::UBTService_Detect()
{
    DetectedActor.AddObjectFilter(this, NAME_None, UObject::StaticClass());
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
    UpdatePlayerControllers();
	DetectClosestPlayerController(OwnerComp, DeltaSeconds);
}

void UBTService_Detect::UpdatePlayerControllers()
{
    if (PlayerControllers.Num() != GetWorld()->GetNumPlayerControllers())
    {
        PlayerControllers.Empty();

        for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
        {
            APlayerController* PC = Iterator->Get();
            if (PC)
            {
                PlayerControllers.Add(MakeWeakObjectPtr(PC));
            }
        }
    }
}

void UBTService_Detect::DetectClosestPlayerController(UBehaviorTreeComponent& OwnerComp, float DeltaSeconds)
{
    FVector Origin = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
    float DistanceSqrd = FLT_MAX;
    UObject* CurrentDetectedActor = nullptr;

    for (auto& PC : PlayerControllers)
    {
        if (PC.IsValid())
        {
            float TempDistanceSqrd = FVector::DistSquared(PC->GetPawn()->GetActorLocation(), Origin);
            if (DistanceSqrd > TempDistanceSqrd)
            {
                DistanceSqrd = TempDistanceSqrd;
                CurrentDetectedActor = PC->GetPawn();
            }
        }

    }

    float detectRange = DetectRange.GetValue(OwnerComp);
    bool isInRange = DistanceSqrd <= FMath::Square(detectRange);
    if (isInRange)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(DetectedActor.SelectedKeyName, CurrentDetectedActor);
    }
}
