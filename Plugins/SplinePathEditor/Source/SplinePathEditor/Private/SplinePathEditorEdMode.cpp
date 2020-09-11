// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinePathEditorEdMode.h"
#include "SplinePathEditorEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "EngineUtils.h"
#include "Engine/Selection.h"

//---------------------------------------------------------------------------------------------------
// 常数定义
const FVector PositionRadii (6);
const FVector ControlPointRadii (4);
const float PositionPointSize = 20;
const float ControlPointSize(16);
//---------------------------------------------------------------------------------------------------
// 点击测试用HitProxy
IMPLEMENT_HIT_PROXY (HSplinePositionProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineControlPointProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineSegmentProxy, HHitProxy);

//---------------------------------------------------------------------------------------------------
const FEditorModeID FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId = TEXT ("EM_SplinePathEditorEdMode");

//---------------------------------------------------------------------------------------------------
FSplinePathEditorEdMode::FSplinePathEditorEdMode ()
{

}

//---------------------------------------------------------------------------------------------------
FSplinePathEditorEdMode::~FSplinePathEditorEdMode ()
{

}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Enter ()
{
	FEdMode::Enter ();

	if (!Toolkit.IsValid () && UsesToolkits ()) {
		Toolkit = MakeShareable (new FSplinePathEditorEdModeToolkit);
		Toolkit->Init (Owner->GetToolkitHost ());
	}
	//---------------------------------------------------------------------------------------------------
	//各位置及控制点的描画用材质
	PositionMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);

	SelectedPositionMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);
	SelectedPositionMaterial->SetVectorParameterValue ("GizmoColor", FLinearColor::Green);

	ControlPointMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);
	ControlPointMaterial->SetVectorParameterValue ("GizmoColor", FLinearColor::Yellow);

	SelectedControlPointMaterial = UMaterialInstanceDynamic::Create (GEngine->ArrowMaterial, nullptr);
	SelectedControlPointMaterial->SetVectorParameterValue ("GizmoColor", FLinearColor::Green);

	//---------------------------------------------------------------------------------------------------
	UWorld* World = GetWorld ();
	for (TActorIterator<ASplinePathActor> It (World); It; ++It) {
		PathList.Add (*It);
	}

    SelectedPathPointOwner = GEditor->GetSelectedActors ()->GetTop<ASplinePathActor> ();
	SelectedPathPointIndex = -1;
	SelectedPathPointControl = -1;
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Exit ()
{
	//TODO: 清理工作

	if (Toolkit.IsValid ())
	{
		FToolkitManager::Get ().CloseToolkit (Toolkit.ToSharedRef ());
		Toolkit.Reset ();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit ();
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::UsesToolkits () const
{
	return true;
}


//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ActorSelectionChangeNotify () {
	SelectedPathPointOwner = GEditor->GetSelectedActors ()->GetTop<ASplinePathActor> ();
}
//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Render (const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	if (IsShowAllPath) {
	    if (UWorld* World = GetWorld ())
	    {
            for (TActorIterator<ASplinePathActor> It(World); It; ++It)
            {
                DrawPath (*It, PDI);
            }
	    }
	}
	else {
		for (FSelectionIterator It (GEditor->GetSelectedActorIterator ()); It; ++It) {
			if (ASplinePathActor* Actor = Cast<ASplinePathActor> (*It)) {
				DrawPath (Actor, PDI);
			}
		}
	}

	FEdMode::Render (View, Viewport, PDI);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawPath (ASplinePathActor* PathActor, FPrimitiveDrawInterface* PDI) const
{
	const int SplinePointCount = PathActor->GetSplinePointCount();
	for (int i = 0; i < SplinePointCount; i++) {
		const UPathPoint* PathPoint = PathActor->PathPoints[i];
		const bool IsPointSelected = SelectedPathPointOwner == PathActor && SelectedPathPointIndex == i;
		DrawPosition (PathPoint->Position, i, IsPointSelected, PDI);

		if (SplinePointCount > 1) {
			if (i != 0) {
				DrawControlPoint (PathPoint, i, true, SelectedPathPointControl == 0, PDI);
			}
			if (i != SplinePointCount - 1) {
				DrawControlPoint (PathPoint, i, false, SelectedPathPointControl == 1, PDI);
                //Draw Spline Curve
                DrawPathCurve(PathActor, i, i+1, PDI);
			}
		}
	}

	const int GetAlternatePointCount = PathActor->GetAlternatePointCount();
    for (int i = 0; i < GetAlternatePointCount; i++) {
        const UPathPoint* PathPoint = PathActor->PathPoints[SplinePointCount + i];
        const bool IsPointSelected = SelectedPathPointOwner == PathActor && SelectedPathPointIndex == i;
        DrawPosition (PathPoint->Position, i, IsPointSelected, PDI);
    }
}

