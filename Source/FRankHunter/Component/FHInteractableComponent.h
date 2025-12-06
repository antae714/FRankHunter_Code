// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FHInteractableComponent.generated.h"

class UInputAction;
class UFHInteractTooltipWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractDelegate, AFHPlayerBase*, Player, const UInputAction*, InputAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractForDelegate, AFHPlayerBase*, Player, const UInputAction*, InputAction, float, HoldDuration);



UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRANKHUNTER_API UFHInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// OnInteract -> 상호작용 키 누르자마자 즉시 호출
	// OnInteractHold -> 상호작용 키 누른 상태로 포커스 벗어나지 않고 HoldDuration만큼 대기시 호출
	// OnInteractRelease -> 상호작용 키 누르고 포커스 벗어나지 않고 상호작용 키 뗄 때 호출

	// 서버를 통해 호출
	UPROPERTY(BlueprintAssignable)
	FOnInteractDelegate OnInteractServer;

	UPROPERTY(BlueprintAssignable)
	FOnInteractForDelegate OnInteractHoldServer;

	UPROPERTY(BlueprintAssignable)
	FOnInteractForDelegate OnInteractReleaseServer;

	// 클라에서 바로 호출
	UPROPERTY(BlueprintAssignable)
	FOnInteractDelegate OnInteractClient;

	UPROPERTY(BlueprintAssignable)
	FOnInteractForDelegate OnInteractHoldClient;

	UPROPERTY(BlueprintAssignable)
	FOnInteractForDelegate OnInteractReleaseClient;

public:	
	UFHInteractableComponent();

protected:
	// Begin UActorComponent override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ~End UActorComponent override;

public:
	void SetIsInteractEnable(bool NewIsInteractEnable);
	void SetIsInteractHoldingEnable(bool NewbIsInteractHoldingEnable);
	void SetIsTooltipUIEnable(bool NewIsTooltipUIEnable);

	bool IsInteractEnable(const UInputAction* FilterInputAction) const;
	bool IsInteractHoldingEnable(const UInputAction* FilterInputAction) const;
	bool IsBlockHoldingReleaseInteract() const;
	bool IsBlockTooltipUI() const;

	float GetHoldDurationForHoldingInteract() const;
	float GetInteractDistance() const;

	TObjectPtr<UFHInteractTooltipWidget> GetTooltipUIWidget() const;
	const TArray<FText>& GetTooltipText() const;
	void SetFirstTooltipText(FText text);
	const TArray<TObjectPtr<UInputAction>>& GetFilterInputActons() const;




	UPROPERTY()
	uint32 bNeedUpdateTooltipText : 1{ false };
	
	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent")
	uint32 bIgnoreThisActorFromInteractCheck : 1{ false };
protected:

	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent")
	uint32 bIsInteractEnable : 1{ true };

	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent")
	uint32 bIsInteractHoldingEnable : 1{ true };
	
	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent")
	uint32 bBlockHoldingReleaseInteract : 1{ false };
	
	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent|TooltipUI")
	uint32 bBlockTooltipUI : 1{ false };

	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent|TooltipUI")
	TObjectPtr<UFHInteractTooltipWidget> TooltipUIWidget;

	UPROPERTY(meta = (DeprecatedProperty, DeprecationMessage = "Use to TooltipTextArray"), EditAnywhere, Category = "FHInteractableComponent|TooltipUI")
	FString TooltipText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FHInteractableComponent|TooltipUI")
	TArray<FText> TooltipTextArray;

	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent")
	float HoldDurationForHoldingInteract{ 1.0f };

	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent")
	float InteractDistance{ 300.0f };

	UPROPERTY(EditAnywhere, Category = "FHInteractableComponent")
	TArray<TObjectPtr<UInputAction>> FilterInputActons;



	// =========== Grow size
	// 일단 만들어는 뒀는데 이것보다 그냥 DT_Item에 스케일 써놓고 생성하는 시점에서 스케일 키우는게 낫다. 만약 이걸 쓰려면 아이템 액터의 ConstructionScript에서 플래그 켜주면 됨.
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bGrowSizeOnFloor : 1{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bGrowSizeOnFloor == true", EditConditionHides))
	float GrowSizeScale { 1.05f };




	// ============= Highlighting
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "FHInteractableComponent|Highlight")
	uint8 bIsHighlightable : 1{ true };
protected:
	UPROPERTY()
	uint8 bIsHighlighting : 1{ false };
	UPROPERTY()
	float ElapsedTimeFromHighlight{ 0.0f };
	UPROPERTY()
	float CurrentHighlightTime{ 0.0f };
public:
	UFUNCTION(BlueprintCallable)
	void Highlight(float Time);
protected:
	void Dehighlight();
	UFUNCTION()
	void WriteOpacity();

	UPROPERTY()
	TArray<class UStaticMeshComponent*> HighlightTargets;


public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
