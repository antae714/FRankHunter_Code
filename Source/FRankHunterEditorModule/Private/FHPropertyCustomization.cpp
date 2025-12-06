#include "FHPropertyCustomization.h"
#include "Item\ItemTypes.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/SBoxPanel.h" // SHorizontalBox 사용 위해 추가
#include "Widgets/Layout/SBox.h"
#include "PropertyEditorModule.h"

TSharedRef<IPropertyTypeCustomization> FSpawnableActorDescriptorCustomization::MakeInstance()
{
    return MakeShareable(new FSpawnableActorDescriptorCustomization);
}

void FSpawnableActorDescriptorCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    TSharedPtr<IPropertyHandle> props[4] = {
        StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSpawnableActorDescriptor, bSpawnable)),
        StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSpawnableActorDescriptor, actor)),
        StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSpawnableActorDescriptor, weights)),
        StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSpawnableActorDescriptor, bRandomRotation))
    };

    TSharedPtr<SHorizontalBox> ValueRow;
    HeaderRow.NameContent()
        [
            StructPropertyHandle->CreatePropertyNameWidget()
        ].ValueContent()
                [
                    SAssignNew(ValueRow, SHorizontalBox)
                ];

			for (const auto& prop : props)
			{
				ValueRow->AddSlot().AutoWidth().Padding(0, 0, 5.0f, 0)
					[
						prop->CreatePropertyNameWidget()
					];
				ValueRow->AddSlot().AutoWidth().MinWidth(20.0f).Padding(0, 0, 20.0f, 0)
					[
						prop->CreatePropertyValueWidget()
					];
			}
}

void FSpawnableActorDescriptorCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}
