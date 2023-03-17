// Fill out your copyright notice in the Description page of Project Settings.

#include "EditorPlayer.h"
#include "EditorUI.h"

void UEditorUI::SynchronizeProperties()
{
	Super::SynchronizeProperties();

}

void UEditorUI::SwapUIState(EToolType NewState)
{
	switch (NewState) {
	case Select: //Explore
		ToolState->SetActiveWidgetIndex(0);
		break;

	case Item:
		ToolState->SetActiveWidgetIndex(1);
		break;

	default:
	
		break;
	}
}
