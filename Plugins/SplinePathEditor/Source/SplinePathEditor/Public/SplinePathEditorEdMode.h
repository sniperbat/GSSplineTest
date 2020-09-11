// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"
#include "SplinePathActor.h"

//---------------------------------------------------------------------------------------------------
struct HSplinePositionProxy : HHitProxy {
	DECLARE_HIT_PROXY ();
	HSplinePositionProxy(ASplinePathActor* PathActor, int Index): HHitProxy(HPP_Wireframe) {
		this->PathActor = PathActor;
	    this->Index = Index;
	}

	ASplinePathActor* PathActor;
	int Index;
};

//---------------------------------------------------------------------------------------------------
struct HSplineControlPointProxy : HHitProxy {
	DECLARE_HIT_PROXY ();
	HSplineControlPointProxy (ASplinePathActor* PathActor, int Index, bool IsIn) : HHitProxy (HPP_Wireframe) {
		this->PathActor = PathActor;
        this->Index = Index;
        this->IsIn = IsIn;
	}
	ASplinePathActor* PathActor;
    int Index;
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
    class UMaterialInstanceDynamic* PositionMaterial;

    UPROPERTY ()
    class UMaterialInstanceDynamic* SelectedPositionMaterial;

    UPROPERTY ()
    class UMaterialInstanceDynamic* ControlPointMaterial;

    UPROPERTY ()
    class UMaterialInstanceDynamic* SelectedControlPointMaterial;

    UPROPERTY ()
    TArray<ASplinePathActor*> PathList;

    TWeakObjectPtr<ASplinePathActor> SelectedPathPointOwner;
    int32 SelectedPathPointIndex;
    int32 SelectedPathPointControl;
    bool IsShowAllPath;
public:
	FSplinePathEditorEdMode ();
	virtual ~FSplinePathEditorEdMode ();

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	//virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool HandleClick (FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual void ActorSelectionChangeNotify() override;
	bool UsesToolkits() const override;
	// End of FEdMode interface

	// FEdMode Public Method for Toolkit
	void AddPath ();
	void RemovePath ();

	void AddPoint (const FVector& Position) const;
	void RemovePoint (int32 Index);

	void MakeSplineCurve() const;

    void ToggleShowAll(bool IsOn);
    // End of Public Method for Toolkit

private:
    void DrawPath(ASplinePathActor* Actor, FPrimitiveDrawInterface* PDI) const;
    void DrawPathCurve(ASplinePathActor* PathActor, int StartIndex, int EndIndex, FPrimitiveDrawInterface* PDI) const;
    void DrawPosition(ASplinePathActor* PathActor, int Index, FPrimitiveDrawInterface* PDI) const;
    void DrawControlPoint(ASplinePathActor* PathActor, int Index, bool IsIn, bool IsSelected, FPrimitiveDrawInterface* PDI) const;
};
