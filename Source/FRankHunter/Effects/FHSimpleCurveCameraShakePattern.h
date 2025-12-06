// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Shakes/SimpleCameraShakePattern.h"
#include "FHSimpleCurveCameraShakePattern.generated.h"


UCLASS()
class FRANKHUNTER_API UFHSimpleCurveCameraShakePattern : public USimpleCameraShakePattern
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SimpleCurve")
	uint32 bCompressCurveIntoDuration : 1{ true };
	
	UPROPERTY(EditAnywhere, Category = "SimpleCurve")
	float RotationMagnitudeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, Category = "SimpleCurve")
	TObjectPtr<UCurveFloat> RotationPitchCurve;
	
	UPROPERTY(EditAnywhere, Category = "SimpleCurve")
	TObjectPtr<UCurveFloat> RotationYawCurve;
	
	UPROPERTY(EditAnywhere, Category = "SimpleCurve")
	TObjectPtr<UCurveFloat> RotationRollCurve;

private:
	float ElapsedTime{ 0.0f };

	float TimeCompressionFactor_Pitch{ 0.0f };
	float TimeCompressionFactor_Yaw{ 0.0f };
	float TimeCompressionFactor_Roll{ 0.0f };

public:
	UFHSimpleCurveCameraShakePattern(const FObjectInitializer& ObjInit);
private:
	virtual void StartShakePatternImpl(const FCameraShakePatternStartParams& Params) override;
	virtual void UpdateShakePatternImpl(const FCameraShakePatternUpdateParams& Params, FCameraShakePatternUpdateResult& OutResult) override;
	virtual void ScrubShakePatternImpl(const FCameraShakePatternScrubParams& Params, FCameraShakePatternUpdateResult& OutResult) override;

	void UpdateSimpleCurve(float DeltaTime, FCameraShakePatternUpdateResult& OutResult);
};
