// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHFRankHunterSettings.h"

FName UFHFRankHunterSettings::GetLocalizeStringTableID() const
{
	return LocalizedStringTable.ToSoftObjectPath().GetAssetPath().ToFName();
}
