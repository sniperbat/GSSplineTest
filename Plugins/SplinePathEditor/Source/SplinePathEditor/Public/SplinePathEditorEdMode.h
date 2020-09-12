// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"
#include "SplinePathActor.h"

//---------------------------------------------------------------------------------------------------
struct HSplinePositionProxy : HHitProxy
{
	DECLARE_HIT_PROXY ();
	HSplinePositionProxy(ASplinePathActor* PathActor, int Index): HHitProxy(HPP_Wireframe)
	{
		this->PathActor = PathActor;
	    this->Index = Index;
	}

	ASplinePathActor* PathActor;
	int Index;
};

//---------------------------------------------------------------------------------------------------
struct HSplineControlPointProxy : HSplinePositionProxy
{
	DECLARE_HIT_PROXY ();
	HSplineControlPointProxy(ASplinePathActor* PathActor, int Index, bool IsIn) : HSplinePositionProxy (PathActor, Index)
	{
        this->IsIn = IsIn;
	}
	
	bool IsIn;
};

//---------------------------------------------------------------------------------------------------
struct HSplineSegmentProxy : HHitProxy
{
    DECLARE_HIT_PROXY ();
    HSplineSegmentProxy () : HHitProxy (HPP_Wireframe)
    {
    }
};

//---------------------------------------------------------------------------------------------------

class FSplinePathEditorEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_SplinePathEditorEdModeId;

private:
    UPROPERTY ()
    TArray<ASplinePathActor*> PathList;

public:
	FSplinePathEditorEdMode ();
	virtual ~FSplinePathEditorEdMode ();

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;

	virtual bool InputKey (FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool HandleClick (FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual void ActorSelectionChangeNotify() override;
	virtual bool UsesToolkits() const override;

	virtual bool ShowModeWidgets () const override;
	virtual bool ShouldDrawWidget () const override;
	virtual bool UsesTransformWidget () const override;
	virtual FVector GetWidgetLocation () const override;
	virtual bool InputDelta (FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	// End of FEdMode interface

	// FEdMode Public Method for Toolkit
	void AddPath ();
	void RemovePath ();

	void AddPoint (const FVector& Position) const;
	void RemovePoint (int32 Index);

	void MakeSplineCurve() const;

    void ToggleShowAll(bool IsOn);
	void ToggleUniteCtrlPoint (bool IsOn);
	void ToggleCanAddPoint (bool IsOn);
    // End of Public Method for Toolkit
};
