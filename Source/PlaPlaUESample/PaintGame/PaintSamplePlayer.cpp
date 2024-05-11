#include "PaintSamplePlayer.h"
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

APaintSamplePlayer::APaintSamplePlayer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APaintSamplePlayer::BeginPlay()
{
    Super::BeginPlay();

    Camera = FindObject<UCameraComponent>(this, TEXT("FirstPersonCamera"));

	PaintMaterialInstance = UMaterialInstanceDynamic::Create(PaintMaterial, this);
	MaskMaterialInstance = UMaterialInstanceDynamic::Create(MaskMaterial, this);

    BrushColorIter.Reset();
    BrushTextureIter.Reset();
    PaintMaterialInstance->SetTextureParameterValue(FName("BrushTexture"), *BrushTextureIter);
    MaskMaterialInstance->SetTextureParameterValue(FName("BrushTexture"), *BrushTextureIter);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
        {
            {
                FPaintSampleDebugHUDDrawParam DrawParam;
                DrawParam.ScreenH = 100;
                DrawParam.ScreenW = 100;
                DrawParam.ScreenX = 620;
                DrawParam.ScreenY = 10;
                DrawParam.Texture = *BrushTextureIter;
                DrawParam.BlendMode = EBlendMode::BLEND_Translucent;
                HUD->UpdateParam(APaintSampleDebugHUD::EIndex::Brush, DrawParam);
                HUD->UpdateTintColor(APaintSampleDebugHUD::EIndex::Brush, *BrushColorIter);
            }
        }
    }
}

void APaintSamplePlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TraceForward();

    if (TraceHitResult.IsValidBlockingHit())
    {
		DrawDebugPoint(GetWorld(), TraceHitResult.Location, 10, FColor::Red, false);
    }
}

void APaintSamplePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APaintSamplePlayer::ExecPaint()
{
    if (TraceHitResult.bBlockingHit)
    {
		auto PaintLoc = TraceHitResult.Location;
        if (APaintTarget* const PaintTarget = Cast<APaintTarget>(TraceHitResult.GetActor()))
        {
            // PaintMaterialInstance に色を設定
			PaintMaterialInstance->SetVectorParameterValue(FName("BrushColor"), *BrushColorIter);
            PaintTarget->DoPaint(PaintMaterialInstance, MaskMaterialInstance, TraceHitResult);
        }
    }
}

void APaintSamplePlayer::FinishPaint()
{
	if (TraceHitResult.bBlockingHit)
	{
		if (APaintTarget* const PaintTarget = Cast<APaintTarget>(TraceHitResult.GetActor()))
		{
			PaintTarget->FinishPaint();
		}
	}
}

void APaintSamplePlayer::ChangeColor()
{
    NextIter(BrushColorIter);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
        {
            HUD->UpdateTintColor(APaintSampleDebugHUD::EIndex::Brush, *BrushColorIter);
        }
    }
}

void APaintSamplePlayer::ChangeBrushTexture()
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

bool APaintSamplePlayer::TraceForward()
{
	// プレイヤーのカメラの位置から前方にレイを飛ばす
    if (!Camera)
    {
		UE_LOG(LogTemp, Error, TEXT("FirstPersonCamera is not found."));
        TraceHitResult = FHitResult();
        return false;
    }

    FVector Start = Camera->GetComponentLocation();
    FVector ForwardVector = Camera->GetForwardVector();
    FVector End = Start + ForwardVector * 1000;

    FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(GetUVFromWorldLocation), true);
    CollisionParams.AddIgnoredActor(this);

    GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility, CollisionParams);

    return true;
}
