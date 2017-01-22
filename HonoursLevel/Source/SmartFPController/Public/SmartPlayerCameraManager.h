// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "SmartPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class SMARTFPCONTROLLER_API ASmartPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()	
	

private:
	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "1.0", ClampMax = "3.0" ) )
	float HeadBobFrequency;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.1", ClampMax = "2.0" ) )
	float HeadBobHeight;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.1", ClampMax = "1.0" ) )
	float HeadBobSwayAngle;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.01", ClampMax = "0.1" ) )
	float HeadBobSideMovement;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.1", ClampMax = "2.0" ) )
	float BobHeightSpeedMultiplier;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.1", ClampMax = "2.0" ) )
	float BobStrideSpeedLengthen;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.1", ClampMax = "5.0" ) )
	float JumpLandMove;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "10.0", ClampMax = "100.0" ) )
	float JumpLandTilt;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.1", ClampMax = "4.0" ) )
	float SpringElastic;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.7", ClampMax = "0.9" ) )
	float SpringDampen;

	UPROPERTY( EditAnywhere, Category = "CameraBob", meta = ( ClampMin = "0.1", ClampMax = "1.5" ) )
	float FootstepInterval;	
	

protected:
	// 
	virtual void UpdateHeadBobbing( float DeltaTime );


public:
	//
	UPROPERTY( BlueprintReadOnly, Category = "SmartFPController|CameraBob" )
	FVector BobLocation;

	UPROPERTY( BlueprintReadOnly, Category = "SmartFPController|CameraBob" )
	FRotator BobRotation;

	
	// Sets default values
	ASmartPlayerCameraManager();

	virtual void UpdateCamera( float DeltaTime ) override;	
};
