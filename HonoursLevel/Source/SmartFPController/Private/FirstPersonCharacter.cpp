// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#include "SmartFPController.h"
#include "FirstPersonCharacter.h"
#include "SmartMovementComponent.h"
#include "SmartPlayerCameraManager.h"


// Sets default values
AFirstPersonCharacter::AFirstPersonCharacter( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer.SetDefaultSubobjectClass<USmartMovementComponent>( ACharacter::CharacterMovementComponentName ) )
{
	bIsGrounded = true;
	GetCharacterMovement()->bAlwaysCheckFloor = true;
	GetCapsuleComponent()->bReturnMaterialOnMove = true;

	// Create a FPCameraComponent Component	
	FPCameraComponent = CreateDefaultSubobject<UCameraComponent>( TEXT( "FPCameraComponent" ) );
	FPCameraComponent->SetupAttachment( GetCapsuleComponent() );
	FPCameraComponent->SetRelativeLocation( FVector( 0.f, 0.f, BaseEyeHeight ) );
	FPCameraComponent->bUsePawnControlRotation = true;
	
	// Create a FPAudioComponent Component	
	FPAudioComponent = CreateDefaultSubobject<UAudioComponent>( TEXT( "FPAudioComponent" ) );
	FPAudioComponent->SetupAttachment( GetCapsuleComponent() );
	FPAudioComponent->SetRelativeLocation( FVector( 0.f, 0.f, -64.f ) );
	FPAudioComponent->bAutoActivate = false;
	FPAudioComponent->bIsUISound = false;
	
	CrouchedEyeHeight = 45.f;
	LookSensitivity = 50.f;
	LookSmooth = .35f;
	bUseHeadBob = true;

	MoveSmooth = .21f;
	RunningSpeedModifier = 1.75f;
	BackwardSpeedModifier = .6f;
	SidewaysSpeedModifier = .7f;
	ClimbingSpeedModifier = .8;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .01f;
}

// Called to bind functionality to input
void AFirstPersonCharacter::SetupPlayerInputComponent( class UInputComponent* InputComponent )
{
	Super::SetupPlayerInputComponent( InputComponent );

	InputComponent->BindAxis( "MoveHorizontal", this, &AFirstPersonCharacter::MoveHorizontal );
	InputComponent->BindAxis( "MoveVertical", this, &AFirstPersonCharacter::MoveVertical );
	InputComponent->BindAxis( "LookHorizontal", this, &AFirstPersonCharacter::LookHorizontal );
	InputComponent->BindAxis( "LookVertical", this, &AFirstPersonCharacter::LookVertical );

	InputComponent->BindAction( "Jump", IE_Pressed, this, &AFirstPersonCharacter::StartJumping );
	
	InputComponent->BindAction( "Crouch", IE_Pressed, this, &AFirstPersonCharacter::SitDown );
	InputComponent->BindAction( "Crouch", IE_Released, this, &AFirstPersonCharacter::StandUp );
	InputComponent->BindAction( "CrouchToggle", IE_Pressed, this, &AFirstPersonCharacter::CrouchToggle );

	InputComponent->BindAction( "Run", IE_Pressed, this, &AFirstPersonCharacter::OnStartRunning );
	InputComponent->BindAction( "Run", IE_Released, this, &AFirstPersonCharacter::OnStopRunning );
	InputComponent->BindAction( "RunToggle", IE_Pressed, this, &AFirstPersonCharacter::OnStartRunningToggle );
}


// Called when the game starts or when spawned
void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->bAlwaysCheckFloor = true;
	GetCapsuleComponent()->bReturnMaterialOnMove = true;
	
	nativeEyeHeight = BaseEyeHeight;
	nativeBrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor;
	nativeCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

// Called every frame
void AFirstPersonCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	//

	deltaTime = DeltaTime;

	bIsGrounded = GetCharacterMovement()->IsMovingOnGround();
	
	const FVector& velocity = GetVelocity();

	bIsMoved = !velocity.IsZero();
	bIsMovedForward = ( bIsMoved && FVector::DotProduct( velocity.GetSafeNormal2D(), GetActorForwardVector() ) > -.1f );

	if( bWantsToRunToggled && !IsRunning() )
	{
		bWantsToRun = false;
		bWantsToRunToggled = false;
	}
	
	CalcDirectionAndSpeed( velocity, DeltaTime * 7.f );

	const FVector desiredLocation( FPCameraComponent->RelativeLocation.X, FPCameraComponent->RelativeLocation.Y, ( bIsCrouched ? CrouchedEyeHeight : nativeEyeHeight ) );
	FPCameraComponent->SetRelativeLocation( FMath::Lerp( FPCameraComponent->RelativeLocation, desiredLocation, DeltaTime * 10.f ) );
}


