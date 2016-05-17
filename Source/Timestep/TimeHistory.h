// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>

//CLASS DECLARATIONS BEGIN
template<typename T>
class TIMESTEP_API TimeInfo
{
public:
	TimeInfo(T value, float time);

	T data;
	float time;
};

template<typename T>
class TIMESTEP_API TimeHistory
{
public:
	TimeHistory(T value, float time, int snapshotsPerSecond);
	~TimeHistory();

	void ForwardUpdate(float time, T value);
	virtual TimeInfo<T> RewindTo(float time);

protected:
	TArray<TimeInfo<T>> history;
	float snapshotInterval;
	bool bInterpolate;
};

template<typename T>
class TIMESTEP_API TimeHistoryInterp : public TimeHistory<T>
{
public:
	TimeHistoryInterp(T value, float time, int snapshotsPerSecond);
	virtual TimeInfo<T> RewindTo(float time) override;
};

//CLASS DECLARATIONS END
//CLASS DEFINITIONS BEGIN
template<typename T>
inline TimeInfo<T>::TimeInfo(T value, float time) :
	data(value),
	time(time)
{
}

template<typename T>
TimeHistory<T>::TimeHistory(T value, float time, int snapshotsPerSecond) :
	snapshotInterval(1.f / snapshotsPerSecond)
{
	history.Emplace(TimeInfo<T>(value, time));
}

template<typename T>
TimeHistory<T>::~TimeHistory()
{
}

template<typename T>
void TimeHistory<T>::ForwardUpdate(float time, T value)
{
	if (time - history[history.Num() - 1].time > snapshotInterval)
	{
		history.Emplace(TimeInfo<T>(value, time));
	}
}

template<typename T>
TimeInfo<T> TimeHistory<T>::RewindTo(float time)
{
	float clampedTime = (time >= 0.f) ? time : 0.f;

	while (history.Num() > 1 && history[history.Num() - 2].time >= clampedTime)
	{
		history.Pop();
	}

	history[history.Num() - 1].time = clampedTime;

	return TimeInfo<T>(history[history.Num() - 1].data, history[history.Num() - 1].time);
}

template<typename T>
TimeHistoryInterp<T>::TimeHistoryInterp(T value, float time, int snapshotsPerSecond) :
	TimeHistory<T>(value, time, snapshotsPerSecond)
{
	//adding a second TimeInfo because min in the array for interpolation is 2
	history.Emplace(TimeInfo<T>(value, time));
}

template<typename T>
inline TimeInfo<T> TimeHistoryInterp<T>::RewindTo(float time)
{
	float clampedTime = (time >= 0.f) ? time : 0.f;

	while (history.Num() > 2 && history[history.Num() - 2].time >= clampedTime)
	{
		history.Pop();
	}

	float lerpval = (clampedTime - history[history.Num() - 2].time) / (history[history.Num() - 1].time - history[history.Num() - 2].time);
	lerpval = std::fmin(1.0f, std::fmax(0.0f, lerpval));

	T interpolatedData = history[history.Num() - 2].data * (1.f - lerpval) + history[history.Num() - 1].data * lerpval;

	return TimeInfo<T>(interpolatedData, time);
}

// Quaternions may lerp the wrong direction (ie 350 degrees instead of 10) this is to check and always lerp shortest path
template<>
inline TimeInfo<FQuat> TimeHistoryInterp<FQuat>::RewindTo(float time)
{
	float clampedTime = (time >= 0.f) ? time : 0.f;

	while (history.Num() > 2 && history[history.Num() - 2].time >= clampedTime)
	{
		history.Pop();
	}

	float lerpval = (clampedTime - history[history.Num() - 2].time) / (history[history.Num() - 1].time - history[history.Num() - 2].time);
	lerpval = std::fmin(1.0f, std::fmax(0.0f, lerpval));

	FQuat interpolatedData = FQuat::Slerp(history[history.Num() - 2].data, history[history.Num() - 1].data, lerpval);

	return TimeInfo<FQuat>(interpolatedData, time);
}