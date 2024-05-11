// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintSampleDebugHUD.h"

APaintSampleDebugHUD::APaintSampleDebugHUD()
{
	DrawParams.SetNum((int)EIndex::Count);
}

void APaintSampleDebugHUD::UpdateParam(EIndex Index, const FPaintSampleDebugHUDDrawParam& Param)
{
	DrawParams[(int)Index] = Param;
}

void APaintSampleDebugHUD::UpdateTexture(EIndex Index, const UTexture* Texture)
{
	DrawParams[(int)Index].Texture = Texture;
}

void APaintSampleDebugHUD::UpdateTintColor(EIndex Index, FColor Color)
{
	DrawParams[(int)Index].TintColor = Color;
}