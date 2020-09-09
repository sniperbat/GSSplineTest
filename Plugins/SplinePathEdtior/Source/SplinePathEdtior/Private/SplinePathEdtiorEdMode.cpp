// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinePathEdtiorEdMode.h"
#include "SplinePathEdtiorEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "Materials/MaterialInstanceDynamic.h"

IMPLEMENT_HIT_PROXY (HSplinePositionProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineControlPointProxy, HHitProxy);

const FEditorModeID FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId = TEXT("EM_SplinePathEditorEdMode");

FSplinePathEditorEdMode::FSplinePathEditorEdMode ()
{

}

FSplinePathEditorEdMode::~FSplinePathEditorEdMode ()
{

}

void FSplinePathEditorEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FSplinePathEdtiorEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
	
	PositionMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);

	SelectedPositionMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);
	SelectedPositionMaterial->SetVectorParameterValue ("GizmoColor", FLinearColor::Green);

	ControlPointMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);
	ControlPointMaterial->SetVectorParameterValue ("GizmoColor", FLinearColor::Yellow);

	SelectedControlPointMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);
	SelectedControlPointMaterial->SetVectorParameterValue ("GizmoColor", FLinearColor::Green);
}

void FSplinePathEditorEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FSplinePathEditorEdMode::UsesToolkits() const
{
	return true;
}


const FVector PositionRadii (10);
const FVector ControlPointRadii (8);

void FSplinePathEditorEdMode::Render (const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	const FVector Center (0);

	PDI->SetHitProxy (new HSplinePositionProxy ());
	
	DrawSphere (PDI, Center, FRotator::ZeroRotator, PositionRadii, 10, 10, PositionMaterial->GetRenderProxy(), SDPG_Foreground);
	//DrawCoordinateSystem (PDI, Center, FRotator::ZeroRotator, 30, SDPG_Foreground);

	PDI->SetHitProxy (nullptr);

	FEdMode::Render (View, Viewport, PDI);
}

void FSplinePathEditorEdMode::AddPath ()
{
	
}
void FSplinePathEditorEdMode::RemovePath ()
{
	
}

void FSplinePathEditorEdMode::AddPoint (FVector Position)
{
	
}

void FSplinePathEditorEdMode::RemovePoint (int32 Index)
{
	
}
