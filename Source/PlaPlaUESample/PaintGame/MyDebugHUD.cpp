// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDebugHUD.h"

AMyDebugHUD::AMyDebugHUD()
{
	DrawParams.SetNum((int)EIndex::Count);
}

void AMyDebugHUD::UpdateParam(EIndex Index, const FMyDebugHUDDrawParam& Param)
{
	DrawParams[(int)Index] = Param;
}

void AMyDebugHUD::UpdateTexture(EIndex Index, const UTexture* Texture)
{
	DrawParams[(int)Index].Texture = Texture;
}

void AMyDebugHUD::UpdateTintColor(EIndex Index, FColor Color)
{
	DrawParams[(int)Index].TintColor = Color;
}