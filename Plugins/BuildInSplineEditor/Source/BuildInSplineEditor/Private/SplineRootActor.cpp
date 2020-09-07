// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineRootActor.h"

// Sets default values
ASplineRootActor::ASplineRootActor(const FObjectInitializer& ObjectInitializer)
{
	this->SplineComponent = ObjectInitializer.CreateDefaultSubobject <USplineComponent> (this, TEXT ("Spline"));
	RootComponent = this->SplineComponent;
 	
	PrimaryActorTick.bCanEverTick = false;

}

bool ASplineRootActor::IsEditorOnly () const
{
	return true;
}

