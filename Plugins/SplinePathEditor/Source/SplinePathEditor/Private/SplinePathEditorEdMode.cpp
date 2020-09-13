// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinePathEditorEdMode.h"
#include "SplinePathEditorEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "EngineUtils.h"
#include "Engine/Selection.h"
#include "InputCoreTypes.h"

//---------------------------------------------------------------------------------------------------
// 常数定义
const float PositionPointSize = 20;
const float ControlPointSize = 16;

//---------------------------------------------------------------------------------------------------
enum {
	NoSelectedPoint = -1,
	SelectedControlPointIn,
	SelectedControlPointOut
};

//---------------------------------------------------------------------------------------------------
//模块本地变量定义
TWeakObjectPtr<ASplinePathActor> SelectedPathPointOwner;
int32 SelectedPathPointIndex;
int32 SelectedPathPointControl;
bool IsShowAllPath;
bool CanAddPoint;
bool IsUniteCtrlPointMove;

//---------------------------------------------------------------------------------------------------
//模块本地函数定义，不通过H文件暴露到外部

//---------------------------------------------------------------------------------------------------
void ResetSelection()
{
	//尝试获取选中的第一个可操作Actor
	SelectedPathPointOwner = GEditor->GetSelectedActors ()->GetTop<ASplinePathActor> ();
	SelectedPathPointIndex = NoSelectedPoint;
	SelectedPathPointControl = NoSelectedPoint;
}

//---------------------------------------------------------------------------------------------------
void SelectPoint (HSplinePositionProxy* HitProxy, const int SelectedCtrlPoint)
{
	SelectedPathPointOwner = HitProxy->PathActor;
	SelectedPathPointIndex = HitProxy->Index;
	SelectedPathPointControl = SelectedCtrlPoint;
	if (SelectedPathPointOwner == nullptr || !SelectedPathPointOwner->IsValidPointIndex(SelectedPathPointIndex))
	{
		SelectedPathPointIndex = NoSelectedPoint;
		SelectedPathPointControl = NoSelectedPoint;
	}
}

//---------------------------------------------------------------------------------------------------
bool IsPointSelected(ASplinePathActor* PathActor, const int Index)
{
	return SelectedPathPointOwner == PathActor && SelectedPathPointIndex == Index;
}

//---------------------------------------------------------------------------------------------------
void DrawPosition (ASplinePathActor* PathActor, int Index, FPrimitiveDrawInterface* PDI)
{
	const UPathPoint* PathPoint = PathActor->PathPoints[Index];
	const FLinearColor PointColor = IsPointSelected (PathActor, Index) ? FLinearColor::Red : FLinearColor::Green;
	PDI->SetHitProxy (new HSplinePositionProxy (PathActor, Index));
	PDI->DrawPoint (PathPoint->Position, PointColor, PositionPointSize, SDPG_Foreground);
	PDI->SetHitProxy (nullptr);
}

//---------------------------------------------------------------------------------------------------
#define IS_IN_CTRL_SELECTED (SelectedPathPointControl == SelectedControlPointIn)
#define IS_OUT_CTRL_SELECTED (SelectedPathPointControl == SelectedControlPointOut)
//---------------------------------------------------------------------------------------------------
bool IsCtrlPointSelected (const bool IsIn)
{
	return IsIn && IS_IN_CTRL_SELECTED || !IsIn && IS_OUT_CTRL_SELECTED;
}
//---------------------------------------------------------------------------------------------------
void DrawControlPoint (ASplinePathActor* PathActor, const int Index, const bool IsIn, FPrimitiveDrawInterface* PDI)
{
	const bool IsCtrlSelected = IsPointSelected(PathActor, Index) && IsCtrlPointSelected (IsIn);
	const UPathPoint* PathPoint = PathActor->PathPoints[Index];
	const FVector& CtrlPos = IsIn ? PathPoint->InCtrlPoint : PathPoint->OutCtrlPoint;
	const FLinearColor PointColor = IsCtrlSelected ? FLinearColor::Red : FLinearColor::Yellow;
	PDI->SetHitProxy (new HSplineControlPointProxy (PathActor, Index, IsIn));
	PDI->DrawPoint (CtrlPos, PointColor, ControlPointSize, SDPG_Foreground);
	PDI->SetHitProxy (nullptr);
	//控制点到位置点的连线
	PDI->DrawLine (PathPoint->Position, CtrlPos, FLinearColor::Gray, SDPG_Foreground);
}

