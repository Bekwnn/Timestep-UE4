// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TimeHistory.h"
#include <functional>
#include "TimeObject.generated.h"

/**
 * 
 */
//base class to ignore TrackerData template type
class TrackerData
{
public:
	virtual void ForwardUpdate() = 0;
	virtual void RewindUpdate() = 0;
	virtual bool RefObjectIsValid() = 0;
};

template<typename T, typename RefObj>
class TrackerDataImpl : public TrackerData
{
	TWeakPtr<RefObj> objectReference; //object reference which lets us check if we should delete the tracker data
	TimeHistory<T> timeHistory;
	std::function<T()> getValueFunc; //used to get the current value in the event of recording (ie ForwardUpdate)
	std::function<void(T)> setValueFunc; //used to set the current value in the event of rewinding (ie RewindUpdate)
public:
	TrackerDataImpl(TWeakPtr<RefObj> checkDeleteObject, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond = 60, bool bInterpolate = false);
	void ForwardUpdate();
	void RewindUpdate();
	bool RefObjectIsValid();
};

UCLASS()
class TIMESTEP_API UTimeObject : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimeObject();
	~UTimeObject();

	void SetTimeDilation(float val) { timeDilation = val; }
	float GetTimeDilation() { return timeDilation; }

	float GetTime() { return time; }

	// given a getter and setter function for a value, will record and rewind the value
	template<typename T, typename RefObj>
	void AddTrackedData(TWeakPtr<RefObj> deleteCheckRef, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond = 60, bool bInterpolate = false);

	virtual void TickComponent(float DeltaTime, 
		enum ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsLocalTimeDilation;

protected:
	static float globalTimeDilation; //the (fake) global time dilation, not to be confused with UE4's own one
	float timeDilation; //this actor's local time scale
	float time; //this actor's local time value

	TArray<TrackerData*> dataHistories; //a dynamic array of all the data being tracked by this object's timeline
};
