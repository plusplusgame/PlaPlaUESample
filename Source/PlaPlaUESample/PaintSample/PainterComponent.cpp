#include "PainterComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "PaintSampleDebugHUD.h"
#include "PaintTargetComponent.h"


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
	PaintMaterialInstance = UMaterialInstanceDynamic::Create(PaintMaterial, this);

    BrushColorIter.Reset();
}

void UPainterComponent::TryPaint(const FHitResult& TraceHitResult)
{
    if (TraceHitResult.bBlockingHit)
    {
		auto PaintLoc = TraceHitResult.Location;
        if (UPaintTargetComponent* const PaintTarget = Cast<UPaintTargetComponent>(TraceHitResult.GetActor()))
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