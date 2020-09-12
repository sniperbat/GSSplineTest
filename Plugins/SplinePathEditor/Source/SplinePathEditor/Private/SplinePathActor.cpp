// Fill out your copyright notice in the Description page of Project Settings.


#include "SplinePathActor.h"

//------------------------------------------------------------------------
FVector BezierPoint (const FVector& Start, const FVector& End, const FVector& StartOutCtrl, const FVector& EndInCtrl, float Delta)
{
	Delta = Delta < 0.0f ? 0.0f : Delta > 1.0f ? 1.0f : Delta;
	const FVector Temp = FMath::Lerp (StartOutCtrl, EndInCtrl, Delta);
	return FMath::Lerp (FMath::Lerp (FMath::Lerp (Start, StartOutCtrl, Delta), Temp, Delta),
		FMath::Lerp (Temp, FMath::Lerp (EndInCtrl, End, Delta), Delta), Delta);
}

//------------------------------------------------------------------------
float BezierLength (const FVector& Start, const FVector& End, const FVector& StartOutCtrl, const FVector& EndInCtrl) {
	const int Divide = 1000;
	const float Step = 1 / static_cast<float>(Divide);
	float Length = 0;
	for (int i = 0; i < Divide - 1; i++) {
		FVector ThisPoint = BezierPoint (Start, End, StartOutCtrl, EndInCtrl, i * Step);
		FVector NextPoint = BezierPoint (Start, End, StartOutCtrl, EndInCtrl, (i + 1) * Step);
		Length += FVector::Distance (ThisPoint, NextPoint);
	}
	return Length;
}

//------------------------------------------------------------------------
ASplinePathActor::ASplinePathActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

//------------------------------------------------------------------------
void ASplinePathActor::AddPoint(const FVector& PointPos)
{
	const FScopedTransaction Transaction(FText::FromString("Add Point"));
	UPathPoint* Point = NewObject<UPathPoint> ();
	Point->SetFlags(RF_Transactional);
	Point->Position = PointPos;
	Modify();
	PathPoints.Add(Point);
}

//------------------------------------------------------------------------
void ASplinePathActor::InitControlPoints()
{
	int Start = GetSplinePointCount() - 1; //从曲线最后一个节点开始计算
	if (Start < 0)
	{
		Start = 0;
	}
	for (int i = Start; i < GetTotalPointCount() - 1; i++)
	{
		UPathPoint* StartPoint = PathPoints[i];
		UPathPoint* EndPoint = PathPoints[i + 1];
		FVector Dir = EndPoint->Position - StartPoint->Position;
		Dir.Normalize();
		Dir *= FVector::Distance (EndPoint->Position, StartPoint->Position) / 4;
		StartPoint->OutCtrlPoint = StartPoint->Position + Dir;
		EndPoint->InCtrlPoint = EndPoint->Position - Dir;
		RecalculateLength (i);
	}
}

//------------------------------------------------------------------------
void ASplinePathActor::MakeSplineCurve()
{
	const FScopedTransaction Transaction (FText::FromString ("MakeSplineCurve"));
	Modify();
	const int TotalPointCount = GetTotalPointCount();
	if (TotalPointCount > SplinePointCount)
	{
		InitControlPoints();
		SplinePointCount = TotalPointCount;
	}
}

//------------------------------------------------------------------------
int ASplinePathActor::GetTotalPointCount() const
{
	return PathPoints.Num();
}

//------------------------------------------------------------------------
int ASplinePathActor::GetSplinePointCount() const
{
	return SplinePointCount;
}

//------------------------------------------------------------------------
int ASplinePathActor::GetAlternatePointCount() const
{
	return GetTotalPointCount() - GetSplinePointCount();
}

//------------------------------------------------------------------------
bool ASplinePathActor::IsValidPointIndex(const int Index) const
{
	return Index >= 0 && Index < GetTotalPointCount();
}

//------------------------------------------------------------------------
FVector ASplinePathActor::GetPositionOnCurve(float Percent)
{
	return FVector::ZeroVector;
}

//------------------------------------------------------------------------
FVector ASplinePathActor::GetPositionOnSegment(const int Index, float Percent)
{
	if (!IsValidPointIndex(Index))
	{
		return FVector::ZeroVector;
	}
	if (Index == GetSplinePointCount() - 1)
	{
		//如果最后一个节点直接返回该节点位置
		return PathPoints[Index]->Position;
	}
	const UPathPoint* Start = PathPoints[Index];
	const UPathPoint* End = PathPoints[Index + 1];
	return BezierPoint (Start->Position, End->Position, Start->OutCtrlPoint, End->InCtrlPoint, Percent);
}

//------------------------------------------------------------------------
void ASplinePathActor::RecalculateLength (const int Index)
{
	if (Index >=0 && Index < GetSplinePointCount() - 1)
	{
		const UPathPoint* Start = PathPoints[Index];
		const UPathPoint* End = PathPoints[Index + 1];
		PathPoints[Index]->Length = BezierLength (Start->Position, End->Position, Start->OutCtrlPoint, End->InCtrlPoint);
	}
}

//------------------------------------------------------------------------
