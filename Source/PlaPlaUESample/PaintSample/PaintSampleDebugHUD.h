// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PaintSampleDebugHUDDrawParam.h"
#include "PaintSampleDebugHUD.generated.h"

// �f�o�b�O�pHUD�B�e�N�X�`����C�ӂ̐����W���w�肵�ĕ`��ł���
UCLASS()
class PLAPLAUESAMPLE_API APaintSampleDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	enum class EIndex
	{
		RenderTarget, // �y�C���g�����Ώۂ̃e�N�X�`��
		Brush,	// �y�C���g�u���V�̃e�N�X�`��	
		
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
