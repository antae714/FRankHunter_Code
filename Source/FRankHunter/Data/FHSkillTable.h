// Copyright F Rank Hunter.. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "FHSkillTable.generated.h"


UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Active,
	Passive,
	Toggle,
};

USTRUCT(BlueprintType)
struct FFHSkillTable_Tinker : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "내구도 감소없이 아이템을 사용하게 될 확률 (레벨 10미만)"))
	float Default{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "내구도 감소없이 아이템을 사용하게 될 확률 (레벨 10이상 ~ 20미만)"))
	float Lv10{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "내구도 감소없이 아이템을 사용하게 될 확률 (레벨 20이상 ~ 30미만)"))
	float Lv20{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "내구도 감소없이 아이템을 사용하게 될 확률 (레벨 30이상)"))
	float Lv30{};

	float GetValueByLevel(int32 CurrentLevel) const
	{
		if (CurrentLevel >= 30) return Lv30;
		if (CurrentLevel >= 20) return Lv20;
		if (CurrentLevel >= 10) return Lv10;
		return Default;
	}
};

USTRUCT(BlueprintType)
struct FFHSkillTable_Breaker : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 10미만)"))
	int32 Default{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 10이상 ~ 20미만)"))
	int32 Lv10{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 20이상 ~ 30미만)"))
	int32 Lv20{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 30이상)"))
	int32 Lv30{};

	int32 GetValueByLevel(int32 CurrentLevel) const
	{
		if (CurrentLevel >= 30) return Lv30;
		if (CurrentLevel >= 20) return Lv20;
		if (CurrentLevel >= 10) return Lv10;
		return Default;
	}
};

USTRUCT(BlueprintType)
struct FFHSkillTable_Balancer : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 10미만)"))
	int32 Default{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 10이상 ~ 20미만)"))
	int32 Lv10{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 20이상 ~ 30미만)"))
	int32 Lv20{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (Tooltip = "기본 스탯에 대한 보너스 스탯값 (레벨 30이상)"))
	int32 Lv30{};

	int32 GetValueByLevel(int32 CurrentLevel) const
	{
		if (CurrentLevel >= 30) return Lv30;
		if (CurrentLevel >= 20) return Lv20;
		if (CurrentLevel >= 10) return Lv10;
		return Default;
	}
};

USTRUCT(BlueprintType)
struct FFHSkillTable_ActiveBase : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Description", meta = (Tooltip = "설명"))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Default{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Lv10{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Lv20{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Lv30{};

	float GetValueByLevel(int32 CurrentLevel) const
	{
		if (CurrentLevel >= 30) return Lv30;
		if (CurrentLevel >= 20) return Lv20;
		if (CurrentLevel >= 10) return Lv10;
		return Default;
	}
};

USTRUCT(BlueprintType)
struct FFHSkillTable_Alchemist : public FFHSkillTable_ActiveBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FName ItemID;
};

USTRUCT(BlueprintType)
struct FFHSkillTable_Breather: public FFHSkillTable_ActiveBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FFHSkillTable_Seeker : public FFHSkillTable_ActiveBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FFHSkillTable_Stopper: public FFHSkillTable_ActiveBase
{
	GENERATED_BODY()
};


//=============== SkillTable ===============//
USTRUCT(BlueprintType)
struct FFHSkillTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	TSubclassOf<class UGameplayAbility> SkillClass;
};

USTRUCT(BlueprintType)
struct FFHSkillInfoTable: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName SkillName;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	ESkillType SkillType = ESkillType::Active;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	FString SkillDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FText SkillTooltip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	TSoftObjectPtr<UTexture2D> SkillIcon;
};