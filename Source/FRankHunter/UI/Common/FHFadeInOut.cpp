// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Common/FHFadeInOut.h"

void UFHFadeInOut::PlayFadeIn()
{
	BP_PlayFadeIn();
}

void UFHFadeInOut::PlayFadeOut()
{
	BP_PlayFadeOut();
}

void UFHFadeInOut::OnFadeInEnd()
{
	OnFadeInEndDelegate.Broadcast();
	OnFadeInEndDelegate.Clear();

	OnFadeEndDelegate.ExecuteIfBound();
	OnFadeEndDelegate.Unbind();

}

void UFHFadeInOut::OnFadeOutEnd()
{
	OnFadeOutEndDelegate.Broadcast();
	OnFadeOutEndDelegate.Clear();

	OnFadeEndDelegate.ExecuteIfBound();
	OnFadeEndDelegate.Unbind();
}
