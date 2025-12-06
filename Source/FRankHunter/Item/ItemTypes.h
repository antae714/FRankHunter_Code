// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DungeonGeneration/FHGateDefines.h"
#include "Common/CommonItemEnum.h"
#include "ItemTypes.generated.h"

// 해당 헤더에서 정의된 목록
enum class EItemType : uint8;
enum class EItemRarity : uint8;
struct FFHItemData;
struct FDropItemData;
struct FFHItemDrobTableRow;
struct FFHManaStoneDropTableRow;



UENUM(BlueprintType)
enum class EItemType : uint8
{
    None,
    Tool,
    Consume,
    Throwable,
    Deployable,
    Consumable,
    Resource,
    ManaStone,
    BackPack,
};


UENUM(BlueprintType)
enum class EItemRarity :uint8
{
    Common,
    UnCommon,
    Rare,
    Epic,
    Legendary,
    Mythic
};

USTRUCT(BlueprintType)
struct FItemRarityColorArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor CommonColor{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor UncommonColor{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor RareColor{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor EpicColor{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor LegendaryColor{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor MythicColor{};


    FLinearColor GetColor(EItemRarity ItemRarity) const;
};


USTRUCT(BlueprintType)
struct FFHItemData : public FTableRowBase
{
    GENERATED_BODY()
    virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override;
    virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
    void OnChangeItem(const FName InRowName);
    const bool IsAddToShop() const { return MaxBuyCount == 0; }

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Identification")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identification")
    EItemType Type{ EItemType::None };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    EItemHoldingType HoldingType{ EItemHoldingType::NoHanded };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    EItemHoldingSocketDirection FPHoldingDirection{ EItemHoldingSocketDirection::Nowhere };
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    EItemHoldingSocketDirection TPHoldingDirection{ EItemHoldingSocketDirection::Nowhere };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FName ItemNameID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identification")
    EItemRarity Rarity{ EItemRarity::Common };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FName DescriptionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FName IconID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 ReqLevel{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MaxStack{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Weight{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Cooldown{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 ConditionCost{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 DefaultCondition{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 SellPrice{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 BuyPrice{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FName DropEffectID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FName SelectSoundID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FName DropSoundID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FName MeshID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbillitySysten")
    FName PassiveEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbillitySysten")
    FName CurrentHoldAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbillitySysten")
    FName PrePareUseAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbillitySysten")
    FName UseAbility;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbillitySysten")
    bool bHasUsingMotion{ false };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuyCount")
    int32 MaxBuyCount{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    FVector TPEquipOffset{};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    FRotator TPEquipRotation{};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    FVector FPEquipOffset{};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    FRotator FPEquipRotation{};
};


USTRUCT(BlueprintType)
struct FFHThrowItemData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbillitySysten")
    FName ImpactAbilityID;

};

USTRUCT(BlueprintType)
struct FFHDeployItemData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbillitySysten")
    FName DeployAbilityID;
};






USTRUCT(BlueprintType)
struct FDropItemData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxPerGate = TNumericLimits<int32>::Max();
};

USTRUCT(BlueprintType)
struct FFHItemDrobTableRow : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_E{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxPerGate_E{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_D{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxPerGate_D{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_C{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxPerGate_C{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_B{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxPerGate_B{ 0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_A{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxPerGate_A{ 0 };

    float GetWeight(EGateRank GateGrade) const;
	int32 GetMaxPerGate(EGateRank GateGrade) const;
};

USTRUCT(BlueprintType)
struct FFHManaStoneDropTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_E{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_D{ 0.0f };
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_C{ 0.0f };
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_B{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_A{ 0.0f };

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Weight_S{ 0.0f };


    float GetWeight(EGateRank GateGrade) const;
};
