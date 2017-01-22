// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "SmartMovementComponent.generated.h"


UCLASS()
class SMARTFPCONTROLLER_API USmartMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()	
	
public:

	// Sets default values for this actor's properties
	USmartMovementComponent();


	virtual float GetMaxSpeed() const override;	
};
