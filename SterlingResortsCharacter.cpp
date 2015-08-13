// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SterlingResorts.h"
#include "SterlingResortsCharacter.h"
#include "SterlingResortsProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASterlingResortsCharacter

ASterlingResortsCharacter::ASterlingResortsCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	CollectionSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);

	TargetWayPoint = NULL;
	bIsMovingForward = true;
	bIsCharacterMoving = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASterlingResortsCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASterlingResortsCharacter::TouchStarted);
	if( EnableTouchscreenMovement(InputComponent) == false )
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ASterlingResortsCharacter::OnFire);
	}
	
	InputComponent->BindAxis("MoveForward", this, &ASterlingResortsCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASterlingResortsCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ASterlingResortsCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ASterlingResortsCharacter::LookUpAtRate);
}

//void ASterlingResortsCharacter::OnFire()
//{ 
//	if (TargetWayPoint != NULL)
//	{
//		UNavigationSystem::SimpleMoveToActor(GetWorld()->GetFirstPlayerController(), TargetWayPoint);
//	}
//	
//	// try and fire a projectile
//	if (ProjectileClass != NULL)
//	{
//		const FRotator SpawnRotation = GetControlRotation();
//		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
//		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);
//
//		UWorld* const World = GetWorld();
//		if (World != NULL)
//		{
//			// spawn the projectile at the muzzle
//			World->SpawnActor<ASterlingResortsProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
//		}
//	}
//
//	// try and play the sound if specified
//	if (FireSound != NULL)
//	{
//		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
//	}
//
//	// try and play a firing animation if specified
//	if(FireAnimation != NULL)
//	{
//		// Get the animation object for the arms mesh
//		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
//		if(AnimInstance != NULL)
//		{
//			AnimInstance->Montage_Play(FireAnimation, 1.f);
//		}
//	}
//
//}

/*
 * Hitting fire will basically move the character towards the TargetWaypoint
 * if there is one. We are a non-violent lot, we are.
 */
void ASterlingResortsCharacter::OnFire()
{
	if (!bIsCharacterMoving)
	{
		if (TargetWayPoint != NULL)
		{
			UNavigationSystem::SimpleMoveToActor(GetWorld()->GetFirstPlayerController(), TargetWayPoint);
			bIsCharacterMoving = true;
		}
	}
	else {
		GetCharacterMovement()->StopActiveMovement();
		bIsCharacterMoving = false;
	}

}
void ASterlingResortsCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if( TouchItem.bIsPressed == true )
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ASterlingResortsCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if( ( FingerIndex == TouchItem.FingerIndex ) && (TouchItem.bMoved == false) )
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

//void ASterlingResortsCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
//					if (ScaledDelta.X != 0.0f)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (ScaledDelta.Y != 0.0f)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y* BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

/*
* Putting my own functionality in the TouchUpdate; this way we can swipe up/down/left/right from the center
* of the screen and move the character in that direction.
*/
void ASterlingResortsCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScreenCenter = ScreenSize / 2.0f;

					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					//FVector2D ScaledDelta = FVector2D(MoveDelta.X - ScreenCenter.X, MoveDelta.Y - ScreenCenter.Y) / ScreenCenter;
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						MoveRight(ScaledDelta.X * BaseTurnRate * -1.0f);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						MoveForward(ScaledDelta.Y * BaseTurnRate * -1.0f);
					}
				}
			}
		}
	}
}

void ASterlingResortsCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASterlingResortsCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASterlingResortsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASterlingResortsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ASterlingResortsCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if(FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch )
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASterlingResortsCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ASterlingResortsCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASterlingResortsCharacter::TouchUpdate);
	}
	return bResult;
}

// Called when the game starts or when spawned
void ASterlingResortsCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 62.5f;

	// So the idea here, so far, is to gather all the waypoints in the collection
	// radius, find the closest one to our character and set that as the TargetWayPoint

	TArray<AActor*> AllWayPoints;
	CollectionSphere->GetOverlappingActors(AllWayPoints, AWayPoint::StaticClass());

	// For now, lazy bum that I am, I will try and set the TargetWaypoint to the first one from this array
	if (AllWayPoints.Num() != 0)
	{
		TargetWayPoint = Cast<AWayPoint>(AllWayPoints[0]);
	}

}

void ASterlingResortsCharacter::SendCharacterTo(AWayPoint* TargetWayPoint)
{
	// Stop the Character from moving by default
	GetCharacterMovement()->StopActiveMovement();
	bIsCharacterMoving = false;

	// Set the location of the character to the location of the WayPoint
	FVector TargetLocation = TargetWayPoint->GetActorLocation();
	TargetLocation.Z += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	SetActorLocation(TargetLocation);

	// Now the tricky part, turn the character to look in the forward
	// direction of the WayPoint.
	/*
	FVector twpForwardVector = TargetWayPoint->GetActorForwardVector();
	FVector afcForwadVector = GetActorForwardVector();

	float DotProd = FVector::DotProduct(twpForwardVector, afcForwadVector);
	float Degrees = FMath::RadiansToDegrees(FMath::Acos(DotProd));
	FRotator TargetRotation = FRotator(0.0f, Degrees, 0.0f);
	SetActorRotation(TargetRotation);
	*/
}
