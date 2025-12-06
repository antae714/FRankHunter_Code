// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Foundation/FHTabListWidget.h"
#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"

DEFINE_LOG_CATEGORY(LogFHTabListWidget);

void UFHTabListWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UFHTabListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTabList();
}

void UFHTabListWidget::NativeDestruct()
{
	for (FFHTabDescriptor& TabDescriptor : PreregisteredTabDescriptorArray)
	{
		if (TabDescriptor.CreatedTabWidget)
		{
			TabDescriptor.CreatedTabWidget->RemoveFromParent();
			TabDescriptor.CreatedTabWidget = nullptr;
		}
	}

	Super::NativeDestruct();
}

void UFHTabListWidget::HandlePreLinkedSwitcherChanged()
{
	for (FFHTabDescriptor& TabDescriptor : PreregisteredTabDescriptorArray)
	{
		if (TabDescriptor.CreatedTabWidget)
		{
			TabDescriptor.CreatedTabWidget->RemoveFromParent();
		}
	}

	Super::HandlePreLinkedSwitcherChanged();
}

void UFHTabListWidget::HandlePostLinkedSwitcherChanged()
{
	if (!IsDesignTime() && GetCachedWidget().IsValid())
	{
		SetupTabList();
	}

	Super::HandlePostLinkedSwitcherChanged();
}

void UFHTabListWidget::HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton)
{
	FFHTabDescriptor TabDescriptor;
	FFHTabDescriptor* TabDescriptorPtr = nullptr;
	if (GetPreregisteredTabDescriptor(TabId, TabDescriptor))
	{
		TabDescriptorPtr = &TabDescriptor;
	}
	else
	{
		TabDescriptorPtr = PendingTabDescriptorMap.Find(TabId);
	}
	
	if (TabButton->GetClass()->ImplementsInterface(UFHTabButtonInterface::StaticClass()))
	{
		if (TabDescriptorPtr)
		{
			IFHTabButtonInterface::Execute_SetTabDescriptor(TabButton, *TabDescriptorPtr);
		}
		else
		{
			UE_LOG(LogFHTabListWidget, Warning, TEXT("Cound not find TabDescriptor corresponding to TabId. RegisterDynamicTab Function should be used."));
		}
	}

	PendingTabDescriptorMap.Remove(TabId);
}

bool UFHTabListWidget::GetPreregisteredTabDescriptor(const FName TabNameId, FFHTabDescriptor& OutTabDescriptor)
{
	const FFHTabDescriptor* const FoundTabDescriptor = PreregisteredTabDescriptorArray.FindByPredicate([&](FFHTabDescriptor& TabDescriptor) -> bool
		{
			return (TabDescriptor.TabId == TabNameId);
		});

	if (!FoundTabDescriptor)
	{
		return false;
	}

	OutTabDescriptor = *FoundTabDescriptor;

	return true;
}

void UFHTabListWidget::SetTabHiddenState(FName TabNameId, bool bHidden)
{
	for(FFHTabDescriptor& TabDescriptor : PreregisteredTabDescriptorArray)
	{
		if (TabDescriptor.TabId == TabNameId)
		{
			TabDescriptor.bHidden = bHidden;
			break;
		}
	}
}

bool UFHTabListWidget::RegisterDynamicTab(const FFHTabDescriptor& TabDescriptor)
{
	if (TabDescriptor.bHidden)
	{
		return true;
	}

	PendingTabDescriptorMap.Add(TabDescriptor.TabId, TabDescriptor);

	return RegisterTab(TabDescriptor.TabId, TabDescriptor.TabButtonClass, TabDescriptor.CreatedTabWidget);
}

bool UFHTabListWidget::IsFirstTabActive() const
{
	if (PreregisteredTabDescriptorArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabDescriptorArray[0].TabId;
	}

	return false;
}

bool UFHTabListWidget::IsLastTabActive() const
{
	if (PreregisteredTabDescriptorArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabDescriptorArray.Last().TabId;
	}

	return false;
}

bool UFHTabListWidget::IsTabVisible(FName TabId)
{
	const UCommonButtonBase* Button = GetTabButtonBaseByID(TabId);
	if (Button)
	{
		const ESlateVisibility TabVisibility = Button->GetVisibility();
		if (TabVisibility == ESlateVisibility::Hidden || TabVisibility == ESlateVisibility::Collapsed)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

int32 UFHTabListWidget::GetVisibleTabCount()
{
	int32 Result = 0;
	const int32 TabCount = GetTabCount();
	for (int32 I = 0; I < TabCount; ++I)
	{
		if (IsTabVisible(GetTabIdAtIndex(I)))
		{
			Result++;
		}
	}

	return Result;
}

void UFHTabListWidget::SetupTabList()
{
	for (FFHTabDescriptor& TabDescriptor : PreregisteredTabDescriptorArray)
	{
		if (TabDescriptor.bHidden)
		{
			continue;
		}

		if (!TabDescriptor.CreatedTabWidget && TabDescriptor.TabContentClass)
		{
			TabDescriptor.CreatedTabWidget = CreateWidget<UCommonUserWidget>(GetOwningPlayer(), TabDescriptor.TabContentClass);

			OnTabCreatedNative.Broadcast(TabDescriptor.TabId, Cast<UCommonUserWidget>(TabDescriptor.CreatedTabWidget));
			OnTabCreated.Broadcast(TabDescriptor.TabId, Cast<UCommonUserWidget>(TabDescriptor.CreatedTabWidget));
		}

		if (UCommonAnimatedSwitcher* CurrentLinkedSwitcher = GetLinkedSwitcher())
		{
			if (!CurrentLinkedSwitcher->HasChild(TabDescriptor.CreatedTabWidget))
			{
				CurrentLinkedSwitcher->AddChild(TabDescriptor.CreatedTabWidget);
			}
		}

		if (GetTabButtonBaseByID(TabDescriptor.TabId) == nullptr)
		{
			RegisterTab(TabDescriptor.TabId, TabDescriptor.TabButtonClass, TabDescriptor.CreatedTabWidget);
		}
	}
}
