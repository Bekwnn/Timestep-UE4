// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TimeHistory.h"
#include <functional>
#include "TimeObject.generated.h"

//CLASS DECLARATIONS BEGIN
//base class to ignore TrackerData template type
class TrackerData
{
public:
	virtual void ForwardUpdate(float time) = 0;
	virtual void RewindUpdate(float time) = 0;
	virtual bool RefObjectIsValid() = 0;
};

template<typename T>
class TrackerDataImpl : public TrackerData
{
public:
	TrackerDataImpl(const UObject* checkDeleteObject, float time, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond = 60, bool bInterpolate = false);
	~TrackerDataImpl();
	void ForwardUpdate(float time);
	void RewindUpdate(float time);
	bool RefObjectIsValid();

	TWeakObjectPtr<UObject> objectReference; //object reference which lets us check if we should delete the tracker data
	TimeHistory<T>* timeHistory;
	std::function<T()>     getValueFunc; //used to get the current value in the event of recording (ie ForwardUpdate)
	std::function<void(T)> setValueFunc; //used to set the current value in the event of rewinding (ie RewindUpdate)
};

UCLASS(Blueprintable)
class TIMESTEP_API UTimeObject : public UActorComponent
{
	GENERATED_BODY()

public:
	UTimeObject();
	~UTimeObject();

	UFUNCTION(BlueprintCallable, category = "Time Manipulation")
	float GetTime() { return localTime; }

	UFUNCTION(BlueprintCallable, category = "Time Manipulation")
	void TrackActorTransform();

	// given a getter and setter function for a value, will record and rewind the value
	template<typename T>
	void AddTrackedData(const UObject* deleteCheckRef, float time, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond = 60, bool bInterpolate = false);

	virtual void TickComponent(float DeltaTime, 
		enum ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, category = "Time Manipulation")
	bool bIsLocalTimeDilation;

	UFUNCTION(BlueprintCallable, category = "Time Manipulation")
	static float GetGlobalTimeDilation() { return UTimeObject::globalTimeDilation; }

	UFUNCTION(BlueprintCallable, category = "Time Manipulation")
	static void SetGlobalTimeDilation(float val) { UTimeObject::globalTimeDilation = std::fmin(1.0f, std::fmax(-1.0f, val)); }

	UFUNCTION(BlueprintCallable, category = "Time Manipulation")
	float GetLocalTimeDilation() { return localTimeDilation; }

	UFUNCTION(BlueprintCallable, category = "Time Manipulation")
	void SetLocalTimeDilation(float val) { localTimeDilation = std::fmin(1.0f, std::fmax(-1.0f, val)); }

protected:
	static float globalTimeDilation; //the (fake) global time dilation, not to be confused with UE4's own one
	float localTimeDilation; //this actor's local time scale
	float localTime; //this actor's local time value

	TArray<TrackerData*> dataHistories; //a dynamic array of all the data being tracked by this object's timeline
};

//CLASS DECLARATIONS END
//CLASS DEFINITIONS BEGIN
template<typename T>
TrackerDataImpl<T>::TrackerDataImpl(const UObject* checkDeleteObject, float time, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond, bool bInterpolate) :
	getValueFunc(getter),
	setValueFunc(setter)
{
	if (!bInterpolate)
		timeHistory = new TimeHistory<T>(getter(), time, snapshotsPerSecond);
	else
		timeHistory = new TimeHistoryInterp<T>(getter(), time, snapshotsPerSecond);

	objectReference = TWeakObjectPtr<UObject>(checkDeleteObject);
}

template<typename T>
TrackerDataImpl<T>::~TrackerDataImpl()
{
	delete timeHistory;
}

//TODO: delete record if function is unbound
template<typename T>
void TrackerDataImpl<T>::ForwardUpdate(float time)
{
	if (objectReference.IsValid())
	{
		timeHistory->ForwardUpdate(time, getValueFunc());
	}
}

template<typename T>
void TrackerDataImpl<T>::RewindUpdate(float time)
{
	if (objectReference.IsValid())
	{
		setValueFunc(timeHistory->RewindTo(time).data);
	}
}

template<typename T>
bool TrackerDataImpl<T>::RefObjectIsValid()
{
	return objectReference.IsValid();
}

template<typename T>
void UTimeObject::AddTrackedData(const UObject* deleteCheckRef, float time, std::function<T()> getter, std::function<void(T)> setter, int snapshotsPerSecond, bool bInterpolate)
{
	dataHistories.Add(new TrackerDataImpl<T>(deleteCheckRef, time, getter, setter, snapshotsPerSecond, bInterpolate));
}