//---------------------------------------------------------------------------------------------------
void DrawPathCurve (ASplinePathActor* PathActor, const int Index, FPrimitiveDrawInterface* PDI)
{
	if (!PathActor->IsLoop && Index == PathActor->GetSplinePointCount() - 1)
	{
		return;
	}
	const int Divide = 50;
	const float Step = 1 / static_cast<float>(Divide);
	for(int i = 0; i < Divide; i++)
	{
		const float T1 = i * Step;
		const float T2 = i == Divide - 1 ? 1 : (i + 1) * Step;
		const FVector Start = PathActor->GetPositionOnSegment (Index, T1);
		const FVector End = PathActor->GetPositionOnSegment (Index, T2);
		PDI->DrawLine (Start, End, FLinearColor::White, SDPG_Foreground);
	}
}

//---------------------------------------------------------------------------------------------------
void DrawPath (ASplinePathActor* PathActor, FPrimitiveDrawInterface* PDI) {
	//首先描画所有正式包含入路径曲线的节点
	const int SplinePointCount = PathActor->GetSplinePointCount ();
	for (int i = 0; i < SplinePointCount; i++) 
	{
		//描画位置节点
		DrawPosition (PathActor, i, PDI);

		if (SplinePointCount > 1) //多于一个节点的情况下需要描画控制点。
		{
			if (i != 0 || PathActor->IsLoop)
			{ //第一个节点的In控制点不描画, 但是Loop模式需要描画
				DrawControlPoint (PathActor, i, true, PDI);
			}
			
			if (i != SplinePointCount - 1 || PathActor->IsLoop) 
			{//最后一个节点的Out控制点不描画, 但是Loop模式需要描画
				DrawControlPoint (PathActor, i, false, PDI);
			}

			DrawPathCurve (PathActor, i, PDI);
		}
	}

	//描画剩下还未包含在曲线中的备选节点
	const int GetAlternatePointCount = PathActor->GetAlternatePointCount ();
	for (int i = 0; i < GetAlternatePointCount; i++) 
	{
		DrawPosition (PathActor, SplinePointCount + i, PDI);
	}
}

//---------------------------------------------------------------------------------------------------
bool HasSelectedPoint ()
{
	return SelectedPathPointOwner != nullptr &&
		SelectedPathPointIndex >= 0 &&
		SelectedPathPointIndex < SelectedPathPointOwner->GetTotalPointCount ();
}

