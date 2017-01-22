// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#pragma once

#include "GameFramework/Actor.h"
#include "Ladder.generated.h"


UCLASS()
class SMARTFPCONTROLLER_API ALadder : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY( VisibleDefaultsOnly, Category = "Static Mesh" ) //HideCategories=(Mobility)
	UStaticMeshComponent* LadderMesh;

	UPROPERTY( VisibleDefaultsOnly, Category = "Ladder" )
	UBoxComponent* BoxCollider;


	UPROPERTY( EditDefaultsOnly, Category = "SFX" )
	USoundCue* FootstepsCUE;

	
public:	

	// Sets default values for this actor's properties
	ALadder();

	
	// PlayLadder FootstepSound
	void PlayLadderFootstepSound( UAudioComponent* audioSource );


	UFUNCTION()
	void OnTriggerEnter( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
	void OnTriggerExit( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
};
