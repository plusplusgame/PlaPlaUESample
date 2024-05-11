// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h" 
#include "MyDebugHUDDrawParam.generated.h"

USTRUCT(BlueprintType)
struct SCAR_API FMyDebugHUDDrawParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	const UTexture* Texture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	float ScreenX = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	float ScreenY = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	float ScreenW = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	float ScreenH = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	FColor TintColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	TEnumAsByte<EBlendMode> BlendMode = EBlendMode::BLEND_Opaque;
};