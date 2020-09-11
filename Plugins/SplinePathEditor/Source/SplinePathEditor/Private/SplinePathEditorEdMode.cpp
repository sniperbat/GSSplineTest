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
const float ControlPointSize (16);

enum {
	SelectedControlPointIn,
	SelectedControlPointOut
};

//---------------------------------------------------------------------------------------------------
// 点击测试用HitProxy
IMPLEMENT_HIT_PROXY (HSplinePositionProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineControlPointProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineSegmentProxy, HHitProxy);

//---------------------------------------------------------------------------------------------------
const FEditorModeID FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId = TEXT ("EM_SplinePathEditorEdMode");

//---------------------------------------------------------------------------------------------------
FSplinePathEditorEdMode::FSplinePathEditorEdMode () {
}

//---------------------------------------------------------------------------------------------------
FSplinePathEditorEdMode::~FSplinePathEditorEdMode () {
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Enter () {
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
	//获取场景中所有的ASplinePathActor备用
	UWorld* World = GetWorld ();
	for (TActorIterator<ASplinePathActor> It (World); It; ++It) {
		PathList.Add (*It);
	}

	//切到当前GameMode时，自动获取选中的第一个可操作Actor
	SelectedPathPointOwner = GEditor->GetSelectedActors ()->GetTop<ASplinePathActor> ();
	SelectedPathPointIndex = -1;
	SelectedPathPointControl = -1;
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Exit ()
{
	//TODO: 清理工作

	if (Toolkit.IsValid ())	{
		FToolkitManager::Get ().CloseToolkit (Toolkit.ToSharedRef ());
		Toolkit.Reset ();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit ();
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::UsesToolkits () const {
	return true;
}


//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ActorSelectionChangeNotify () {
	//切换选中的Actor时，获取第一个选择的那个作为操作对象
	SelectedPathPointOwner = GEditor->GetSelectedActors ()->GetTop<ASplinePathActor> ();
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Render (const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) {
	//一个Level内可以包含多条Spline路径
	if (IsShowAllPath) {
		//打开全部显示标志，则不管是否选中，路径都需要显示，方便整体查看
		if (UWorld* World = GetWorld ()) {
			//可能有个别情况下GetWorld()返回nullptr，增加一个判断
			for (TActorIterator<ASplinePathActor> It (World); It; ++It)	{
				DrawPath (*It, PDI);
			}
		}
	} else {
		//只显示选中的路径，但也可能是多选
		for (FSelectionIterator It (GEditor->GetSelectedActorIterator ()); It; ++It) {
			if (ASplinePathActor* Actor = Cast<ASplinePathActor> (*It)) {
				DrawPath (Actor, PDI);
			}
		}
	}
	FEdMode::Render (View, Viewport, PDI);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawPath (ASplinePathActor* PathActor, FPrimitiveDrawInterface* PDI) const {
	//首先描画所有正式包含入路径曲线的节点
	const int SplinePointCount = PathActor->GetSplinePointCount ();
	for (int i = 0; i < SplinePointCount; i++) {
		//描画位置节点
		DrawPosition (PathActor, i, PDI);
		
		if (SplinePointCount > 1) {
			//多于一个节点的情况下需要描画控制点。
			if (i != 0) {
				//第一个节点的In控制点不描画
				DrawControlPoint (PathActor, i, true, SelectedPathPointControl == SelectedControlPointIn, PDI);
			}
			if (i != SplinePointCount - 1) {
				//最后一个节点的Out控制点不描画
				DrawControlPoint (PathActor, i, false, SelectedPathPointControl == SelectedControlPointOut, PDI);
				//非最后一个节点时，描画从当前节点到下一个节点的曲线
				DrawPathCurve (PathActor, i, i + 1, PDI);
			}
		}
	}
	//描画剩下还未包含在曲线中的备选节点
	const int GetAlternatePointCount = PathActor->GetAlternatePointCount ();
	for (int i = 0; i < GetAlternatePointCount; i++) {
		DrawPosition (PathActor, SplinePointCount + i, PDI);
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawPathCurve (ASplinePathActor* PathActor, int StartIndex, int EndIndex, FPrimitiveDrawInterface* PDI) const {
	const UPathPoint* StartPoint = PathActor->PathPoints[StartIndex];
	const UPathPoint* EndPoint = PathActor->PathPoints[EndIndex];
	//临时只画直线
	PDI->DrawLine (StartPoint->Position, EndPoint->Position, FLinearColor::White, SDPG_Foreground);
}


//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawPosition (ASplinePathActor* PathActor, int Index, FPrimitiveDrawInterface* PDI) const {
	// FMaterialRenderProxy* SphereMaterialProxy = IsSelected ?
	// 	SelectedPositionMaterial->GetRenderProxy () :
	// 	PositionMaterial->GetRenderProxy ();
	//DrawSphere (PDI, Pos, FRotator::ZeroRotator, PositionRadii, 10, 10, SphereMaterialProxy, SDPG_Foreground);
	
	const bool IsPointSelected = SelectedPathPointOwner == PathActor && SelectedPathPointIndex == Index;
	const UPathPoint* PathPoint = PathActor->PathPoints[Index];
	PDI->SetHitProxy (new HSplinePositionProxy (PathActor, Index));
	PDI->DrawPoint (PathPoint->Position, IsPointSelected ? FLinearColor::Red : FLinearColor::Green, PositionPointSize, SDPG_Foreground);
	PDI->SetHitProxy (nullptr);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::DrawControlPoint (ASplinePathActor* PathActor, int Index, bool IsIn, bool IsSelected, FPrimitiveDrawInterface* PDI) const {
	// FMaterialRenderProxy* SphereMaterialProxy = IsSelected ?
	// 	SelectedControlPointMaterial->GetRenderProxy () :
	// 	ControlPointMaterial->GetRenderProxy ();
	//DrawSphere (PDI, CtrlPos, FRotator::ZeroRotator, ControlPointRadii, 10, 10, SphereMaterialProxy, SDPG_Foreground);

	const UPathPoint* PathPoint = PathActor->PathPoints[Index];
	const FVector& CtrlPos = IsIn ? PathPoint->InCtrlPoint : PathPoint->OutCtrlPoint;
	PDI->SetHitProxy (new HSplineControlPointProxy (PathActor, Index, IsIn));
	PDI->DrawPoint (CtrlPos, IsSelected ? FLinearColor::Red : FLinearColor::Yellow, ControlPointSize, SDPG_Foreground);
	PDI->SetHitProxy (nullptr);
	//控制点到位置点的连线
	PDI->DrawLine (PathPoint->Position, CtrlPos, FLinearColor::Gray, SDPG_Foreground);
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::HandleClick (FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) {
	bool IsHandled = false;
	if (HitProxy) {
		if (HitProxy->IsA (HSplinePositionProxy::StaticGetType ())) {
			//选中位置节点
			IsHandled = true;
			HSplinePositionProxy* PositionHitProxy = static_cast<HSplinePositionProxy*>(HitProxy);
			SelectedPathPointOwner = PositionHitProxy->PathActor;
			SelectedPathPointIndex = PositionHitProxy->Index;
			SelectedPathPointControl = -1;
		} else if (HitProxy->IsA (HSplineControlPointProxy::StaticGetType ())) {
			//选中控制节点
			IsHandled = true;
			HSplineControlPointProxy* ControlHitProxy = static_cast<HSplineControlPointProxy*>(HitProxy);
			SelectedPathPointOwner = ControlHitProxy->PathActor;
			SelectedPathPointIndex = ControlHitProxy->Index;
			SelectedPathPointControl = ControlHitProxy->IsIn ? 0 : 1;
		}
	}
	return IsHandled;
}


//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::AddPath () {
	UWorld* World = GEditor->GetEditorWorldContext ().World ();
	ASplinePathActor* SelectedPath = World->SpawnActor<ASplinePathActor> (FVector::ZeroVector, FRotator::ZeroRotator);
	PathList.Add (SelectedPath);
	GEditor->SelectActor (SelectedPath, true, true);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePath () {
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
		SelectedPathPointOwner->AddPoint (Position);
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePoint (int32 Index) {
	
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::MakeSplineCurve () const {
	if (SelectedPathPointOwner != nullptr) {
		//连接备选点，生成曲线
		SelectedPathPointOwner->MakeSplineCurve ();
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ToggleShowAll (const bool IsOn) {
	IsShowAllPath = IsOn;
}

