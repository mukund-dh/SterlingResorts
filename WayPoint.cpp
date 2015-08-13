// Fill out your copyright notice in the Description page of Project Settings.

#include "SterlingResorts.h"
#include "WayPoint.h"
#include "SterlingResortsCharacter.h"


// Sets default values
AWayPoint::AWayPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BaseTriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BaseTriggerVolume"));
	RootComponent = BaseTriggerVolume;	

	BaseTriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AWayPoint::OnComponentBeginOverlap);

	WayPointDirection = CreateAbstractDefaultSubobject<UArrowComponent>(TEXT("WaypointDirection"));
	WayPointDirection->AttachTo(RootComponent);

	NextWayPoint = NULL;
	PrevWayPoint = NULL;
	HaltMovement = true;
}

// Called when the game starts or when spawned
void AWayPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWayPoint::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AWayPoint::OnComponentBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	ASterlingResortsCharacter* afcMyCharacter = Cast<ASterlingResortsCharacter>(OtherActor);
	UCapsuleComponent* afcCapsuleComponent = Cast<UCapsuleComponent>(OtherComp);
	if (afcMyCharacter && afcCapsuleComponent)
	{
		// Set the bIsCharacterMoving boolean to false
		afcMyCharacter->bIsCharacterMoving = false;

		// So if this cast works, trigger the update of the character's TargetWayPoint
		if (afcMyCharacter->bIsMovingForward)
		{
			if (NextWayPoint != NULL)
			{
				// If I find the next waypoint here, I'll set the 
				// waypoint to it
				afcMyCharacter->TargetWayPoint = NextWayPoint;
			}
			else {
				// Set the movement directon to backwards
				afcMyCharacter->bIsMovingForward = false;
				if (PrevWayPoint != NULL)
					afcMyCharacter->TargetWayPoint = PrevWayPoint;
			}
		}
		else {
			if (PrevWayPoint != NULL)
			{
				// Set the previous waypoint as the target
				afcMyCharacter->TargetWayPoint = PrevWayPoint;
			}
			else {
				// Set the movement directon to forwards
				afcMyCharacter->bIsMovingForward = true;
				if (NextWayPoint != NULL)
					afcMyCharacter->TargetWayPoint = NextWayPoint;
			}
		}

		// If this is just an intermediate waypoint, where the character needn't stop
		// make sure we continue the walk unimpeded.
		if (!HaltMovement)
		{
			if (afcMyCharacter->TargetWayPoint != NULL)
			{
				UNavigationSystem::SimpleMoveToActor(GetWorld()->GetFirstPlayerController(), afcMyCharacter->TargetWayPoint);
				afcMyCharacter->bIsCharacterMoving = true;
			}
		}
	}
}

