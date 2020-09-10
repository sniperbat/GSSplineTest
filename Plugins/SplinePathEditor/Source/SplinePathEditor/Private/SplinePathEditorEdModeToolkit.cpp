// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinePathEditorEdModeToolkit.h"
#include "SplinePathEditorEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "FSplinePathEdtiorEdModeToolkit"

FSplinePathEditorEdModeToolkit::FSplinePathEditorEdModeToolkit ()
{
}

void FSplinePathEditorEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
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
					.OnClicked(this, &FSplinePathEditorEdModeToolkit::OnBtnAddPath)
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

FName FSplinePathEditorEdModeToolkit::GetToolkitFName() const
{
	return FName("SplinePathEdtiorEdMode");
}

FText FSplinePathEditorEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("SplinePathEdtiorEdModeToolkit", "DisplayName", "SplinePathEdtiorEdMode Tool");
}

class FEdMode* FSplinePathEditorEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId);
}

FReply FSplinePathEditorEdModeToolkit::OnBtnAddPath () const
{
	static_cast<FSplinePathEditorEdMode*>(GetEditorMode())->AddPath ();
	return FReply::Handled ();
}


#undef LOCTEXT_NAMESPACE
