// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TimeObject.h"
#include "Components/ActorComponent.h"
#include "TimePhysics.generated.h"

UCLASS(Blueprintable)
class TIMESTEP_API UTimePhysics : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimePhysics();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, category = "Time Manipulation")
	void TrackVelocities();

	FVector realLinearVelocity;
	FVector realAngularVelocity;

	UPROPERTY(BlueprintReadWrite)
	UTimeObject* timeObjectComponent;

	UPROPERTY(BlueprintReadWrite)
	UPrimitiveComponent* physicsBodyComponent;

protected:
	void ClearForcesAndImpulses();

	//used when timescale drops to <= 0.f
	void FreezePhysicsBody();

	bool bTimeWasForward;

	float lastFrameTimeDilation; //the time dilation value used last frame

	static float defaultGravityZ;
	static float defaultTerminalVelocity;
	
};
