// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplinePathEdtior.h"
#include "SplinePathEdtiorEdMode.h"

#define LOCTEXT_NAMESPACE "FSplinePathEdtiorModule"

void FSplinePathEdtiorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FSplinePathEditorEdMode>(FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId, LOCTEXT("SplinePathEdtiorEdModeName", "SplinePathEdtiorEdMode"), FSlateIcon(), true);
}

void FSplinePathEdtiorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FSplinePathEditorEdMode::EM_SplinePathEditorEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSplinePathEdtiorModule, SplinePathEdtior)