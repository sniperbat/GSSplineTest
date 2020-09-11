// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UPathPoint.generated.h"

UCLASS()
class SPLINEPATHEDITOR_API UPathPoint : public UObject
{
	GENERATED_BODY()
public:
    UPROPERTY (EditAnywhere, Category = "Path Points")
    FVector Position;

	UPROPERTY (EditAnywhere, Category = "Path Points")
    FVector InCtrlPoint;

    UPROPERTY (EditAnywhere, Category = "Path Points")
    FVector OutCtrlPoint;
};
