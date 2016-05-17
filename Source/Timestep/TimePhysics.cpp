// Fill out your copyright notice in the Description page of Project Settings.

#include "Timestep.h"
#include "TimePhysics.h"

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

	// ...
}


// Called when the game starts
void UTimePhysics::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTimePhysics::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (timeObjectComponent->GetCurrentTimeDilation() > 0.f)
	{
		if (bTimeWasForward)
		{
			physicsBodyComponent->SetPhysicsLinearVelocity(realLinearVelocity * timeObjectComponent->GetCurrentTimeDilation());
			physicsBodyComponent->SetPhysicsAngularVelocity(realAngularVelocity * timeObjectComponent->GetCurrentTimeDilation());
		}
		else
		{
			//floating point errors significant?
			float ratio = timeObjectComponent->GetCurrentTimeDilation() / lastFrameTimeDilation;
			
			FVector adjustingLinearImpulse = physicsBodyComponent->GetPhysicsLinearVelocity() - physicsBodyComponent->GetPhysicsLinearVelocity() * ratio;
			physicsBodyComponent->AddImpulse(adjustingLinearImpulse, NAME_None, true);
			FVector adjustingAngularImpulse = physicsBodyComponent->GetPhysicsAngularVelocity() - physicsBodyComponent->GetPhysicsAngularVelocity() * ratio;
			physicsBodyComponent->AddAngularImpulse(adjustingAngularImpulse, NAME_None, true);
			//physicsBodyComponent->SetPhysicsLinearVelocity(physicsBodyComponent->GetPhysicsLinearVelocity() * ratio);
			//physicsBodyComponent->SetPhysicsAngularVelocity(physicsBodyComponent->GetPhysicsAngularVelocity() * ratio);

			realLinearVelocity = physicsBodyComponent->GetPhysicsLinearVelocity() / timeObjectComponent->GetCurrentTimeDilation();
			realAngularVelocity = physicsBodyComponent->GetPhysicsAngularVelocity() / timeObjectComponent->GetCurrentTimeDilation();

			realLinearVelocity = realLinearVelocity.GetClampedToMaxSize(UTimePhysics::defaultTerminalVelocity);
		}

		//adds gravity to current
		physicsBodyComponent->AddForce(FVector(0, 0, UTimePhysics::defaultGravityZ) * timeObjectComponent->GetCurrentTimeDilation(), NAME_None, true);

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

void UTimePhysics::FreezePhysicsBody()
{
	physicsBodyComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	physicsBodyComponent->SetPhysicsAngularVelocity(FVector::ZeroVector);

	bTimeWasForward = true;
}

