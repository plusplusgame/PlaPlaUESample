#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "PainterComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAPLAUESAMPLE_API UPainterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPainterComponent();

protected:
	virtual void BeginPlay() override;

public:	

	// ヒット相手にペイントを実行
	UFUNCTION(BlueprintCallable)
	void TryPaint(const FHitResult& TraceHitResult);

	// ブラシの色変更
	UFUNCTION(BlueprintCallable)
	void ChangeColor();

	// ブラシのテクスチャ変更
	UFUNCTION(BlueprintCallable)
	void ChangeBrushTexture();

private:

	// ペイントする色のリスト
	UPROPERTY(EditAnywhere)
	TArray<FColor> BrushColors =
	{
		FColor(255,80,80),
		FColor(60,255,60),
		FColor(60,60,255),
		FColor(255,255,60),
		FColor(60,255,255),
		FColor(255,60,255),
		FColor::White,
	};

	// マテリアル
	UPROPERTY(EditAnywhere)
	UMaterial* PaintMaterial; 

	// ペイントするテクスチャのリスト
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> BrushTextures = { nullptr };

private:
	UPROPERTY()
	UMaterialInstanceDynamic* PaintMaterialInstance;
	UPROPERTY()
	UCameraComponent* Camera;

	TArray<FColor>::TIterator BrushColorIter = TArray<FColor>::TIterator(BrushColors);
	TArray<UTexture2D*>::TIterator BrushTextureIter = TArray<UTexture2D*>::TIterator(BrushTextures);

};
