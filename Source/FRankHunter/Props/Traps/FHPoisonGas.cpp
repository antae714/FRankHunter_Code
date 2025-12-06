// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/Traps/FHPoisonGas.h"
#include "Components/LocalFogVolumeComponent.h"

void AFHPoisonGas::SetDensity(float Value)
{
	ULocalFogVolumeComponent* FogComponent = GetComponentByClass<ULocalFogVolumeComponent>();
	if (FogComponent)
	{
		FogComponent->SetRadialFogExtinction(Value);
	}
}
