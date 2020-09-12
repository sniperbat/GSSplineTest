// Fill out your copyright notice in the Description page of Project Settings.

#include "UPathPoint.h"

//------------------------------------------------------------------------
void UPathPoint::MovePosition (const FVector& InDelta)
{
	const FScopedTransaction Transaction (FText::FromString ("Move Path Point Position"));
    Modify ();
    Position += InDelta;
    InCtrlPoint += InDelta;
    OutCtrlPoint += InDelta;
}

//------------------------------------------------------------------------
void UPathPoint::MoveInCtrl (const FVector& InDelta, const bool IsUniteOut)
{
	const FScopedTransaction Transaction (FText::FromString ("Move In Ctrl Point"));
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
void UPathPoint::MoveOutCtrl (const FVector& InDelta, const bool IsUniteIn)
{
	const FScopedTransaction Transaction (FText::FromString ("Move Out Ctrl Point"));
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
