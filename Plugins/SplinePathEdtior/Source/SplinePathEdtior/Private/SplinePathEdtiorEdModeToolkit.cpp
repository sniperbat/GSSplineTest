// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinePathEdtiorEdModeToolkit.h"
#include "SplinePathEdtiorEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "FSplinePathEdtiorEdModeToolkit"

FSplinePathEdtiorEdModeToolkit::FSplinePathEdtiorEdModeToolkit()
{
}

void FSplinePathEdtiorEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			return true;// GEditor->GetSelectedActors ()->Num () != 0;
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
					SNew (SButton)
					.Text (FText::FromString("Add Path"))
					.OnClicked(this, &FSplinePathEdtiorEdModeToolkit::OnBtnAddPath)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew (SButton)
					.Text (FText::FromString("Remove Path"))
					//.OnClicked_Static (&Locals::OnBtnRemovePath)
				]
		
		];
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FSplinePathEdtiorEdModeToolkit::GetToolkitFName() const
{
	return FName("SplinePathEdtiorEdMode");
}

FText FSplinePathEdtiorEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("SplinePathEdtiorEdModeToolkit", "DisplayName", "SplinePathEdtiorEdMode Tool");
}

class FEdMode* FSplinePathEdtiorEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId);
}

FReply FSplinePathEdtiorEdModeToolkit::OnBtnAddPath () const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->AddPath ();
	return FReply::Handled ();
}


#undef LOCTEXT_NAMESPACE
