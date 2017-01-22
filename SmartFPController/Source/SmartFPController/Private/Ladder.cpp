// Asset: Smart First Person Controller, Copyright(c): 2016 Victor Klepikov

#include "SmartFPController.h"
#include "Ladder.h"
#include "FirstPersonCharacter.h"


// Sets default values
ALadder::ALadder()
{
	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "LadderMesh" ) );
	RootComponent = LadderMesh;
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>( TEXT( "BoxCollider" ) );
	BoxCollider->SetupAttachment( LadderMesh );
	BoxCollider->BodyInstance.SetCollisionProfileName( "Trigger" );
	BoxCollider->bGenerateOverlapEvents = true;
	BoxCollider->OnComponentBeginOverlap.AddDynamic( this, &ALadder::OnTriggerEnter );
	BoxCollider->OnComponentEndOverlap.AddDynamic( this, &ALadder::OnTriggerExit );

	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}


// PlayLadder FootstepSound
void ALadder::PlayLadderFootstepSound( UAudioComponent* audioSource )
{
	audioSource->SetSound( FootstepsCUE );
	audioSource->Play();
}

// OnTrigger Enter
void ALadder::OnTriggerEnter( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( AFirstPersonCharacter* playerCharacter = Cast<AFirstPersonCharacter>( OtherActor ) )	
		playerCharacter->OnLadderEnter( this );	
}
// OnTrigger Exit
void ALadder::OnTriggerExit( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	if( AFirstPersonCharacter* playerCharacter = Cast<AFirstPersonCharacter>( OtherActor ) )	
		playerCharacter->OnLadderExit();	
}