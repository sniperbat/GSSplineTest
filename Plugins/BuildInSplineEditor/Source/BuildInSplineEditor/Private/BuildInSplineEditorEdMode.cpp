// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildInSplineEditorEdMode.h"
#include "BuildInSplineEditorEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FBuildInSplineEditorEdMode::EM_BuildInSplineEditorEdModeId = TEXT("EM_BuildInSplineEditorEdMode");

FBuildInSplineEditorEdMode::FBuildInSplineEditorEdMode()
{

}

FBuildInSplineEditorEdMode::~FBuildInSplineEditorEdMode()
{

}

void FBuildInSplineEditorEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FBuildInSplineEditorEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FBuildInSplineEditorEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FBuildInSplineEditorEdMode::UsesToolkits() const
{
	return true;
}




