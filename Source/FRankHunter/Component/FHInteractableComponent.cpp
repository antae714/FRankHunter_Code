// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/FHInteractableComponent.h"
#include "InputAction.h"
#include "Components/StaticMeshComponent.h"
#include "Common/FHRenderingRules.h"
#include "FRankHunter.h"
#include "Net/UnrealNetwork.h"

UFHInteractableComponent::UFHInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	bAutoActivate = true;
	
	bIsInteractEnable = true;
}


void UFHInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bIgnoreThisActorFromInteractCheck)
	{
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}

	if (bGrowSizeOnFloor)
	{
		AActor* Owner = GetOwner();
		Owner->SetActorScale3D(Owner->GetActorScale3D() * GrowSizeScale);
	}
}


void UFHInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsHighlighting)
	{
		ElapsedTimeFromHighlight += DeltaTime;
		if (ElapsedTimeFromHighlight >= CurrentHighlightTime)
		{
			Dehighlight();
		}
		else
		{
			WriteOpacity();
		}
	}
}

void UFHInteractableComponent::SetIsInteractEnable(bool NewIsInteractEnable)
{
	bIsInteractEnable = NewIsInteractEnable;
}

void UFHInteractableComponent::SetIsInteractHoldingEnable(bool NewbIsInteractHoldingEnable)
{
	bIsInteractHoldingEnable = NewbIsInteractHoldingEnable;
}

void UFHInteractableComponent::SetIsTooltipUIEnable(bool NewIsTooltipUIEnable)
{
	bBlockTooltipUI = !NewIsTooltipUIEnable;
}

bool UFHInteractableComponent::IsInteractEnable(const UInputAction* FilterInputAction) const
{
	return bIsInteractEnable && FilterInputAction && FilterInputActons.Contains(FilterInputAction);
}

bool UFHInteractableComponent::IsInteractHoldingEnable(const UInputAction* FilterInputAction) const
{
	return bIsInteractHoldingEnable && FilterInputAction && FilterInputActons.Contains(FilterInputAction);
}

bool UFHInteractableComponent::IsBlockHoldingReleaseInteract() const
{
	return bBlockHoldingReleaseInteract;
}

bool UFHInteractableComponent::IsBlockTooltipUI() const
{
	return bBlockTooltipUI;
}

float UFHInteractableComponent::GetHoldDurationForHoldingInteract() const
{
	return HoldDurationForHoldingInteract;
}

float UFHInteractableComponent::GetInteractDistance() const
{
	return InteractDistance;
}

TObjectPtr<UFHInteractTooltipWidget> UFHInteractableComponent::GetTooltipUIWidget() const
{
	return TooltipUIWidget;
}

const TArray<FText>& UFHInteractableComponent::GetTooltipText() const
{
	return TooltipTextArray;
}

void UFHInteractableComponent::SetFirstTooltipText(FText text)
{
	if (TooltipTextArray.IsEmpty())
	{
		TooltipTextArray.Add(text);
	}
	else
	{
		TooltipTextArray[0] = text;
	}
	bNeedUpdateTooltipText = true;
}

const TArray<TObjectPtr<UInputAction>>& UFHInteractableComponent::GetFilterInputActons() const
{
	return FilterInputActons;
}

void UFHInteractableComponent::Highlight(float Time)
{
	if (bIsHighlightable == false)
	{
		return;
	}

	HighlightTargets.Empty();
	GetOwner()->GetComponents<UStaticMeshComponent>(HighlightTargets);

	if (HighlightTargets.IsEmpty()) return;

	ElapsedTimeFromHighlight = 0.0f;
	CurrentHighlightTime = Time;
	bIsHighlighting = true;

	for (auto* mesh : HighlightTargets)
	{
		mesh->SetRenderCustomDepth(true);
		mesh->SetCustomDepthStencilValue(100);
	}
}

void UFHInteractableComponent::Dehighlight()
{
	bIsHighlighting = false;

	if (HighlightTargets.IsEmpty()) return;

	for (auto* mesh : HighlightTargets)
	{
		mesh->SetRenderCustomDepth(false);
	}
}

void UFHInteractableComponent::WriteOpacity()
{
	if (HighlightTargets.IsEmpty()) return;
	
	float NewOpacity = 0.0f;
	float t = (CurrentHighlightTime - ElapsedTimeFromHighlight) / CurrentHighlightTime;
	if (t > 1.0f) t = 1.0f;

	if (t < 0.25f)
	{
		NewOpacity =  t / 0.25f;
	}
	else if (t < 0.75f)
	{
		NewOpacity =  1.0f;
	}
	else if (t <= 1.0f)
	{
		NewOpacity = (1.0f - t) / 0.25f;
	}
	else
	{
		NewOpacity = 0.0f;
	}

	for (auto* mesh : HighlightTargets)
	{
		mesh->SetCustomDepthStencilValue(static_cast<int32>(100.0f + (100.0f * NewOpacity)));
	}
}

void UFHInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFHInteractableComponent, bIsHighlightable);
}

