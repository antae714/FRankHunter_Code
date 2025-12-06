// Copyright F Rank Hunter. All Rights Reserved.


#include "Effects/FHSimpleCurveCameraShakePattern.h"

UFHSimpleCurveCameraShakePattern::UFHSimpleCurveCameraShakePattern(const FObjectInitializer& ObjInit) : Super(ObjInit)
{

}

void UFHSimpleCurveCameraShakePattern::StartShakePatternImpl(const FCameraShakePatternStartParams& Params)
{
	Super::StartShakePatternImpl(Params);

	if (!Params.bIsRestarting)
	{
		ElapsedTime = 0.0f;
		float MinTime, MaxTime{ 0.0f };

		if (bCompressCurveIntoDuration)
		{
			if (RotationPitchCurve)
			{
				RotationPitchCurve->GetTimeRange(MinTime, MaxTime);
				TimeCompressionFactor_Pitch = MaxTime / Duration;
			}
			if (RotationYawCurve)
			{
				RotationYawCurve->GetTimeRange(MinTime, MaxTime);
				TimeCompressionFactor_Yaw = MaxTime / Duration;
			}
			if (RotationRollCurve)
			{
				RotationRollCurve->GetTimeRange(MinTime, MaxTime);
				TimeCompressionFactor_Roll = MaxTime / Duration;
			}
		}
	}
}

void UFHSimpleCurveCameraShakePattern::UpdateShakePatternImpl(const FCameraShakePatternUpdateParams& Params, FCameraShakePatternUpdateResult& OutResult)
{
	UpdateSimpleCurve(Params.DeltaTime, OutResult);

	const float BlendWeight = State.Update(Params.DeltaTime);
	OutResult.ApplyScale(BlendWeight);
}

void UFHSimpleCurveCameraShakePattern::ScrubShakePatternImpl(const FCameraShakePatternScrubParams& Params, FCameraShakePatternUpdateResult& OutResult)
{
	ElapsedTime = 0.0f;

	UpdateSimpleCurve(Params.AbsoluteTime, OutResult);

	const float BlendWeight = State.Scrub(Params.AbsoluteTime);
	OutResult.ApplyScale(BlendWeight);
}

void UFHSimpleCurveCameraShakePattern::UpdateSimpleCurve(float DeltaTime, FCameraShakePatternUpdateResult& OutResult)
{
	ElapsedTime += DeltaTime;

	if (bCompressCurveIntoDuration)
	{
		if (RotationPitchCurve)
		{
			OutResult.Rotation.Pitch = RotationPitchCurve->GetFloatValue(ElapsedTime * TimeCompressionFactor_Pitch) * RotationMagnitudeMultiplier;
		}
		if (RotationYawCurve)
		{
			OutResult.Rotation.Yaw = RotationYawCurve->GetFloatValue(ElapsedTime * TimeCompressionFactor_Yaw) * RotationMagnitudeMultiplier;
		}
		if (RotationRollCurve)
		{
			OutResult.Rotation.Roll = RotationRollCurve->GetFloatValue(ElapsedTime * TimeCompressionFactor_Roll) * RotationMagnitudeMultiplier;
		}
	}
	else
	{
		if (RotationPitchCurve)
		{
			OutResult.Rotation.Pitch = RotationPitchCurve->GetFloatValue(ElapsedTime) * RotationMagnitudeMultiplier;
		}
		if (RotationYawCurve)
		{
			OutResult.Rotation.Yaw = RotationYawCurve->GetFloatValue(ElapsedTime) * RotationMagnitudeMultiplier;
		}
		if (RotationRollCurve)
		{
			OutResult.Rotation.Roll = RotationRollCurve->GetFloatValue(ElapsedTime) * RotationMagnitudeMultiplier;
		}
	}
}
