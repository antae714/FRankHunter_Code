// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/FHItemDropPointComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void UFHItemDropPointSocketComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HandleComponent)
	{
		PointNameArray = GetPointNames();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleComponent is null"));
	}
	CurrentIndex = 0;
}

FVector UFHItemDropPointSocketComponent::PopNextPoint_Implementation()
{
	if (PointNameArray.Num() == 0)
	{
		PointNameArray = GetPointNames();
		if (PointNameArray.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("CachedPoints is empty"));
			return GetOwner()->GetActorLocation();
		}
	}
	if (!HandleComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HandleComponent is null"));
	}
	if (!PointNameArray.IsValidIndex(CurrentIndex))
	{
		CurrentIndex = 0;
	}
	return HandleComponent->GetSocketLocation(PointNameArray[CurrentIndex++]);
}

FVector UFHItemDropPointSocketComponent::PeekPoint_Implementation()
{
	if (PointNameArray.IsValidIndex(CurrentIndex))
	{
		return HandleComponent->GetSocketLocation(PointNameArray[CurrentIndex]);
	}
	else
	{
		return GetOwner()->GetActorLocation();
	}
}

TArray<FName> UFHItemDropPointSocketComponent::GetPointNames()
{
	TArray<FName> SocketNames = HandleComponent->GetAllSocketNames();
	TArray<FName> pointSocketArray;

	for (auto& SocketName : SocketNames)
	{
		if (SocketName.ToString().Contains(TEXT("ItemDropPoint")))
		{
			pointSocketArray.Add(SocketName);
		}
	}
	return pointSocketArray;
}


void UFHItemDropPointMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

}

FVector UFHItemDropPointMarkerComponent::PopNextPoint_Implementation()
{
	if (GetNumChildrenComponents() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedPoints is empty"));
		return GetComponentLocation();
	}
	if (!GetChildComponent(CurrentIndex))
	{
		CurrentIndex = 0;
	}
	if (GetChildComponent(CurrentIndex))
	{
		return GetChildComponent(CurrentIndex++)->GetComponentLocation();
	}
	else
	{
		return GetComponentLocation();
	}
}

FVector UFHItemDropPointMarkerComponent::PeekPoint_Implementation()
{
	if (GetChildComponent(CurrentIndex))
	{
		return GetChildComponent(CurrentIndex)->GetComponentLocation();
	}
	else
	{
		return GetComponentLocation();
	}
}

FVector UFHItemDropPoinManualComponent::PopNextPoint_Implementation()
{
	if (ManualPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedPoints is empty"));
		return GetOwner()->GetActorLocation();
	}
	if (!ManualPoints.IsValidIndex(CurrentIndex))
	{
		CurrentIndex = 0;
	}
	return ManualPoints[CurrentIndex++];
}

FVector UFHItemDropPoinManualComponent::PeekPoint_Implementation()
{
	if (!ManualPoints.IsValidIndex(CurrentIndex))
	{
		return ManualPoints[CurrentIndex];
	}
	else
	{
		return GetOwner()->GetActorLocation();
	}
}
