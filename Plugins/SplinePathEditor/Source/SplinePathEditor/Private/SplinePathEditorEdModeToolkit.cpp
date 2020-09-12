// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinePathEditorEdModeToolkit.h"
#include "SplinePathEditorEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"

//----------------------------------------------------------------------------------------------------------------------
#define LOCTEXT_NAMESPACE "FSplinePathEdtiorEdModeToolkit"

//----------------------------------------------------------------------------------------------------------------------
FSplinePathEditorEdModeToolkit::FSplinePathEditorEdModeToolkit()
{
}

//----------------------------------------------------------------------------------------------------------------------
void FSplinePathEditorEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			return true; // GEditor->GetSelectedActors ()->Num () != 0;
		}
	};

	const float Factor = 256.0f;

	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		.IsEnabled_Static(&Locals::IsWidgetEnabled)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .HAlign(HAlign_Center)
		  .AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Center)
			  .AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Show All Path:"))
			]
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Center)
			  .AutoWidth()
			[
				SNew(SCheckBox)
				.OnCheckStateChanged(this, &FSplinePathEditorEdModeToolkit::OnToggleShowAll)
			]
		]
		+ SVerticalBox::Slot()
		  .HAlign(HAlign_Center)
		  .AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Center)
			  .AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Unite Control Point:"))
			]
			+ SHorizontalBox::Slot()
			  .HAlign(HAlign_Center)
			  .AutoWidth()
			[
				SNew(SCheckBox)
				.OnCheckStateChanged(this, &FSplinePathEditorEdModeToolkit::OnToggleUniteCtrlPoint)
			]
		]
		+ SVerticalBox::Slot()
		  .HAlign(HAlign_Center)
		  .AutoHeight()
		[
			SNew(SButton)
					.Text(FText::FromString("Add Path"))
					.OnClicked(this, &FSplinePathEditorEdModeToolkit::OnBtnAddPath)
		]
		+ SVerticalBox::Slot()
		  .HAlign(HAlign_Center)
		  .AutoHeight()
		[
			SNew(SButton)
					.Text(FText::FromString("Remove Path"))
				        .OnClicked(this, &FSplinePathEditorEdModeToolkit::OnBtnRemovePath)
		]
		+ SVerticalBox::Slot()
		  .HAlign(HAlign_Center)
		  .AutoHeight()
		[
			SNew(SButton)
					.Text(FText::FromString("Add Random Point"))
					.OnClicked(this, &FSplinePathEditorEdModeToolkit::OnBtnAddPoint)
		]
		+ SVerticalBox::Slot()
		  .HAlign(HAlign_Center)
		  .AutoHeight()
		[
			SNew(SButton)
                      .Text(FText::FromString("Make Spline"))
                      .OnClicked(this, &FSplinePathEditorEdModeToolkit::OnBtnMakeSplineCurve)
		]

	];

	FModeToolkit::Init(InitToolkitHost);
}

//----------------------------------------------------------------------------------------------------------------------
FName FSplinePathEditorEdModeToolkit::GetToolkitFName() const
{
	return FName("SplinePathEditorEdMode");
}

//----------------------------------------------------------------------------------------------------------------------
FText FSplinePathEditorEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("SplinePathEditorEdModeToolkit", "DisplayName", "SplinePathEditorEdMode Tool");
}

//----------------------------------------------------------------------------------------------------------------------
class FEdMode* FSplinePathEditorEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId);
}

//----------------------------------------------------------------------------------------------------------------------
FReply FSplinePathEditorEdModeToolkit::OnBtnAddPath() const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->AddPath();
	return FReply::Handled();
}

//----------------------------------------------------------------------------------------------------------------------
FReply FSplinePathEditorEdModeToolkit::OnBtnRemovePath() const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->RemovePath();
	return FReply::Handled();
}

//----------------------------------------------------------------------------------------------------------------------
FReply FSplinePathEditorEdModeToolkit::OnBtnAddPoint() const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->AddPoint(FMath::VRand() * 50);
	return FReply::Handled();
}

//----------------------------------------------------------------------------------------------------------------------
void FSplinePathEditorEdModeToolkit::OnToggleShowAll(const ECheckBoxState NewState) const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->ToggleShowAll(NewState == ECheckBoxState::Checked);
}

//----------------------------------------------------------------------------------------------------------------------
void FSplinePathEditorEdModeToolkit::OnToggleUniteCtrlPoint(const ECheckBoxState NewState) const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->ToggleUniteCtrlPoint(NewState == ECheckBoxState::Checked);
}

//----------------------------------------------------------------------------------------------------------------------
void FSplinePathEditorEdModeToolkit::OnToggleCanAddPoint(const ECheckBoxState NewState) const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->ToggleCanAddPoint(NewState == ECheckBoxState::Checked);
}

//----------------------------------------------------------------------------------------------------------------------
FReply FSplinePathEditorEdModeToolkit::OnBtnMakeSplineCurve() const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->MakeSplineCurve();
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE
