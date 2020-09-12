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
void ASplinePathActor::RemovePoint (const int Index)
{
	if(IsValidPointIndex(Index))
	{
		const FScopedTransaction Transaction (FText::FromString ("Remove Point"));
		Modify ();
		if (Index >= GetSplinePointCount())
		{
			PathPoints.RemoveAt (Index);
			return;
		}
		PathPoints.RemoveAt (Index);
		SplinePointCount--;
		if (Index != 0)
		{
			RecalculateLength (Index - 1);
		}
	}
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
//获取在整个曲线路径中的位置，Percent是整体百分比
FVector ASplinePathActor::GetPositionOnCurve(const float Percent)
{
	//总长度*百分比计算出目标长度
	const float TargetLength = Length * Percent;
	float SearchLength = 0;
	for(int i = 0; i < GetSplinePointCount(); i++)
	{
		if (i == GetSplinePointCount() - 1 && !IsLoop)
		{//如果非Loop状态下，查找到最后一个点，则直接返回最后一个点位置。理论上不应该计算到这个位置，因为总长度不包含这段。
			return PathPoints[GetSplinePointCount () - 1]->Position;
		}
		//每次查找增加一个节点的长度值
		SearchLength += PathPoints[i]->Length;
		if (SearchLength > TargetLength)
		{//在i起始的这个段内
			const float SegmentPercent = 1 - PathPoints[i]->Length / (SearchLength - TargetLength);
			return GetPositionOnSegment (i, SegmentPercent);
		}
	}
	
	return FVector::ZeroVector;
}

//------------------------------------------------------------------------
//获取在一个路径段内的位置，Index为Start点的索引，Percent是到下一个点的百分比
FVector ASplinePathActor::GetPositionOnSegment(const int Index, const float Percent)
{
	if (!IsValidPointIndex(Index))
	{
		return FVector::ZeroVector;
	}
	if (!IsLoop && Index == GetSplinePointCount () - 1)
	{
		//非Loop状态，最后一个节点直接返回该节点位置
		return PathPoints[Index]->Position;
	}

	const UPathPoint* Start = PathPoints[Index];
	const UPathPoint* End = Index == GetSplinePointCount () - 1 && IsLoop ? PathPoints[0] : PathPoints[Index + 1];
	return BezierPoint (Start->Position, End->Position, Start->OutCtrlPoint, End->InCtrlPoint, Percent);
}

//------------------------------------------------------------------------
void ASplinePathActor::RecalculateLength (const int Index)
{
	if (Index >=0 && Index < GetSplinePointCount())
	{
		const UPathPoint* Start = PathPoints[Index];
		//如果是最后一个曲线节点，也要计算到第一个节点的路线长度
		const UPathPoint* End = Index == GetSplinePointCount () - 1 ? PathPoints[0] : PathPoints[Index + 1];
		PathPoints[Index]->Length = BezierLength (Start->Position, End->Position, Start->OutCtrlPoint, End->InCtrlPoint);
	}

	//计算整条线路的总长度
	for(int i = 0; i < GetSplinePointCount(); i++)
	{
		if (i == GetSplinePointCount() - 1 && !IsLoop)
		{
			break;
		}
		Length += PathPoints[i]->Length;
	}
}

//------------------------------------------------------------------------
void ASplinePathActor::ToggleLoop ()
{
	const FScopedTransaction Transaction (FText::FromString ("Toggle Spline Loop"));
	Modify ();
	IsLoop = !IsLoop;
	if (IsLoop)
	{
		const int Index = GetSplinePointCount () - 1;
		//如果控制点没有初始化过则需要重新设置
		UPathPoint* StartPoint = PathPoints[Index];
		UPathPoint* EndPoint = PathPoints[0];
		FVector Dir = EndPoint->Position - StartPoint->Position;
		Dir.Normalize ();
		Dir *= FVector::Distance (EndPoint->Position, StartPoint->Position) / 4;
		if (StartPoint->OutCtrlPoint.IsZero())
		{
			StartPoint->OutCtrlPoint = StartPoint->Position + Dir;
		}

		if (EndPoint->InCtrlPoint.IsZero())
		{
			EndPoint->InCtrlPoint = EndPoint->Position - Dir;
		}
		//Loop模式，重新计算最后一个节点到头节点的长度
		RecalculateLength (Index);
	}
}
