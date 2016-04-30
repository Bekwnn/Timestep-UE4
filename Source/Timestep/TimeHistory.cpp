// Fill out your copyright notice in the Description page of Project Settings.

#include "Timestep.h"
#include "TimeHistory.h"

template<class T>
TimeHistory<T>::TimeHistory(T value, int snapshotsPerSecond, bool bInterpolate) :
	snapshotInterval(1.f/snapshotsPerSecond),
	bInterpolate(bInterpolate)
{

}

template<class T>
TimeHistory<T>::~TimeHistory()
{
}

//TODO: takes lambda expression returning T for value
template<class T>
void TimeHistory<T>::ForwardUpdate(TimeHistory<T>& timeHistory, float time, T value)
{
	if (time - timeHistory.history[timeHistory.history.Num() - 1].time > timeHistory.snapshotInterval)
		timeHistory.history.Emplace(value, time);

	//TODO: if value is same and history is not interpolated, just update timestamp of most recent
}

template<class T>
TimeInfo<T> TimeHistory<T>::RewindTo(TimeHistory<T>& timeHistory, float time)
{
	float clampedTime = (time >= 0.f) ? time : 0.f;

	if (timeHistory.bInterpolate)
	{
		auto& hist = timeHistory.history;
		while (hist.Num() > 1 && hist[hist.Num()-2].time >= clampedTime)
		{
			hist.Pop();
		}

		float lerpval = (clampedTime - hist[hist.Num()-2].time)/(hist[hist.Num()-1].time - hist[hist.Num()-2].time);
		T interpolatedData = (1.f-lerpval)*hist[hist.Num()-2].data + lerpval*hist[hist.Num()-1].data;
		
		return TimeInfo<T>(interpolatedData, clampedTime);
	}
	else
	{
		auto& hist = timeHistory.history;
		while (hist.Num() > 1 && hist[hist.Num() - 2].time >= clampedTime)
		{
			hist.Pop();
		}

		hist[hist.Num()-1].time = clampedTime;
		return TimeInfo<T>(hist[hist.Num()-1].data, hist[hist.Num()-1].time);
	}
}
