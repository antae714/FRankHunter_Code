// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/Tuple.h"
#include "ManagerActor.h"
#include "FHUIManager.generated.h"

class UUserWidget;

USTRUCT(BlueprintType)
struct FWidgetDescript
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WidgtKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ViewportOrder{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsAutoColapsed{ true };
};

UCLASS()
class FRANKHUNTER_API AFHUIManager : public AManagerActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFHUIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual FName GetManagerName() const { return TEXT("UIManager"); }

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	bool RegistWidget(UUserWidget* Widget, FName WidgetKey);

	template <class T>
	T* GetWidget(FName Key)
	{
		if (UUserWidget** WidgetPtr = UIWidgets.Find(Key))
		{
			return Cast<T>(*WidgetPtr);
		}
		return nullptr;
	}

	template <>
	UUserWidget* GetWidget(FName Key)
	{
		if (UUserWidget** WidgetPtr = UIWidgets.Find(Key))
		{
			return *WidgetPtr;
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "UIManager")
	UUserWidget* GetWidgetByKey(FName Key)
	{
		return GetWidget<UUserWidget>(Key);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIContainer")
	TArray<FWidgetDescript> WidgetClasses;

	UPROPERTY()
	TMap<FName, UUserWidget*> UIWidgets;
};
