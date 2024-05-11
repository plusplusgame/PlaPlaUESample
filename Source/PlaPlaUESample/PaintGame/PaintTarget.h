// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PaintTarget.generated.h"

UCLASS()
class SCAR_API APaintTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaintTarget();

	void DoPaint(UMaterialInstanceDynamic* PaintMaterial, UMaterialInstanceDynamic* MaskMaterial, const FHitResult& HitResult);
	void FinishPaint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void PaintToPoint(UMaterialInstanceDynamic* BrushMaterial, UMaterialInstanceDynamic* MaskMaterial, const FHitResult& HitResult, const FVector PaintPos);
	// ヒット位置からUV座標を計算
	FVector2f CalcUV(const FHitResult& HitResult) const;

	bool IsBeingPainted() const { return LatestPaintedPosition.IsSet(); }
	void UpdateHUD();

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
	TOptional<FVector> LatestPaintedPosition;
};
