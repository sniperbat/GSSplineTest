// Copyright Epic Games, Inc. All Rights Reserved.

#include "PathSpine.h"
#include "PathSpineEdMode.h"

#define LOCTEXT_NAMESPACE "FPathSpineModule"

void FPathSpineModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FPathSpineEdMode>(FPathSpineEdMode::EM_PathSpineEdModeId, LOCTEXT("PathSpineEdModeName", "PathSpineEdMode"), FSlateIcon(), true);
}

void FPathSpineModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FPathSpineEdMode::EM_PathSpineEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPathSpineModule, PathSpine)