void FSplinePathEditorEdMode::DrawPathCurve (ASplinePathActor* PathActor, int StartIndex, int EndIndex, FPrimitiveDrawInterface* PDI) const
{
    const UPathPoint* StartPoint = PathActor->PathPoints[StartIndex];
    const UPathPoint* EndPoint = PathActor->PathPoints[EndIndex];
    //临时只画直线
    PDI->DrawLine (StartPoint->Position, EndPoint->Position, FLinearColor::White, SDPG_Foreground);
}


//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawPosition (const FVector& Pos, int Index, bool IsSelected, FPrimitiveDrawInterface* PDI) const
{
	FMaterialRenderProxy* SphereMaterialProxy = IsSelected ?
		SelectedPositionMaterial->GetRenderProxy () :
		PositionMaterial->GetRenderProxy ();
	PDI->SetHitProxy (new HSplinePositionProxy (Index));
	//DrawSphere (PDI, Pos, FRotator::ZeroRotator, PositionRadii, 10, 10, SphereMaterialProxy, SDPG_Foreground);
	PDI->DrawPoint(Pos, FLinearColor::Red, PositionPointSize, SDPG_Foreground);
	PDI->SetHitProxy (nullptr);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawControlPoint (const UPathPoint* Point, int Index, bool IsIn, bool IsSelected, FPrimitiveDrawInterface* PDI) const
{
	FMaterialRenderProxy* SphereMaterialProxy = IsSelected ?
		SelectedControlPointMaterial->GetRenderProxy () :
		ControlPointMaterial->GetRenderProxy ();

	const FVector& CtrlPos = IsIn ? Point->InCtrlPoint : Point->OutCtrlPoint;

	PDI->SetHitProxy (new HSplineControlPointProxy (Index, IsIn));
	//DrawSphere (PDI, CtrlPos, FRotator::ZeroRotator, ControlPointRadii, 10, 10, SphereMaterialProxy, SDPG_Foreground);
    PDI->DrawPoint(CtrlPos, FLinearColor::Yellow, ControlPointSize, SDPG_Foreground);
	PDI->SetHitProxy (nullptr);
	PDI->DrawLine (Point->Position, CtrlPos, FLinearColor::Gray, SDPG_Foreground);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::AddPath ()
{
	UWorld* World = GEditor->GetEditorWorldContext ().World ();
	ASplinePathActor* SelectedPath = World->SpawnActor<ASplinePathActor> (FVector::ZeroVector, FRotator::ZeroRotator);
	PathList.Add (SelectedPath);
	GEditor->SelectActor (SelectedPath, true, true);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePath ()
{
	for (FSelectionIterator It (GEditor->GetSelectedActorIterator ()); It; ++It) {
		ASplinePathActor* Actor = Cast<ASplinePathActor> (*It);
		if (Actor) {
			PathList.RemoveAll ([&](const ASplinePathActor* PathActor)
				{
					return PathActor == Actor;
				});
			Actor->Destroy ();
		}
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::AddPoint (const FVector& Position) const {
  if (SelectedPathPointOwner != nullptr) {
	  SelectedPathPointOwner->AddPoint(Position);
  }
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePoint (int32 Index)
{
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::MakeSplineCurve()
{
    if (SelectedPathPointOwner != nullptr) {
        SelectedPathPointOwner->MakeSplineCurve();
    }
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ToggleShowAll(bool IsOn)
{
    IsShowAllPath = IsOn;
}

