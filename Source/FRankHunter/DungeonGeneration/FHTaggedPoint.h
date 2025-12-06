// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "FHTaggedPoint.generated.h"


// ============ obsolete ==============

//UENUM(BlueprintType, meta = (Bitflags))
//enum class ETaggedPointRole : uint8
//{
//	None = 0 UMETA(hidden),
//	SpawnCreature = 1 << 0,
//	SpawnMagicStone = 1 << 1,
//	SpawnCore = 1 << 2
//};
//ENUM_CLASS_FLAGS(ETaggedPointRole)
//
//UCLASS()
//class FRANKHUNTER_API AFHTaggedPoint : public AActor
//{
//	GENERATED_BODY()
//	
//public:
//	UPROPERTY(EditAnywhere, Category = "PointSettings", meta = (Bitmask, BitmaskEnum = "ETaggedPointRole"))
//	int32 RoleFlag;
//
//	UPROPERTY(EditAnywhere, Category = "PointSettings")
//	uint32 bEnableRandomRotation : 1{ false };
//	UPROPERTY(EditAnywhere, Category = "PointSettings")
//	uint32 bEnableRandomLocationInRadius : 1{ false };
//	UPROPERTY(EditAnywhere, Category = "PointSettings", meta = (EditCondition = "bEnableRandomLocationInRadius == true", EditConditionHides))
//	float RandomLocationRadius{ 100.0f };
//
//
//public:
//	AFHTaggedPoint();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//};
