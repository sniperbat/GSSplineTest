// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UPathPoint.h"
#include "SplinePathActor.generated.h"

UCLASS()
class SPLINEPATHEDITOR_API ASplinePathActor : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplinePathActor();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	void AddPoint(const FVector& PointPos);

	void InitControlPoints();
	void MakeSplineCurve();

	int GetTotalPointCount();
	int GetSplinePointCount();
	int GetAlternatePointCount();

public:
	UPROPERTY (EditAnywhere, Category = "Path Points")
	TArray<UPathPoint*> PathPoints;

	UPROPERTY (EditAnywhere, Category = "Path Points")
	int SplinePointCount;

};
