// Fill out your copyright notice in the Description page of Project Settings.

#include "Timestep.h"
#include "TimePhysics.h"
#include "PhysXIncludes.h"

//uses the ue4 defaults
float UTimePhysics::defaultGravityZ = -980.f;
float UTimePhysics::defaultTerminalVelocity = 4000.f;

// Sets default values for this component's properties
UTimePhysics::UTimePhysics()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	SetTickGroup(ETickingGroup::TG_PrePhysics);
	// ...
}


// Called when the game starts
void UTimePhysics::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

//AddAngularImpulse() is fucked???
void UTimePhysics::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	FVector oldRealLinear = realLinearVelocity;
	FVector oldRealAngular = realAngularVelocity;

	if (timeObjectComponent->GetCurrentTimeDilation() > 0.f)
	{
		if (bTimeWasForward)
		{
			physicsBodyComponent->SetPhysicsLinearVelocity(realLinearVelocity * timeObjectComponent->GetCurrentTimeDilation());
			physicsBodyComponent->SetPhysicsAngularVelocity(realAngularVelocity * timeObjectComponent->GetCurrentTimeDilation());
			ClearForcesAndImpulses();
		}
		else
		{
			float ratio = timeObjectComponent->GetCurrentTimeDilation() / lastFrameTimeDilation; //0.1/0.2 = 0.5
			
			FVector adjustingLinearImpulse = physicsBodyComponent->GetPhysicsLinearVelocity() * ratio - physicsBodyComponent->GetPhysicsLinearVelocity();
			physicsBodyComponent->AddImpulse(adjustingLinearImpulse, NAME_None, true);
			FVector adjustingAngularImpulse = physicsBodyComponent->GetPhysicsAngularVelocity() * ratio - physicsBodyComponent->GetPhysicsAngularVelocity();
			physicsBodyComponent->AddAngularImpulse(FVector::DegreesToRadians(adjustingAngularImpulse), NAME_None, true);

			// should sync to last frame's time dilation since none of the forces or impulse affect the values of GetPhysicsLinearVelocity()
			realLinearVelocity = physicsBodyComponent->GetPhysicsLinearVelocity() / lastFrameTimeDilation;
			realAngularVelocity = physicsBodyComponent->GetPhysicsAngularVelocity() / lastFrameTimeDilation;

			realLinearVelocity = realLinearVelocity.GetClampedToMaxSize(UTimePhysics::defaultTerminalVelocity);
		}

		//adds gravity to current
		physicsBodyComponent->AddImpulse(FVector(0, 0, UTimePhysics::defaultGravityZ) * timeObjectComponent->GetCurrentTimeDilation() * DeltaTime, NAME_None, true);

		bTimeWasForward = false;
	}
	else
	{
		FreezePhysicsBody();
	}

	lastFrameTimeDilation = timeObjectComponent->GetCurrentTimeDilation();
	
}

void UTimePhysics::TrackVelocities()
{
	//linear velocity
	FVector* realLinearAddr = &realLinearVelocity;
	timeObjectComponent->AddTrackedData<FVector>(
		physicsBodyComponent,
		timeObjectComponent->GetLocalTime(),
		[realLinearAddr]() { return *realLinearAddr; },
		[realLinearAddr](FVector newVal) { *realLinearAddr = newVal; },
		60,
		true
		);

	//angular velocity
	FVector* realAngularAddr = &realAngularVelocity;
	timeObjectComponent->AddTrackedData<FVector>(
		physicsBodyComponent,
		timeObjectComponent->GetLocalTime(),
		[realAngularAddr]() { return *realAngularAddr; },
		[realAngularAddr](FVector newVal) { *realAngularAddr = newVal; },
		60,
		true
		);
}

void UTimePhysics::ClearForcesAndImpulses()
{
#if WITH_PHYSX
	PxRigidBody* rigidBody = physicsBodyComponent->BodyInstance.GetPxRigidBody();
	rigidBody->clearForce(PxForceMode::eFORCE);
	rigidBody->clearForce(PxForceMode::eIMPULSE);
	rigidBody->clearTorque(PxForceMode::eFORCE);
	rigidBody->clearTorque(PxForceMode::eIMPULSE);
#endif
}

void UTimePhysics::FreezePhysicsBody()
{
	physicsBodyComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	physicsBodyComponent->SetPhysicsAngularVelocity(FVector::ZeroVector);
	ClearForcesAndImpulses();

	bTimeWasForward = true;
}

