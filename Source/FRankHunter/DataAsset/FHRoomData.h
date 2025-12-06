// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RoomData.h"
#include "FHRoomData.generated.h"

USTRUCT()
struct FFHCustomRoomConnectionSetting
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UFHRoomData> OtherRoom;
	UPROPERTY(EditAnywhere)
	float Weight = 0.0f;
};

USTRUCT()
struct FFHCustomMinimumDistanceRule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UFHRoomData> TargetRoom;
	UPROPERTY(EditAnywhere)
	float Distance = 0.0f;
};

UCLASS()
class FRANKHUNTER_API UFHRoomData : public URoomData
{
	GENERATED_BODY()
	
public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MinimapConfig")
	//FColor FillColor{ 255, 200, 200, 255 };
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MinimapConfig")
	//FColor BorderColor{ 165, 60, 60, 255 };
	
	// Each index represents a floor. (ex: index 0 is floor 1)
	UPROPERTY(EditAnywhere, Category = "MinimapConfig")
	TArray<TObjectPtr<UTexture2D>> MinimapImages;

	UPROPERTY(EditAnywhere, Category = "MinimapConfig")
	TArray<TObjectPtr<UTexture2D>> RoomIconImages;




	UPROPERTY(EditAnywhere, Category = "Rules|CustomConnection")
	uint32 bUseCustomConnectionSetting : 1{ false };
	UPROPERTY(EditAnywhere, Category = "Rules|CustomConnection", meta = (EditCondition = "bUseCustomConnectionSetting == true", EditConditionHides))
	TArray<FFHCustomRoomConnectionSetting> CustomConnectionSetting;
	// If true, only rules specified by CustomRules will be used. If false, other rooms not defined by CustomRules will be included in the candidates with a own default weight.
	UPROPERTY(EditAnywhere, Category = "Rules|CustomConnection", meta = (EditCondition = "bUseCustomConnectionSetting == true", EditConditionHides))
	uint32 bUseOnlyCustomConnectionRule : 1{ false };
	
	// This room can only be placed when it is at least as far away as the distance specified from target room in the following list.
	UPROPERTY(EditAnywhere, Category = "Rules|CustomConnection")
	uint32 bUseMinimumDistanceRule : 1{ false };
	UPROPERTY(EditAnywhere, Category = "Rules|CustomConnection", meta = (EditCondition = "bUseMinimumDistanceRule == true", EditConditionHides))
	TArray<FFHCustomMinimumDistanceRule> MinimumDistanceRules;


	
	UPROPERTY(EditAnywhere, Category = "Rules")
	uint32 bUseMinProgressRateConstraint : 1{ false };
	UPROPERTY(EditAnywhere, Category = "Rules", meta = (EditCondition = "bUseMinProgressRateConstraint == true", EditConditionHides))
	float MinimumProgressRate{ 0.0f };
	UPROPERTY(EditAnywhere, Category = "Rules")
	uint32 bUseMaxProgressRateConstraint : 1{ false };
	UPROPERTY(EditAnywhere, Category = "Rules", meta = (EditCondition = "bUseMaxProgressRateConstraint == true", EditConditionHides))
	float MaximumProgressRate{ 1.0f };

	UPROPERTY(EditAnywhere, Category = "Rules")
	uint32 bUseMinDistanceFromStartRoom : 1{ false };
	UPROPERTY(EditAnywhere, Category = "Rules", meta = (EditCondition = "bUseMinDistanceFromStartRoom == true", EditConditionHides))
	float MinimumDistanceFromStartRoom{ 0.0f };
	UPROPERTY(EditAnywhere, Category = "Rules")
	uint32 bUseMaxDistanceFromStartRoom : 1{ false };
	UPROPERTY(EditAnywhere, Category = "Rules", meta = (EditCondition = "bUseMaxDistanceFromStartRoom == true", EditConditionHides))
	float MaximumDistanceFromStartRoom{ 0.0f };


	UPROPERTY(EditAnywhere)
	uint32 bIsCoreRoom : 1{ false };
	UPROPERTY(EditAnywhere)
	float DefaultWeight{ 1.0f };

	// obsolete / Use Special Rooms bUseRandomRange setting.
	//UPROPERTY(EditAnywhere, Category = "Rules")
	//uint32 bSetMaximumRoomNum : 1{ false };
	//UPROPERTY(EditAnywhere, Category = "Rules", meta = (EditCondition = "bSetMaximumRoomNum == true", EditConditionHides))
	//uint32 MaximumRoomNum{ 1 };

	bool CheckConnectable(const UFHRoomData* other) const;
	bool CheckProgressRate(float dungeonGenProgressRate) const;
	bool CheckRoomNum(uint32 roomNum) const;
	bool CheckDistanceFromStartRoom(FVector RoomUnit, FVector InitRoomPosCenter, const struct FDoorDef& DoorData) const;
};
