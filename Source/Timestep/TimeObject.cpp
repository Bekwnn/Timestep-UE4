// Fill out your copyright notice in the Description page of Project Settings.

#include "Timestep.h"
#include "TimeObject.h"

// TimeObject BEGINS
float UTimeObject::globalTimeDilation = 1.0f;

UTimeObject::UTimeObject()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	bIsLocalTimeDilation = false;
}

UTimeObject::~UTimeObject()
{
}

void UTimeObject::TrackActorTransform()
{
	AActor* owner = GetOwner();
	AddTrackedData<FVector>(
		owner,
		GetLocalTime(),
		[owner]()               { return owner->GetActorLocation(); },
		[owner](FVector newVal) { owner->SetActorLocation(newVal); },
		60,
		true
		);
	AddTrackedData<FQuat>(
		owner,
		GetLocalTime(),
		[owner]()             { return owner->GetActorQuat(); },
		[owner](FQuat newVal) { owner->SetActorRotation(newVal); },
		60,
		true
		);
}

void UTimeObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float timeScale = (bIsLocalTimeDilation) ? localTimeDilation : globalTimeDilation;

	localTime = std::fmax(0.f, localTime + DeltaTime * timeScale);

	for (TrackerData* trackerData : dataHistories)
	{
		if (timeScale > 0.f)
		{
			trackerData->ForwardUpdate(localTime);
		}
		else if (timeScale < 0.f)
		{
			trackerData->RewindUpdate(localTime);
		}
	}
}
