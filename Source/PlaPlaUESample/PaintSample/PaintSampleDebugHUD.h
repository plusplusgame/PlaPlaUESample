// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PaintSampleDebugHUDDrawParam.h"
#include "PaintSampleDebugHUD.generated.h"

// デバッグ用HUD。テクスチャを任意の数座標を指定して描画できる
UCLASS()
class PLAPLAUESAMPLE_API APaintSampleDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	enum class EIndex
	{
		RenderTarget, // ペイントした対象のテクスチャ
		Brush,	// ペイントブラシのテクスチャ	
		
		Count,
	};

public:
	APaintSampleDebugHUD();
	void UpdateParam(EIndex Index, const FPaintSampleDebugHUDDrawParam& Param);
	void UpdateTexture(EIndex Index, const UTexture* Texture);
	void UpdateTintColor(EIndex Index, FColor Color);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	TArray<FPaintSampleDebugHUDDrawParam> DrawParams;

};