// Move
void AFirstPersonCharacter::MoveHorizontal( float AxisValue )
{
	static float smoothValue = 0.f;
	smoothValue = FMath::Lerp( smoothValue, AxisValue, MoveSmooth * 50.f * deltaTime );

	if( smoothValue != 0.f )
		AddMovementInput( GetActorRightVector(), smoothValue * SidewaysSpeedModifier );
}
void AFirstPersonCharacter::MoveVertical( float AxisValue )
{
	static float smoothValue = 0.f;
	smoothValue = FMath::Lerp( smoothValue, AxisValue, MoveSmooth * 50.f * deltaTime );

	if( smoothValue != 0.f )
	{
		const bool moveForward = ( AxisValue > 0.f );
		//smoothValue = moveForward ? smoothValue : smoothValue * BackwardSpeedModifier;
		const FVector& pawnForwardVector = GetActorForwardVector();

		if( bIsClimbing )
		{			
			FVector moveVector = pawnForwardVector;
			bool lookUp = FPCameraComponent->GetComponentRotation().Pitch > -25.f;
				
			if( moveForward )
				moveVector = lookUp ? currentLadder->GetActorUpVector() : -currentLadder->GetActorUpVector();

			if( bIsGrounded )
			{
				if( moveForward && !lookUp )
					moveVector += pawnForwardVector;
			}
			else
			{
				if( moveForward && lookUp )
					moveVector += pawnForwardVector;
			}

			AddMovementInput( moveVector, smoothValue );
		}
		else
		{
			AddMovementInput( pawnForwardVector, smoothValue );
		}		
	}
}

// Look
void AFirstPersonCharacter::LookHorizontal( float AxisValue )
{
	static float smoothValue = 0.f;
	smoothValue = FMath::Lerp( smoothValue, AxisValue, LookSmooth * 50.f * deltaTime );
	AddControllerYawInput( smoothValue * LookSensitivity * deltaTime );
}
void AFirstPersonCharacter::LookVertical( float AxisValue )
{
	static float smoothValue = 0.f;
	smoothValue = FMath::Lerp( smoothValue, AxisValue, LookSmooth * 50.f * deltaTime );
	AddControllerPitchInput( smoothValue * LookSensitivity * deltaTime );
}


// Sit Down
void AFirstPersonCharacter::SitDown()
{
	if( bIsGrounded && !bIsClimbing )
		Crouch();
}
// Stand Up
void AFirstPersonCharacter::StandUp()
{
	if( bIsGrounded && !bIsClimbing )
		UnCrouch();
}

// Toggle Crouch
void AFirstPersonCharacter::CrouchToggle()
{
	if( bIsGrounded && !bIsClimbing )
		bIsCrouched ? UnCrouch() : Crouch();			
}

// OnStart Crouch
void AFirstPersonCharacter::OnStartCrouch( float HalfHeightAdjust, float ScaledHalfHeightAdjust )
{
	Super::OnStartCrouch( HalfHeightAdjust, ScaledHalfHeightAdjust );
	// 
	const FVector desiredLocation( FPCameraComponent->RelativeLocation.X, FPCameraComponent->RelativeLocation.Y, HalfHeightAdjust + nativeEyeHeight );
	FPCameraComponent->SetRelativeLocation( desiredLocation );
}
// OnEnd Crouch
void AFirstPersonCharacter::OnEndCrouch( float HalfHeightAdjust, float ScaledHalfHeightAdjust )
{
	Super::OnEndCrouch( HalfHeightAdjust, ScaledHalfHeightAdjust );
	//
	const FVector desiredLocation( FPCameraComponent->RelativeLocation.X, FPCameraComponent->RelativeLocation.Y, CrouchedEyeHeight - HalfHeightAdjust );
	FPCameraComponent->SetRelativeLocation( desiredLocation );
}

// Get Susface
FFloorSurface* AFirstPersonCharacter::GetFloorSusface()
{
	if( !bIsGrounded )
		return nullptr;
	
	const EPhysicalSurface& targetSurface = UPhysicalMaterial::DetermineSurfaceType( GetCharacterMovement()->CurrentFloor.HitResult.PhysMaterial.Get() );

	if( targetSurface != UPhysicalMaterial::DetermineSurfaceType( GEngine->DefaultPhysMaterial ) )
		for( auto& Sur : Surfaces )
			if( Sur.GetSufaceType() == targetSurface )		
				return &Sur;
	
	return nullptr;
}

