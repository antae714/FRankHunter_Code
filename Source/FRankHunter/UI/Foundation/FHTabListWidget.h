// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CommonTabListWidgetBase.h"
#include "FHTabListWidget.generated.h"

USTRUCT(BlueprintType)
struct FFHTabDescriptor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bHidden : 1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonButtonBase> TabButtonClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCommonUserWidget> TabContentClass;

	UPROPERTY(Transient)
	TObjectPtr<UWidget> CreatedTabWidget;
};

UINTERFACE(BlueprintType)
class UFHTabButtonInterface : public UInterface
{
	GENERATED_BODY()
};

class IFHTabButtonInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "TabButton")
	void SetTabDescriptor(const FFHTabDescriptor& TabDescriptor);
};

UCLASS(Blueprintable, BlueprintType, Abstract, meta = (DisableNativeTick))
class FRANKHUNTER_API UFHTabListWidget : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** @With UCommonAnimatedSwitcher */
	virtual void HandlePreLinkedSwitcherChanged() override;
	virtual void HandlePostLinkedSwitcherChanged() override;

	virtual void HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton) override;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTabCreated, FName, TabId, UCommonUserWidget*, TabWidget);
	DECLARE_EVENT_TwoParams(UFHTabListWidget, FOnTabCreatedNative, FName /* TabId */, UCommonUserWidget* /* TabWidget */);

	UPROPERTY(BlueprintAssignable, Category = "TabList")
	FOnTabCreated OnTabCreated;
	FOnTabCreatedNative OnTabCreatedNative;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TabList")
	bool GetPreregisteredTabDescriptor(const FName TabNameId, FFHTabDescriptor& OutTabDescriptor);

	UFUNCTION(BlueprintCallable, Category = "TabList")
	void SetTabHiddenState(FName TabNameId, bool bHidden);

	UFUNCTION(BlueprintCallable, Category = "TabList")
	bool RegisterDynamicTab(const FFHTabDescriptor& TabDescriptor);

	UFUNCTION(BlueprintCallable, Category = "TabList")
	bool IsFirstTabActive() const;

	UFUNCTION(BlueprintCallable, Category = "TabList")
	bool IsLastTabActive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TabList")
	bool IsTabVisible(FName TabId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TabList")
	int32 GetVisibleTabCount();

private:
	UPROPERTY(EditAnywhere, meta=(TitleProperty="TabId"))
	TArray<FFHTabDescriptor> PreregisteredTabDescriptorArray;
	
	UPROPERTY()
	TMap<FName, FFHTabDescriptor> PendingTabDescriptorMap;

private:
	void SetupTabList();
};

DECLARE_LOG_CATEGORY_EXTERN(LogFHTabListWidget, Log, All);