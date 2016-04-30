// Fill out your copyright notice in the Description page of Project Settings.

#include "Timestep.h"
#include "TimeObject.h"

// TrackerDataImpl BEGINS
template<typename T, typename RefObj>
TrackerDataImpl<T, RefObj>::TrackerDataImpl(TWeakPtr<RefObj> checkDeleteObject, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond, bool bInterpolate) :
	getValueFunc(getter),
	setValueFunc(setter)
{
	objectReference = new TWeakPtr<RefObj>();
	timeHistory = TimeHistory<T>(getter(), snapshotsPerSecond, bInterpolate);
}

template<typename T, typename RefObj>
void TrackerDataImpl<T, RefObj>::ForwardUpdate()
{
	TimeHistory::ForwardUpdate(timeHistory, time, getValueFunc());
}

template<typename T, typename RefObj>
void TrackerDataImpl<T, RefObj>::RewindUpdate()
{
	setValueFunc(TimeHistory::RewindUpdate(timeHistory, time));
}

template<typename T, typename RefObj>
bool TrackerDataImpl<T, RefObj>::RefObjectIsValid()
{
	return objectReference.IsValid();
}

// TimeObject BEGINS
float UTimeObject::globalTimeDilation = 1.0f;

UTimeObject::UTimeObject()
{
}

UTimeObject::~UTimeObject()
{
}

template<typename T, typename RefObj>
void UTimeObject::AddTrackedData(TWeakPtr<RefObj> deleteCheckRef, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond, bool bInterpolate)
{
	dataHistories.Add(new TrackerDataImpl<T, RefObj>(deleteCheckRef, getter, setter, snapshotsPerSecond, bInterpolate) );
}

void UTimeObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	for (TrackerData* trackerData : dataHistories)
	{
		if (!trackerData->RefObjectIsValid())
		{
			//remove trackerData from dataHistories and continue the loop
			continue;
		}

		float timeScale = (bIsLocalTimeDilation) ? timeDilation : globalTimeDilation;

		if (timeScale > 0.f)
		{
			trackerData->ForwardUpdate();
		}
		else if (timeScale < 0.f)
		{
			trackerData->RewindUpdate();
		}
	}
}
