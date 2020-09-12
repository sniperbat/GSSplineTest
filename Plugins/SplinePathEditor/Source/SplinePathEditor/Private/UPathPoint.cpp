// Fill out your copyright notice in the Description page of Project Settings.

#include "UPathPoint.h"

//------------------------------------------------------------------------
void UPathPoint::MovePosition (const FVector& InDelta)
{
    Modify ();
    Position += InDelta;
    InCtrlPoint += InDelta;
    OutCtrlPoint += InDelta;
}

//------------------------------------------------------------------------
void UPathPoint::MoveInCtrl (const FVector& InDelta, bool IsUniteOut)
{
    Modify ();
    InCtrlPoint += InDelta;
	if (IsUniteOut)
	{
		FVector Dir = Position - InCtrlPoint;
		Dir.Normalize ();
		OutCtrlPoint = Position + Dir * FVector::Distance (OutCtrlPoint, Position);
	}
}

//------------------------------------------------------------------------
void UPathPoint::MoveOutCtrl (const FVector& InDelta, bool IsUniteIn)
{
    Modify ();
    OutCtrlPoint += InDelta;
	if (IsUniteIn)
	{
		FVector Dir = Position - OutCtrlPoint;
		Dir.Normalize ();
		InCtrlPoint = Position + Dir * FVector::Distance (InCtrlPoint, Position);
	}
}

//------------------------------------------------------------------------
