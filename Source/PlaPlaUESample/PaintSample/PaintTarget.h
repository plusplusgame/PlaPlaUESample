// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PaintTarget.generated.h"

UCLASS()
class PLAPLAUESAMPLE_API APaintTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	APaintTarget();

	void PaintToPoint(UMaterialInstanceDynamic* BrushMaterial
		, const FHitResult& HitResult
	);

protected:
	virtual void BeginPlay() override;

private:
	// ヒット位置からUV座標を計算
	FVector2f CalcUV(const FHitResult& HitResult) const;

protected:

	UPROPERTY(EditAnywhere, Category = "C++ Ref")
	UMaterialInterface* StoneMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "C++ Ref")
	UMaterialInterface* RenderTargetMaterialOriginal = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "C++ BP")
	UMaterialInstanceDynamic* PaintMaterialInstance = nullptr;

	UPROPERTY(EditAnywhere, Category = "C++ Param")
	float PaintInterval = 0.5f;

private:
	UPROPERTY()
	UTextureRenderTarget2D* PaintRenderTarget = nullptr;
	UPROPERTY()
	UTextureRenderTarget2D* PaintRenderTargetMask = nullptr;
	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent = nullptr;
};
