// Copyright F Rank Hunter. All Rights Reserved.


#include "BTElement/Services/BTService_AttributeSetBlackBoard.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"

void UBTService_AttributeSetBlackBoard::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds) ;

	APawn* Pawn = Cast<APawn>(OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr);
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (!ASC || !TargetAttribute.IsValid())
	{
		return;
	}

	float Value = ASC->GetNumericAttribute(TargetAttribute);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsFloat(BlackboardKey.SelectedKeyName, Value);
	}
}
