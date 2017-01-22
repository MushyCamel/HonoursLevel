// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#include "SmartFPController.h"
#include "SmartPlayerCameraManager.h"
#include "FirstPersonCharacter.h"


// Sets default values
ASmartPlayerCameraManager::ASmartPlayerCameraManager()
{
	ViewPitchMax = 65.f;
	ViewPitchMin = -65.f;
	bAlwaysApplyModifiers = true;

	HeadBobFrequency = 1.5f;
	HeadBobHeight = .25f;
	HeadBobSwayAngle = .25f;
	HeadBobSideMovement = .035f;
	BobHeightSpeedMultiplier = .35f;
	BobStrideSpeedLengthen = .25f;
	JumpLandMove = 2.f;
	JumpLandTilt = 35.f;
	SpringElastic = 1.25f;
	SpringDampen = .77f;
	FootstepInterval = .5f;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .01f;
}

// Update Camera
void ASmartPlayerCameraManager::UpdateCamera( float DeltaTime )
{
	Super::UpdateCamera( DeltaTime );
	//	
	UpdateHeadBobbing( DeltaTime );
}


// Update HeadBobbing
void ASmartPlayerCameraManager::UpdateHeadBobbing( float DeltaTime )
{
	AFirstPersonCharacter* m_Character = Cast<AFirstPersonCharacter>( GetOwningPlayerController()->GetCharacter() );

	if( m_Character == nullptr )
		return;


	static FVector prevVelocity = FVector::ZeroVector;
	FVector velocity = m_Character->GetVelocity() / 140.f;
	const FVector& velocityChange = velocity - prevVelocity;
	prevVelocity = velocity;

	if( !m_Character->IsClimbing() )
		velocity.Z = 0.f;

	static float springPos = 0.f, springVelocity = 0.f;
	springVelocity -= velocityChange.Z;
	springVelocity -= springPos * SpringElastic;
	springVelocity *= SpringDampen;
	springPos += springVelocity * DeltaTime;
	springPos = FMath::Clamp( springPos, -.32f, .32f );

	if( FMath::Abs( springVelocity ) < .005f && FMath::Abs( springPos ) < .005f )
		springVelocity = springPos = 0.f;

	const float velSize = velocity.Size();
	float flatVelocity = velSize;

	if( m_Character->IsClimbing() )
		flatVelocity *= 4.f;
	else if( !m_Character->IsClimbing() && !m_Character->IsGrounded() )
		flatVelocity = 0.f;

	const float strideLengthen = 1.f + flatVelocity * BobStrideSpeedLengthen;	
	static float headBobCycle = 0.f;
	headBobCycle += ( flatVelocity / strideLengthen ) * ( DeltaTime / HeadBobFrequency );

	float bobFactor = FMath::Sin( headBobCycle * PI * 2.f );
	bobFactor = 1.f - ( bobFactor * .5f + 1.f );
	bobFactor *= bobFactor;

	const float headBobFade = FMath::Clamp( FMath::Abs( m_Character->GetNormalizedSpeed() ), 0.f, 1.f );
	const float bobSwayFactor = FMath::Sin( headBobCycle * PI * 2.f + HALF_PI );
	const float speedHeightFactor = 1.f + ( flatVelocity * BobHeightSpeedMultiplier );

	const float yPos = -HeadBobSideMovement * bobSwayFactor * headBobFade;
	const float zPos = springPos * JumpLandMove + bobFactor * HeadBobHeight * speedHeightFactor * headBobFade;
	const float pitchTilt = springPos * JumpLandTilt;
	const float yawTilt = bobSwayFactor * HeadBobSwayAngle * headBobFade;

	BobLocation.X = 0.f;
	BobLocation.Y = yPos * 140.f;
	BobLocation.Z = zPos * 140.f;

	BobRotation.Pitch = pitchTilt * 4.f;
	BobRotation.Yaw = yawTilt * 4.f;
	BobRotation.Roll = 0.f;

	if( m_Character->bUseHeadBob )
	{		
		CameraCache.POV.Location += BobLocation;
		CameraCache.POV.Rotation += BobRotation;
	}	

	m_Character->PlayFootstepSound( headBobCycle, FootstepInterval );
}