// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASplinePathActor.generated.h"

USTRUCT()
struct FPathPoint
{
	GENERATED_BODY ()

	FVector Position;
	FVector InCtrlPoint;
	FVector OutCtrlPoint;
};

UCLASS()
class SPLINEPATHEDTIOR_API AASplinePathActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AASplinePathActor();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY (EditAnywhere, Category = "Path Points")
	TArray<FPathPoint> PathPoints;
};
