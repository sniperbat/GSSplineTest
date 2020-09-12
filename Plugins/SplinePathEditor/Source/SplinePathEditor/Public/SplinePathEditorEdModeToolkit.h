// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class FSplinePathEditorEdModeToolkit : public FModeToolkit
{
public:

	FSplinePathEditorEdModeToolkit();

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
	FReply OnBtnRemovePath () const;
	FReply OnBtnAddPoint () const;
	// FReply OnBtnRemovePoint (int32 Index);
    FReply OnBtnMakeSplineCurve () const;

	void OnToggleShowAll(ECheckBoxState NewState) const;
	void OnToggleUniteCtrlPoint (const ECheckBoxState NewState) const;
	void OnToggleCanAddPoint (ECheckBoxState NewState)const;

};
