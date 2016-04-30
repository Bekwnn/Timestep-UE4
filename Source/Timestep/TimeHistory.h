// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>

/**
 * 
 */
template<class T>
class TIMESTEP_API TimeInfo
{
public:
	TimeInfo(T value, float time);

	T data;
	float time;
};

template<class T>
inline TimeInfo<T>::TimeInfo(T value, float time)
{
	data = value;
	this->time = time;
}

template<class T>
class TIMESTEP_API TimeHistory
{
public:
	TimeHistory(T value, int snapshotsPerSecond, bool bInterpolate = false);
	~TimeHistory();

	static void ForwardUpdate(TimeHistory<T>& history, float time, T value);
	static TimeInfo<T> RewindTo(TimeHistory<T>& history, float time);

protected:
	TArray<TimeInfo<T>> history;
	float snapshotInterval;
};
