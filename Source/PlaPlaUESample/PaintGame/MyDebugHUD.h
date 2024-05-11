// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyDebugHUDDrawParam.h"
#include "MyDebugHUD.generated.h"

UCLASS()
class SCAR_API AMyDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	enum class EIndex
	{
		RenderTarget,
		RenderTargetMask,
		Brush,
		
		Count,
	};

public:
	AMyDebugHUD();
	void UpdateParam(EIndex Index, const FMyDebugHUDDrawParam& Param);
	void UpdateTexture(EIndex Index, const UTexture* Texture);
	void UpdateTintColor(EIndex Index, FColor Color);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyDebugHUD")
	TArray<FMyDebugHUDDrawParam> DrawParams;

};
