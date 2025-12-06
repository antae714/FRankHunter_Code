// Copyright F Rank Hunter. All Rights Reserved.


#include "DataAsset/FHLevelAsset.h"

TSoftObjectPtr<UWorld> UFHLevelAsset::GetTitleMap()
{
	return TitleMap;
}

TSoftObjectPtr<UWorld> UFHLevelAsset::GetStartLobbyMap()
{
	return MorningLobbyMap;
}

TSoftObjectPtr<UWorld> UFHLevelAsset::GetEndLobbyMap()
{
	return NightLobbyMap;
}

TSoftObjectPtr<UWorld> UFHLevelAsset::GetGateMap()
{
	return GateMap;
}

TSoftObjectPtr<UWorld> UFHLevelAsset::GetCreditMap()
{
	return CreditMap;
}
