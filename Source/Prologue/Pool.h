// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/UnrealTemplate.h"

/**
 * 
 */
template<typename T>
class Pool
{
static_assert(TIsDerivedFrom<T, AActor>::IsDerived, "TPool<T> Type is not AActor children (sejin code)");
	
private:
	UWorld* World;
	TArray<T*> PoolArray;
	int32 Max;
	int32 Current;

	void Active(AActor* Target);
	void Deactive(AActor* Target);
	
public:
	Pool();
	Pool(UWorld* World, TSubclassOf<T> Origin, int32 Count);
	T* Pop();
	void Return(T* Target);
};

template <typename T>
void Pool<T>::Active(AActor* Target)
{
	Target->SetActorHiddenInGame(false);
	Target->SetActorEnableCollision(true);
	Target->SetActorTickEnabled(true);
}

template <typename T>
void Pool<T>::Deactive(AActor* Target)
{
	Target->SetActorHiddenInGame(true);
	Target->SetActorEnableCollision(false);
	Target->SetActorTickEnabled(false);
}

template <typename T>
Pool<T>::Pool()
	: Max(0)
{
}

template <typename T>
Pool<T>::Pool(UWorld* World, TSubclassOf<T> Origin, int32 Count)
	: World(World), Max(Count), Current(0)
{
	if (World && *Origin)
	{
		PoolArray.Reserve(Count);
	
		for (int32 i = 0; i < Count; i++)
		{
			T* _Obj = World->SpawnActor<T>(Origin, FTransform::Identity);
			Deactive(_Obj);
			PoolArray.Add(_Obj);
		}
	}
}

template <typename T>
T* Pool<T>::Pop()
{
	T* Target = PoolArray[Current];
	Current = (Current + 1) % Max;
	Active(Target);
	return Target;
}

template <typename T>
void Pool<T>::Return(T* Target)
{
	if (!Target) return;

	Deactive(Target);
}