//---------------------------------------------------------------------------------------------------
// 点击测试用HitProxy
IMPLEMENT_HIT_PROXY (HSplinePositionProxy, HHitProxy);
IMPLEMENT_HIT_PROXY (HSplineControlPointProxy, HSplinePositionProxy);
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

	if (!Toolkit.IsValid () && UsesToolkits ()) 
	{
		Toolkit = MakeShareable (new FSplinePathEditorEdModeToolkit);
		Toolkit->Init (Owner->GetToolkitHost ());
	}

	ResetSelection ();
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
void FSplinePathEditorEdMode::ActorSelectionChangeNotify ()
{
	ResetSelection();
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::Render (const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	//一个Level内可以包含多条Spline路径
	if (IsShowAllPath) 
	{
		//打开全部显示标志，则不管是否选中，路径都需要显示，方便整体查看
		if (UWorld* World = GetWorld ()) 
		{
			//可能有个别情况下GetWorld()返回nullptr，增加一个判断
			for (TActorIterator<ASplinePathActor> It (World); It; ++It)	
			{
				DrawPath (*It, PDI);
			}
		}
	}
	else 
	{
		//只显示选中的路径，但也可能是多选
		for (FSelectionIterator It (GEditor->GetSelectedActorIterator ()); It; ++It) 
		{
			if (ASplinePathActor* Actor = Cast<ASplinePathActor> (*It)) 
			{
				DrawPath (Actor, PDI);
			}
		}
	}
	FEdMode::Render (View, Viewport, PDI);
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::InputKey (FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bool IsHandled = false;
	if (!IsHandled && Event == IE_Released)
	{
		if (Key == EKeys::Enter)
		{
			MakeSplineCurve ();
			IsHandled = true;
		}
		else if (Key == EKeys::L)
		{
			if (SelectedPathPointOwner != nullptr)
			{
				SelectedPathPointOwner->ToggleLoop ();
				IsHandled = true;
			}
		} else if (Key == EKeys::D)
		{
			if (HasSelectedPoint() && SelectedPathPointControl == NoSelectedPoint)
			{
				SelectedPathPointOwner->RemovePoint (SelectedPathPointIndex);
				ResetSelection ();
				IsHandled = true;
			}
		}
	}
	return IsHandled;
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::HandleClick (FEditorViewportClient* ViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	bool IsHandled = false;
	if (HitProxy) 
	{
		if (HitProxy->IsA (HSplineControlPointProxy::StaticGetType ()))
		{
			//选中控制节点
			IsHandled = true;
			HSplineControlPointProxy* ControlHitProxy = static_cast<HSplineControlPointProxy*>(HitProxy);
			SelectPoint (ControlHitProxy, ControlHitProxy->IsIn ? SelectedControlPointIn : SelectedControlPointOut);
		}
		else if (HitProxy->IsA (HSplinePositionProxy::StaticGetType ()))
		{
			//选中位置节点
			IsHandled = true;
			SelectPoint(static_cast<HSplinePositionProxy*>(HitProxy), NoSelectedPoint);
		}
		else if (ViewportClient->IsCtrlPressed() && Click.GetKey () == EKeys::LeftMouseButton)
		{
			const FVector Pos = FMath::RayPlaneIntersection (Click.GetOrigin(), Click.GetDirection(), FPlane (FVector::UpVector, 0));
			AddPoint (Pos);
			IsHandled = true;
		}
	}
	return IsHandled;
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::AddPath ()
{
	UWorld* World = GEditor->GetEditorWorldContext ().World ();
	ASplinePathActor* SelectedPath = World->SpawnActor<ASplinePathActor> (FVector::ZeroVector, FRotator::ZeroRotator);
	GEditor->SelectActor (SelectedPath, true, true);
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePath ()
{
	for (FSelectionIterator It (GEditor->GetSelectedActorIterator ()); It; ++It) 
	{
		if (ASplinePathActor* Actor = Cast<ASplinePathActor> (*It))
		{
			Actor->Destroy ();
		}
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::AddPoint (const FVector& Position) const
{
	if (SelectedPathPointOwner != nullptr) 
	{
		SelectedPathPointOwner->AddPoint (Position);
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::RemovePoint (int32 Index)
{
	
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::MakeSplineCurve () const
{
	if (SelectedPathPointOwner != nullptr) 
	{
		//连接备选点，生成曲线
		SelectedPathPointOwner->MakeSplineCurve ();
	}
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ToggleShowAll (const bool IsOn)
{
	IsShowAllPath = IsOn;
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ToggleCanAddPoint (const bool IsOn)
{
	CanAddPoint = IsOn;
}

//---------------------------------------------------------------------------------------------------
void FSplinePathEditorEdMode::ToggleUniteCtrlPoint (const bool IsOn)
{
	IsUniteCtrlPointMove = IsOn;
}
//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::ShowModeWidgets () const
{
	return true;
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::ShouldDrawWidget () const
{
	return true;
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::UsesTransformWidget () const
{
	return true;
}

//---------------------------------------------------------------------------------------------------
FVector FSplinePathEditorEdMode::GetWidgetLocation () const
{
	if (HasSelectedPoint()) 
	{
		const UPathPoint* Point = SelectedPathPointOwner->PathPoints[SelectedPathPointIndex];
		switch (SelectedPathPointControl)
		{
		case NoSelectedPoint:
			return Point->Position;
		case SelectedControlPointIn:
			return Point->InCtrlPoint;
		case SelectedControlPointOut:
			return Point->OutCtrlPoint;
		default: break;
		}
	}
	return FEdMode::GetWidgetLocation ();
}

//---------------------------------------------------------------------------------------------------
bool FSplinePathEditorEdMode::InputDelta (FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	if (InViewportClient->GetCurrentWidgetAxis () != EAxisList::None && !InDrag.IsZero ()) 
	{
		if (HasSelectedPoint())
		{
			UPathPoint* Point = SelectedPathPointOwner->PathPoints[SelectedPathPointIndex];
			switch (SelectedPathPointControl)
			{
			case NoSelectedPoint:
				Point->MovePosition (InDrag);
				SelectedPathPointOwner->RecalculateLength (SelectedPathPointIndex-1);
				SelectedPathPointOwner->RecalculateLength (SelectedPathPointIndex);
				return true;
			case SelectedControlPointIn:
				Point->MoveInCtrl (InDrag, IsUniteCtrlPointMove);
				SelectedPathPointOwner->RecalculateLength (SelectedPathPointIndex - 1);
				return true;
			case SelectedControlPointOut:
				Point->MoveOutCtrl (InDrag, IsUniteCtrlPointMove);
				SelectedPathPointOwner->RecalculateLength (SelectedPathPointIndex);
				return true;
			default: break;
			}
		}
	}
	return false;
}
