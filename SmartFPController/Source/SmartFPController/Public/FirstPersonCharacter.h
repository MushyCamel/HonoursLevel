// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#pragma once

#include "Ladder.h"
#include "GameFramework/Character.h"
#include "FirstPersonCharacter.generated.h"


USTRUCT()
struct SMARTFPCONTROLLER_API FStepSounds
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY( EditDefaultsOnly )
	USoundCue* JumpingCUE;
	UPROPERTY( EditDefaultsOnly )
	USoundCue* LandingCUE;
	UPROPERTY( EditDefaultsOnly )
	USoundCue* FootstepsCUE;
};

USTRUCT()
struct SMARTFPCONTROLLER_API FFloorSurface
{
	GENERATED_USTRUCT_BODY()

private:	
	// To edit surface type for your project, use ProjectSettings/Physics/PhysicalSurface section
	UPROPERTY( EditDefaultsOnly )
	TEnumAsByte<EPhysicalSurface> SurfaceType;

public:
	UPROPERTY( EditDefaultsOnly )
	FStepSounds Sounds;

	// Get SufaceType
	FORCEINLINE EPhysicalSurface GetSufaceType() const { return SurfaceType; }
};


UCLASS()
class SMARTFPCONTROLLER_API AFirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

		
private:
	//
	UPROPERTY( EditAnywhere, Category = "Camera", meta = ( ClampMin = "0.1", ClampMax = "1" ) )
	float LookSmooth;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = ( AllowPrivateAccess = "true" ) )
	UCameraComponent* FPCameraComponent;


	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = ( AllowPrivateAccess = "true" ) )
	UAudioComponent* FPAudioComponent;

	UPROPERTY( EditDefaultsOnly, Category = "Footstep Sounds" )
	FStepSounds GenericSounds;

	UPROPERTY( EditDefaultsOnly, Category = "Footstep Sounds" )
	TArray<FFloorSurface> Surfaces;

	UPROPERTY( EditAnywhere, Category = "Character", meta = ( ClampMin = "0.1", ClampMax = "1" ) )
	float MoveSmooth;

	UPROPERTY( EditAnywhere, Category = "Character", meta = ( ClampMin = "1", ClampMax = "3.75" ) )
	float RunningSpeedModifier;

	UPROPERTY( EditAnywhere, Category = "Character", meta = ( ClampMin = "0.1", ClampMax = "1" ) )
	float BackwardSpeedModifier;

	UPROPERTY( EditAnywhere, Category = "Character", meta = ( ClampMin = "0.1", ClampMax = "1" ) )
	float SidewaysSpeedModifier;

	UPROPERTY( EditAnywhere, Category = "Character", meta = ( ClampMin = "0.1", ClampMax = "1" ) )
	float ClimbingSpeedModifier;


	float deltaTime;
	float nativeBrakingFrictionFactor, nativeEyeHeight, nativeCapsuleHalfHeight;
	float normalizedSpeed, direction;
	bool bIsGrounded, bIsClimbing;
	bool bIsMoved, bIsMovedForward;
	bool bWantsToRun, bWantsToRunToggled;	
	ALadder* currentLadder;	

	void CalcDirectionAndSpeed( const FVector& velocity, const float smoothTime );

	FFloorSurface* GetFloorSusface();

	void MoveHorizontal( float AxisValue );
	void MoveVertical( float AxisValue );
	void LookHorizontal( float AxisValue );
	void LookVertical( float AxisValue );
	
	FORCEINLINE void OnStartRunning() { bWantsToRun = true; };
	FORCEINLINE void OnStartRunningToggle(){ bWantsToRunToggled = true; }
	FORCEINLINE void OnStopRunning(){ bWantsToRun = false; }
	

public:
	//
	UPROPERTY( EditAnywhere, Category = "Camera", meta = ( ClampMin = "10", ClampMax = "100" ) )
	float LookSensitivity;
	
	// On/Off head bobbing effect
	UPROPERTY( EditAnywhere, Category = "Camera" )
	uint32 bUseHeadBob : 1;


	// Sets default values for this character's properties
	AFirstPersonCharacter( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get() );

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent( class UInputComponent* InputComponent ) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	

	// OnStart Crouch
	virtual void OnStartCrouch( float HalfHeightAdjust, float ScaledHalfHeightAdjust ) override;
	// OnEnd Crouch
	virtual void OnEndCrouch( float HalfHeightAdjust, float ScaledHalfHeightAdjust ) override;

	// Play FootstepSound
	void PlayFootstepSound( const float HeadBobCycle, const float FootstepInterval );


	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Action" )
	void StartJumping();

	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Action" )
	void SitDown();
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Action" )
	void StandUp();
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Action" )
	void CrouchToggle();


	// Get mooving on ground state
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE bool IsGrounded() const { return bIsGrounded; }

	// Get movement state
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE bool IsMoved() const { return bIsMoved; }

	// Get movement forward state
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE bool IsMovedForward() const { return bIsMovedForward; }

	// Get the modifier value for running speed
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE float GetBackwardSpeedModifier() const { return BackwardSpeedModifier; }

	// Get running state
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE bool IsRunning() const { return !bIsCrouched && ( bWantsToRun || bWantsToRunToggled ) && bIsMovedForward; }

	// Get the modifier value for running speed
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE float GetRunningSpeedModifier() const { return RunningSpeedModifier; }

	// Get climbing state
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE bool IsClimbing() const { return bIsClimbing; }

	// Get the modifier value for climbing speed
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE float GetClimbingSpeedModifier() const { return ClimbingSpeedModifier; }
	
	// Get NormalizedSpeed
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE float GetNormalizedSpeed() const { return normalizedSpeed; }
	// Get Direction
	UFUNCTION( BlueprintCallable, Category = "SmartFPController|Character" )
	FORCEINLINE float GetDirection() const { return direction; }


	// OnLadder Enter
	void OnLadderEnter( ALadder* ladder );
	// OnLadder Exit
	void OnLadderExit();
};
