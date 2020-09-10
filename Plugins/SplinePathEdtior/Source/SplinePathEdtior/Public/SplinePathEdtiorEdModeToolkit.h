// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class FSplinePathEdtiorEdModeToolkit : public FModeToolkit
{
public:

	FSplinePathEdtiorEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

private:

	TSharedPtr<SWidget> ToolkitWidget;

private:
	FReply OnBtnAddPath () const;
	// FReply OnBtnRemovePath ();
	// FReply OnBtnAddPoint (FVector Position);
	// FReply OnBtnRemovePoint (int32 Index);
};