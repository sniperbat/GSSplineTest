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
	ASplinePathActor();

	void AddPoint(const FVector& PointPos);
	void RemovePoint (int Index);

	void InitControlPoints();
	void MakeSplineCurve();
	void ToggleLoop ();

	bool IsValidPointIndex (int Index) const;

	int GetTotalPointCount() const;
	int GetSplinePointCount() const;
	int GetAlternatePointCount() const;

	void RecalculateLength (int Index);
	
	FVector GetPositionOnCurve (float Percent);
	FVector GetPositionOnSegment (int Index, float Percent);
	
	UPROPERTY (EditAnywhere, Category = "Path Points")
	TArray<UPathPoint*> PathPoints;

	UPROPERTY (EditAnywhere, Category = "Path Points")
	int SplinePointCount;

	UPROPERTY (EditAnywhere, Category = "Path Points")
	bool IsLoop;
	

};
