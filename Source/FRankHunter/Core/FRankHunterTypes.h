#pragma once
#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "FRankHunterTypes.generated.h"

class AFHPlayerStateBase;


UENUM(BlueprintType)
enum class EBuildType : uint8
{
	Default		UMETA(DisplayName = "Default"),
	GStar		UMETA(DisplayName = "GStar"),
};

UENUM(BlueprintType)
enum class EPlayerGateState : uint8
{
	Alive,
	Died,
	Escape,
	Undefined
};

UENUM(BlueprintType)
enum class ELicenseRank : uint8
{
	None,
	F UMETA(DisplayName="F"),
	E UMETA(DisplayName="E"),
	D UMETA(DisplayName="D"),
	C UMETA(DisplayName="C"),
	B UMETA(DisplayName="B"),
	A UMETA(DisplayName="A"),
	S UMETA(DisplayName="S")
};

USTRUCT(BlueprintType)
struct FPlayerInfo : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AFHPlayerStateBase> PlayerState;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EPlayerGateState PlayerGateState = EPlayerGateState::Undefined;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float MiningAmount = 0.0f;
};

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE(FOnFadeEnd);