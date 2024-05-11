#include "PainterComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "PaintSampleDebugHUD.h"
#include "PaintTarget.h"


template <typename T>
void NextIter(T& Iter)
{
	Iter++;
	if (!Iter)
	{
		Iter.Reset();
	}
}

UPainterComponent::UPainterComponent()
{
}

void UPainterComponent::BeginPlay()
{
    Camera = FindObject<UCameraComponent>(this, TEXT("FirstPersonCamera"));

	PaintMaterialInstance = UMaterialInstanceDynamic::Create(PaintMaterial, this);

    BrushColorIter.Reset();
    BrushTextureIter.Reset();
    PaintMaterialInstance->SetTextureParameterValue(FName("BrushTexture"), *BrushTextureIter);

    // デバッグ描画の初期化
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
		{
			FPaintSampleDebugHUDDrawParam DrawParam;
			DrawParam.ScreenH = 100;
			DrawParam.ScreenW = 100;
			DrawParam.ScreenX = 10;
			DrawParam.ScreenY = 10;
			DrawParam.Texture = *BrushTextureIter;
			DrawParam.BlendMode = EBlendMode::BLEND_Translucent;
			HUD->UpdateParam(APaintSampleDebugHUD::EIndex::Brush, DrawParam);
			HUD->UpdateTintColor(APaintSampleDebugHUD::EIndex::Brush, *BrushColorIter);
        }
    }
}

void UPainterComponent::TryPaint(const FHitResult& TraceHitResult)
{
    if (TraceHitResult.bBlockingHit)
    {
		auto PaintLoc = TraceHitResult.Location;
        if (APaintTarget* const PaintTarget = Cast<APaintTarget>(TraceHitResult.GetActor()))
        {
            PaintTarget->PaintToPoint(PaintMaterialInstance, TraceHitResult);
        }
    }
}

void UPainterComponent::ChangeColor()
{
    NextIter(BrushColorIter);
    PaintMaterialInstance->SetVectorParameterValue(FName("BrushColor"), *BrushColorIter);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
        {
            HUD->UpdateTintColor(APaintSampleDebugHUD::EIndex::Brush, *BrushColorIter);
        }
    }
}

void UPainterComponent::ChangeBrushTexture()
{
	NextIter(BrushTextureIter);
    // マテリアルのテクスチャを差し替え
	PaintMaterialInstance->SetTextureParameterValue(FName("BrushTexture"), *BrushTextureIter);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
        {
            HUD->UpdateTexture(APaintSampleDebugHUD::EIndex::Brush, *BrushTextureIter);
        }
    }
}