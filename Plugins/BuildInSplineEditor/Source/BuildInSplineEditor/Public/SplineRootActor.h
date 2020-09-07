// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SplineRootActor.generated.h"

UCLASS()
class BUILDINSPLINEEDITOR_API ASplineRootActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineRootActor (const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY ()
	USplineComponent* SplineComponent;

public:	

	virtual bool IsEditorOnly() const override;
	
};
