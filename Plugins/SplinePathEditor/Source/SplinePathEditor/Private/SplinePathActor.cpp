// Fill out your copyright notice in the Description page of Project Settings.


#include "SplinePathActor.h"

//------------------------------------------------------------------------
const float DefaultControlLineLength = 20;

//------------------------------------------------------------------------
ASplinePathActor::ASplinePathActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------
void ASplinePathActor::AddPoint(const FVector& PointPos)
{
    const FScopedTransaction Transaction(FText::FromString("Add Point"));
    UPathPoint* Point = NewObject<UPathPoint>();
    Point->Position = PointPos;
    Modify();
    PathPoints.Add (Point);
}

//------------------------------------------------------------------------
void ASplinePathActor::InitControlPoints()
{
    int Start = GetSplinePointCount() - 1;//从最后一个节点开始计算
    if (Start < 0) {
        Start = 0;
    }
    for(int i = Start; i < GetTotalPointCount() - 1; i++) {
        UPathPoint* StartPoint = PathPoints[i];
        UPathPoint* EndPoint = PathPoints[i + 1];
        FVector Dir = EndPoint->Position - StartPoint->Position;
        float Distance = FVector::Distance(EndPoint->Position, StartPoint->Position);
        Distance = Distance > (DefaultControlLineLength * 3) ? DefaultControlLineLength : Distance / 3;
        Dir.Normalize();
        Dir *= Distance;
        StartPoint->OutCtrlPoint = StartPoint->Position + Dir;
        EndPoint->InCtrlPoint = EndPoint->Position - Dir;
    }
}

//------------------------------------------------------------------------
void ASplinePathActor::MakeSplineCurve()
{
    InitControlPoints();
    SplinePointCount = GetTotalPointCount();
}

//------------------------------------------------------------------------
int ASplinePathActor::GetTotalPointCount()
{
    return PathPoints.Num();
}

//------------------------------------------------------------------------
int ASplinePathActor::GetSplinePointCount()
{
    return SplinePointCount;
}

//------------------------------------------------------------------------
int ASplinePathActor::GetAlternatePointCount()
{
    return GetTotalPointCount() - GetSplinePointCount();
}
