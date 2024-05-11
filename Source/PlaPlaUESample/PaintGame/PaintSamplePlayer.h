#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "PaintSamplePlayer.generated.h"

UCLASS()
class PLAPLAUESAMPLE_API APaintSamplePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APaintSamplePlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void ExecPaint();

	UFUNCTION(BlueprintCallable)
	void FinishPaint();

	UFUNCTION(BlueprintCallable)
	void ChangeColor();

	UFUNCTION(BlueprintCallable)
	void ChangeBrushTexture();

	bool TraceForward();

private:

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

	UPROPERTY(EditAnywhere)
	UMaterial* PaintMaterial; // 材料を設定するためのプロパティ

	UPROPERTY(EditAnywhere)
	UMaterial* MaskMaterial;

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> BrushTextures = { nullptr };

private:
	UPROPERTY()
	UMaterialInstanceDynamic* PaintMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* MaskMaterialInstance;
	UPROPERTY()
	UCameraComponent* Camera;

	FHitResult TraceHitResult;
	bool IsPainting = false;
	TArray<FColor>::TIterator BrushColorIter = TArray<FColor>::TIterator(BrushColors);
	TArray<UTexture2D*>::TIterator BrushTextureIter = TArray<UTexture2D*>::TIterator(BrushTextures);

};
