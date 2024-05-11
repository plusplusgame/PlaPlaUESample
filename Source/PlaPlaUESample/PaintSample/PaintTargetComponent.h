// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PaintTargetComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAPLAUESAMPLE_API UPaintTargetComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:

	// @todo 消す
	UPROPERTY(EditAnywhere, Category = "C++ Ref")
	UMaterialInterface* RenderTargetMaterialOriginal = nullptr;

public:	
	UPaintTargetComponent();

	void PaintToPoint(UMaterialInstanceDynamic* BrushMaterial
		, const FHitResult& HitResult
	);

protected:
	virtual void BeginPlay() override;

private:
	// ヒット位置からUV座標を計算
	FVector2f CalcUV(const FHitResult& HitResult) const;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* PaintMaterialInstance = nullptr;
	UPROPERTY()
	UTextureRenderTarget2D* PaintRenderTarget = nullptr;
	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent = nullptr;
};
