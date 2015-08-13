// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "WayPoint.generated.h"

UCLASS()
class STERLINGRESORTS_API AWayPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Base Trigger Volume
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Waypoint)
	UBoxComponent* BaseTriggerVolume;

	// Base Trigger Volume
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Waypoint)
	UArrowComponent* WayPointDirection;

	// Pointer to the Next Waypoint in the chain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoint)
	AWayPoint* NextWayPoint;

	// Pointer to the Previous Waypoint in the chain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoint)
	AWayPoint* PrevWayPoint;

	// Pointer to the Next Waypoint in the chain
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoint)
	bool HaltMovement;

	// Sets default values for this actor's properties
	AWayPoint();

	UFUNCTION()
	void OnComponentBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