// Start Jumping
void AFirstPersonCharacter::StartJumping()
{
	if( bIsGrounded && !bIsClimbing )
	{
		if( bIsCrouched )
		{
			StandUp();
			return;
		}

		const FFloorSurface* tmpSurface = GetFloorSusface();
		FPAudioComponent->SetSound( ( tmpSurface != nullptr ) ? tmpSurface->Sounds.JumpingCUE : GenericSounds.JumpingCUE );
		FPAudioComponent->Play();
		Jump();
	}
}

// Play FootstepSound
void AFirstPersonCharacter::PlayFootstepSound( const float HeadBobCycle, const float FootstepInterval )
{	
	static float nextStep = 0.f;
	static bool prevGrounded = false;

	if( !prevGrounded && bIsGrounded )
	{
		StopJumping();

		nextStep = HeadBobCycle + FootstepInterval;
		const FFloorSurface* tmpSurface = GetFloorSusface();
		UGameplayStatics::PlaySoundAtLocation( this, ( tmpSurface != nullptr ) ? tmpSurface->Sounds.LandingCUE : GenericSounds.LandingCUE, FPAudioComponent->RelativeLocation );
		prevGrounded = true;
		return;
	}

	if( HeadBobCycle > nextStep )
	{
		nextStep = HeadBobCycle + FootstepInterval;

		if( bIsGrounded && !bIsClimbing )
		{
			const FFloorSurface* tmpSurface = GetFloorSusface();
			FPAudioComponent->SetSound( ( tmpSurface != nullptr ) ? tmpSurface->Sounds.FootstepsCUE : GenericSounds.FootstepsCUE );
			FPAudioComponent->Play();
		}
		else if( bIsClimbing )
		{
			currentLadder->PlayLadderFootstepSound( FPAudioComponent );
		}
	}

	prevGrounded = bIsGrounded;
}


// OnLadder Enter
void AFirstPersonCharacter::OnLadderEnter( ALadder* ladder )
{
	if( bIsCrouched )
		StandUp();
	
	bIsClimbing = true;
	currentLadder = ladder;
	nativeBrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor;
	GetCharacterMovement()->BrakingFrictionFactor = 1000.f;
	GetCharacterMovement()->SetMovementMode( EMovementMode::MOVE_Flying );
}
// OnLadder Exit
void AFirstPersonCharacter::OnLadderExit()
{
	bIsClimbing = false;
	currentLadder = nullptr;
	GetCharacterMovement()->BrakingFrictionFactor = nativeBrakingFrictionFactor;
	GetCharacterMovement()->SetMovementMode( EMovementMode::MOVE_Walking );
}


// Calc Direction And Speed
void AFirstPersonCharacter::CalcDirectionAndSpeed( const FVector& velocity, const float smoothTime )
{
	float velSize2D = 0.f;
		
	if( bIsGrounded && !bIsClimbing )
	{
		velSize2D = velocity.GetSafeNormal2D().Size2D();
		velSize2D = IsRunning() ? velSize2D * 2.f : velSize2D;
		velSize2D = bIsMovedForward ? velSize2D : -velSize2D;

		const float percent = velocity.Size() / GetCharacterMovement()->GetMaxSpeed();
		normalizedSpeed = FMath::Lerp( normalizedSpeed, velSize2D * percent, smoothTime );
	}	

	if( velSize2D != 0.f )
	{
		const FRotator targetRotation( GetActorRotation().GetInverse().Quaternion() * velocity.ToOrientationQuat() );
		float yaw = targetRotation.Yaw;
		//yaw = ( yaw >= 180.f ) ? yaw - 360.f : yaw;		
		
		if( bIsMovedForward )		
			yaw = FMath::Clamp( yaw, -90.f, 90.f );
		else
			yaw = ( yaw > 90.f ) ? yaw - 180.f : ( ( yaw < -90.f ) ? yaw + 180.f : yaw );

		direction = FMath::Lerp( direction, yaw, smoothTime );
	}
	else
	{
		direction = FMath::Lerp( direction, 0.f, smoothTime );
		normalizedSpeed = FMath::Lerp( normalizedSpeed, 0.f, smoothTime );
	}
}


//FRuntimeFloatCurve
//GEngine->AddOnScreenDebugMessage( -1, 1.f, FColor::White, TEXT( "Play Footstep Sound. " ) );