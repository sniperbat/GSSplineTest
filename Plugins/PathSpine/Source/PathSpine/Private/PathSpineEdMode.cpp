// Copyright Epic Games, Inc. All Rights Reserved.

#include "PathSpineEdMode.h"
#include "PathSpineEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FPathSpineEdMode::EM_PathSpineEdModeId = TEXT("EM_PathSpineEdMode");

FPathSpineEdMode::FPathSpineEdMode()
{

}

FPathSpineEdMode::~FPathSpineEdMode()
{

}

void FPathSpineEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FPathSpineEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FPathSpineEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FPathSpineEdMode::UsesToolkits() const
{
	return true;
}




