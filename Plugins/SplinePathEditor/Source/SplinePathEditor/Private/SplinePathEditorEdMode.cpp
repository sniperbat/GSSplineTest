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
const FVector PositionRadii (10);
const FVector ControlPointRadii (8);

//---------------------------------------------------------------------------------------------------
// 点击测试用HitProxy
IMPLEMENT_HIT_PROXY (HSplinePositionProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineControlPointProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineSegmentProxy, HHitProxy);

//---------------------------------------------------------------------------------------------------
const FEditorModeID FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId = TEXT("EM_SplinePathEditorEdMode");

//---------------------------------------------------------------------------------------------------
FSplinePathEditorEdMode::FSplinePathEditorEdMode()
{

}

//---------------------------------------------------------------------------------------------------
FSplinePathEditorEdMode::~FSplinePathEditorEdMode()
{

}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FSplinePathEditorEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
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
    SelectedPath = nullptr;
    SelectedPathPointIndex = -1;
    SelectedControlPointInOut = -1;
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Exit()
{
	//TODO: 清理工作

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::UsesToolkits() const
{
	return true;
}


//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ActorSelectionChangeNotify() {

}
//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Render (const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
    if (IsShowAllPath){
        UWorld* World = GetWorld();
        for (TActorIterator<ASplinePathActor> It(World); It; ++It) {
            DrawPath(*It, PDI);
        }
    } else {
        for(FSelectionIterator It(GEditor->GetSelectedActorIterator()); It; ++It) {
            ASplinePathActor* Actor = Cast<ASplinePathActor>(*It);
            if (Actor){
                DrawPath(Actor, PDI);
            }
        }
    }

	FEdMode::Render (View, Viewport, PDI);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawPath(ASplinePathActor* Actor, FPrimitiveDrawInterface* PDI) const
{
	const int PointCount = Actor->PathPoints.Num();
    for(int i = 0; i < PointCount; i++) {
        const FPathPoint &PathPoint = Actor->PathPoints[i];
        const bool IsPointSelected = SelectedPath == Actor && SelectedPathPointIndex == i;
        DrawPosition(PathPoint.Position, i, IsPointSelected, PDI);
        if (PointCount > 1) {
            if (i != 0) {
                DrawControlPoint(PathPoint, i, false, SelectedControlPointInOut == 0, PDI);
            }
            if (i != PointCount - 1) {
                DrawControlPoint(PathPoint, i, true, SelectedControlPointInOut == 0, PDI);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawPosition(const FVector& Pos, int Index, bool IsSelected, FPrimitiveDrawInterface* PDI) const
{
    FMaterialRenderProxy* SphereMaterialProxy = IsSelected ?
            SelectedPositionMaterial->GetRenderProxy() :
            PositionMaterial->GetRenderProxy();
    PDI->SetHitProxy (new HSplinePositionProxy (Index));
    DrawSphere (PDI, Pos, FRotator::ZeroRotator, PositionRadii, 10, 10, SphereMaterialProxy, SDPG_Foreground);
    PDI->SetHitProxy (nullptr);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawControlPoint(const FPathPoint& Point, int Index, bool IsIn, bool IsSelected, FPrimitiveDrawInterface* PDI) const
{
    FMaterialRenderProxy * SphereMaterialProxy = IsSelected ?
            SelectedControlPointMaterial->GetRenderProxy() :
            ControlPointMaterial->GetRenderProxy();
    const FVector& CtrlPos = IsIn ? Point.InCtrlPoint : Point.OutCtrlPoint;
    PDI->SetHitProxy (new HSplineControlPointProxy (Index, IsIn));
    DrawSphere (PDI, CtrlPos, FRotator::ZeroRotator, ControlPointRadii, 10, 10, SphereMaterialProxy, SDPG_Foreground);
    PDI->SetHitProxy (nullptr);
    PDI->DrawLine(Point.Position, CtrlPos, FLinearColor::Gray, SDPG_Foreground);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::AddPath ()
{
    UWorld* World = GEditor->GetEditorWorldContext().World();
	SelectedPath = World->SpawnActor<ASplinePathActor>(FVector::ZeroVector, FRotator::ZeroRotator);
    PathList.Add(MakeShareable(SelectedPath.Get()));
    GEditor->SelectActor(SelectedPath.Get(), true, true);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePath ()
{
	if (SelectedPath != nullptr) {
	    SelectedPath->Destroy();
        SelectedPath = nullptr;
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::AddPoint (const FVector& Position)
{
	if (SelectedPath != nullptr){

	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePoint (int32 Index)
{
    if (SelectedPath != nullptr){

    }
}

//---------------------------------------------------------------------------------------------------


