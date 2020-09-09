// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

struct HSplinePositionProxy : HHitProxy
{
	DECLARE_HIT_PROXY ();
	HSplinePositionProxy(): HHitProxy(HPP_Wireframe)
	{
	}
};

struct HSplineControlPointProxy : HHitProxy
{
	DECLARE_HIT_PROXY ();
	HSplineControlPointProxy () : HHitProxy (HPP_Wireframe)
	{
	}
};


class FSplinePathEditorEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_SplinePathEditorEdModeId;

	UPROPERTY ()
	class UMaterialInstanceDynamic* PositionMaterial;

	UPROPERTY ()
	class UMaterialInstanceDynamic* SelectedPositionMaterial;

	UPROPERTY ()
	class UMaterialInstanceDynamic* ControlPointMaterial;

	UPROPERTY ()
	class UMaterialInstanceDynamic* SelectedControlPointMaterial;
	
public:
	FSplinePathEditorEdMode ();
	virtual ~FSplinePathEditorEdMode ();

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	//virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	//virtual void ActorSelectionChangeNotify() override;
	bool UsesToolkits() const override;
	// End of FEdMode interface


public:
	void AddPath ();
	void RemovePath ();

	void AddPoint (FVector Position);
	void RemovePoint (int32 Index);
};
