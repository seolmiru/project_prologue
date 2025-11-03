// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prologue.h"
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
	TSubclassOf<T> OriginRef;
	TQueue<T*> PoolQueue;

	// void Active(AActor* Target);
	// void Deactive(AActor* Target);
	
public:
	Pool();
	Pool(UWorld* World, TSubclassOf<T> Origin, int32 Count);
	T* Pop();
	void Return(T* Target);
};

/*template <typename T>
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
}*/

template <typename T>
Pool<T>::Pool()
{
}

template <typename T>
Pool<T>::Pool(UWorld* World, TSubclassOf<T> Origin, int32 Count)
	: World(World), OriginRef(Origin)
{
	if (World && *Origin)
	{
		for (int32 i = 0; i < Count; i++)
		{
			T* _Obj = World->SpawnActor<T>(Origin, FTransform::Identity);
			// Deactive(_Obj);
			PoolQueue.Enqueue(_Obj);
		}
	}
}

template <typename T>
T* Pool<T>::Pop()
{
	if (PoolQueue.IsEmpty())
	{
		T* _Obj = World->SpawnActor<T>(OriginRef, FTransform::Identity);
		LOG_SCREEN("Add Pool Object");
		return _Obj;
	}
	
	T* Target;
	PoolQueue.Dequeue(Target);
	// Active(Target);
	return Target;
}

template <typename T>
void Pool<T>::Return(T* Target)
{
	if (!Target) return;

	// Deactive(Target);
	PoolQueue.Enqueue(Target);
}
