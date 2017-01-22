// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#include "SmartFPController.h"
#include "SmartMovementComponent.h"
#include "FirstPersonCharacter.h"


// Sets default values
USmartMovementComponent::USmartMovementComponent()
{
	GetNavAgentPropertiesRef().bCanCrouch = true;
	GetNavAgentPropertiesRef().bCanFly = true;
	bAlwaysCheckFloor = true;
	Mass = 65.f;
	JumpZVelocity = 360.f;
	MaxWalkSpeed = 425.f;
	MaxWalkSpeedCrouched = 200.f;
	MaxFlySpeed = MaxWalkSpeed * .5;
	MaxCustomMovementSpeed = MaxWalkSpeed;
}


// Get MaxSpeed
float USmartMovementComponent::GetMaxSpeed() const
{
	const AFirstPersonCharacter* m_Character = Cast<AFirstPersonCharacter>( GetPawnOwner() );

	float maxSpeed = Super::GetMaxSpeed();

	if( m_Character == nullptr )
		return maxSpeed;

	if( m_Character->IsRunning() )
		maxSpeed *= m_Character->GetRunningSpeedModifier();

	if( m_Character->IsClimbing() )
		maxSpeed *= m_Character->GetClimbingSpeedModifier();

	if( !m_Character->IsMovedForward() )
		maxSpeed *= m_Character->GetBackwardSpeedModifier();
	
	return maxSpeed;
}
