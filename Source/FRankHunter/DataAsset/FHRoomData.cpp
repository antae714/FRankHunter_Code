// Copyright F Rank Hunter. All Rights Reserved.


#include "DataAsset/FHRoomData.h"
#include "Room.h"

bool UFHRoomData::CheckConnectable(const UFHRoomData* other) const
{
	if (!bUseCustomConnectionSetting) return true;

	for (const auto& setting : CustomConnectionSetting)
	{
		if (setting.OtherRoom.Get() == other && setting.Weight <= 0.0f)
			return false;
	}

	return true;
}

bool UFHRoomData::CheckProgressRate(float dungenGenProgressRate) const
{
	if (bUseMinProgressRateConstraint)
	{
		if (dungenGenProgressRate < MinimumProgressRate)
			return false;
	}

	if (bUseMaxProgressRateConstraint)
	{
		if (dungenGenProgressRate > MaximumProgressRate)
			return false;
	}

	return true;
}

bool UFHRoomData::CheckRoomNum(uint32 roomNum) const
{
	return true;

	//if (bSetMaximumRoomNum)
	//{
	//	if (roomNum > MaximumRoomNum)
	//		return false;
	//}

	//return true;
}

bool UFHRoomData::CheckDistanceFromStartRoom(FVector RoomUnit, FVector InitRoomPosCenter, const FDoorDef& DoorData) const
{
	if (bUseMinDistanceFromStartRoom == false && bUseMaxDistanceFromStartRoom == false)
		return true;

	FVector doorPos = FDoorDef::GetRealDoorPosition(RoomUnit, DoorData);
	float distance = static_cast<float>(FVector::Distance(doorPos, InitRoomPosCenter));

	if (bUseMinDistanceFromStartRoom)
	{
		if (distance >= MinimumDistanceFromStartRoom)
			return true;
		else
			return false;
	}
	if (bUseMaxDistanceFromStartRoom)
	{
		if (distance < MaximumDistanceFromStartRoom)
			return true;
		else
			return false;
	}

	return true;
}
