#include "MyPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "MyDebugHUD.h"
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

AMyPlayer::AMyPlayer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayer::BeginPlay()
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
        if (AMyDebugHUD* HUD = Cast<AMyDebugHUD>(PC->GetHUD()))
        {
            {
                FMyDebugHUDDrawParam DrawParam;
                DrawParam.ScreenH = 100;
                DrawParam.ScreenW = 100;
                DrawParam.ScreenX = 620;
                DrawParam.ScreenY = 10;
                DrawParam.Texture = *BrushTextureIter;
                DrawParam.BlendMode = EBlendMode::BLEND_Translucent;
                HUD->UpdateParam(AMyDebugHUD::EIndex::Brush, DrawParam);
                HUD->UpdateTintColor(AMyDebugHUD::EIndex::Brush, *BrushColorIter);
            }
        }
    }
}

void AMyPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TraceForward();

    if (TraceHitResult.IsValidBlockingHit())
    {
		DrawDebugPoint(GetWorld(), TraceHitResult.Location, 10, FColor::Red, false);
    }
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMyPlayer::ExecPaint()
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

void AMyPlayer::FinishPaint()
{
	if (TraceHitResult.bBlockingHit)
	{
		if (APaintTarget* const PaintTarget = Cast<APaintTarget>(TraceHitResult.GetActor()))
		{
			PaintTarget->FinishPaint();
		}
	}
}

void AMyPlayer::ChangeColor()
{
    NextIter(BrushColorIter);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (AMyDebugHUD* HUD = Cast<AMyDebugHUD>(PC->GetHUD()))
        {
            HUD->UpdateTintColor(AMyDebugHUD::EIndex::Brush, *BrushColorIter);
        }
    }
}

void AMyPlayer::ChangeBrushTexture()
{
	NextIter(BrushTextureIter);
    // マテリアルのテクスチャを差し替え
	PaintMaterialInstance->SetTextureParameterValue(FName("BrushTexture"), *BrushTextureIter);

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (AMyDebugHUD* HUD = Cast<AMyDebugHUD>(PC->GetHUD()))
        {
            HUD->UpdateTexture(AMyDebugHUD::EIndex::Brush, *BrushTextureIter);
        }
    }
}

bool AMyPlayer::TraceForward()
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
