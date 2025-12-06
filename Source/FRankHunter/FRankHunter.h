// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(FH, Log, All);

// 주석처리하면 로그 창에만. 주석 풀면 뷰포트에 그림
//#define LOG_TO_VIEWPORT



// 스텐실 ID 사용 현황
// 100~200 상호작용 강조 아웃라인
// 77 3D 스턴 효과
// 78



#ifdef LOG_TO_VIEWPORT
#include "Engine/Engine.h"
#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_CALLINFO() UE_LOG(FH, Warning, TEXT("%s"), *CALLINFO)
#define PRINT_LOG(fmt, ...) UE_LOG(FH, Warning, TEXT("%s %s"), *CALLINFO, \
	*FString::Printf(fmt, ##__VA_ARGS__)); \
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, \
		FString::Printf(fmt, ##__VA_ARGS__))
#else
#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_CALLINFO() UE_LOG(FH, Warning, TEXT("%s"), *CALLINFO)

#define PRINT_LOG(fmt, ...)\
UE_LOG(FH, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__));



#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))
#endif


#define FH_MARK_PROPERTY_DIRTY(PropertyName) 							\
if (HasAuthority())														\
{																		\
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PropertyName, this);		\
}																		\

DECLARE_LOG_CATEGORY_EXTERN(NetDebug, Log, All);

#define NETENV (GetNetMode() == NM_ListenServer) ? TEXT("Server") : TEXT("Client")
#define NET_DEBUG_LOG(fmt, ...)\
UE_LOG(NetDebug, Warning, TEXT("[%s] %s %s"), NETENV, *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__));

// GAS
UENUM(BlueprintType)
enum class EFHPlayerAbilityInputID : uint8
{
	None			UMETA(DisplayName = "None"),
	Confirm			UMETA(DisplayName = "Confirm"),
	Cancel			UMETA(DisplayName = "Cancel"),
	Attack			UMETA(DisplayName = "Attack"),
	Jump			UMETA(DisplayName = "Jump"),
	Sprint			UMETA(DisplayName = "Sprint"),
	Interact		UMETA(DisplayName = "Interact"),
	DropItem		UMETA(DisplayName = "DropItem"),
	PrePareUseItem	UMETA(DisplayName = "PrePareUseItem"),
	UseItem			UMETA(DisplayName = "UseItem"),
	Crouch			UMETA(DisplayName = "Crouch"),
	UseSkill		UMETA(DisplayName = "UseSkill"),
	ObserveNext		UMETA(DisplayName = "ObserveNext"),
	ObservePrev		UMETA(DisplayName = "ObservePrev"),
	Cloaking		UMETA(DisplayName = "Cloaking"),
	ToggleSpeak		UMETA(DisplayName = "ToggleSpeak"),
	PunchAttack		UMETA(DisplayName = "PunchAttack")
};




class FRankHunterModule : public IModuleInterface
{
public:
	// ~BEGIN IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool IsGameModule() const override
	{
		return true;
	}
};


struct FFRankHunterCustomVersion
{
	enum Type
	{
		// Before any version changes were made in the plugin
		BeforeCustomVersionWasAdded = 0,
		FirstVersion = 1,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FFRankHunterCustomVersion() {}
};

