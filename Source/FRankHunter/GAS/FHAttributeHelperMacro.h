// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "AttributeSet.h"

// GAMEPLAYATTRIBUTE 매크로가 요구합니다.
#include "AbilitySystemComponent.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE(FFHAttributeEvent);