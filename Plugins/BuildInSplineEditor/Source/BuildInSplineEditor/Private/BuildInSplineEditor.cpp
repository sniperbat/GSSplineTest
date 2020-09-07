// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildInSplineEditor.h"
#include "BuildInSplineEditorEdMode.h"

#define LOCTEXT_NAMESPACE "FBuildInSplineEditorModule"

void FBuildInSplineEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FBuildInSplineEditorEdMode>(FBuildInSplineEditorEdMode::EM_BuildInSplineEditorEdModeId, LOCTEXT("BuildInSplineEditorEdModeName", "BuildInSplineEditorEdMode"), FSlateIcon(), true);
}

void FBuildInSplineEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FBuildInSplineEditorEdMode::EM_BuildInSplineEditorEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBuildInSplineEditorModule, BuildInSplineEditor)