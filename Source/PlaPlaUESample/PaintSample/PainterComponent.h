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

	// �q�b�g����Ƀy�C���g�����s
	UFUNCTION(BlueprintCallable)
	void TryPaint(const FHitResult& TraceHitResult);

	// �u���V�̐F�ύX
	UFUNCTION(BlueprintCallable)
	void ChangeColor();

	// �u���V�̃e�N�X�`���ύX
	UFUNCTION(BlueprintCallable)
	void ChangeBrushTexture();

private:

	// �y�C���g����F�̃��X�g
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

	// �}�e���A��
	UPROPERTY(EditAnywhere)
	UMaterial* PaintMaterial; 

	// �y�C���g����e�N�X�`���̃��X�g
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
