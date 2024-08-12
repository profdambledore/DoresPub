// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UI_Tools_BuildWidget.h"

void UUI_Tools_BuildWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_Tools_BuildWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UUI_Tools_BuildWidget::UpdateTextVisibility(bool bVisible)
{
	if (bVisible) {
		SizeText->SetVisibility(ESlateVisibility::Visible);
		CostText->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		SizeText->SetVisibility(ESlateVisibility::Hidden);
		CostText->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UUI_Tools_BuildWidget::UpdateDisplayedText(int X, int Y, int Price)
{
	SizeText->SetText(FText::FromString(FString::Printf(TEXT("%i x %i"), X, Y)));
	CostText->SetText(FText::AsCurrencyBase(Price, "£"));
}
