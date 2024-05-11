// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintTarget.h"
#include "PaintSampleDebugHUD.h"
#include "kismet/KismetRenderingLibrary.h"


// Sets default values
APaintTarget::APaintTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APaintTarget::BeginPlay()
{
	Super::BeginPlay();
	
	// テクスチャ作成
	PaintRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor::White);
	PaintRenderTargetMask = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor::Transparent);

	// 初期テクスチャを描画
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PaintRenderTarget, StoneMaterial);


	// マテリアルを作成して、テクスチャを設定
	PaintMaterialInstance = UMaterialInstanceDynamic::Create(RenderTargetMaterialOriginal, this);
	PaintMaterialInstance->SetTextureParameterValue(FName("TextureSampleParam"), PaintRenderTarget);

	// メッシュのマテリアルを置き換え
	StaticMeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetMaterial(0, PaintMaterialInstance);

		UE_LOG(LogTemp, Warning, TEXT("Texture Replace Completed"));
	}

	// デバッグ描画
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
		{
			{
				FPaintSampleDebugHUDDrawParam DrawParam;
				DrawParam.ScreenH = 300;
				DrawParam.ScreenW = 300;
				DrawParam.ScreenX = 10;
				DrawParam.ScreenY = 10;
				DrawParam.Texture = PaintRenderTarget;
				HUD->UpdateParam(APaintSampleDebugHUD::EIndex::RenderTarget, DrawParam);
			}
			{
				FPaintSampleDebugHUDDrawParam DrawParam;
				DrawParam.ScreenH = 300;
				DrawParam.ScreenW = 300;
				DrawParam.ScreenX = 320;
				DrawParam.ScreenY = 10;
				DrawParam.Texture = PaintRenderTargetMask;
				DrawParam.BlendMode = EBlendMode::BLEND_Translucent;
				HUD->UpdateParam(APaintSampleDebugHUD::EIndex::RenderTargetMask, DrawParam);
			}
		}
	}
}

// Called every frame
void APaintTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APaintTarget::DoPaint(UMaterialInstanceDynamic* BrushMaterial, UMaterialInstanceDynamic* MaskMaterial, const FHitResult& HitResult)
{
	if (!HitResult.IsValidBlockingHit())
	{
		return;
	}
	if (!BrushMaterial)
	{
		return;
	}

	// 初回
	if (!LatestPaintedPosition.IsSet())
	{
		// HUDの中身を差し替え
		UpdateHUD();
		LatestPaintedPosition = HitResult.ImpactPoint;
		PaintToPoint(BrushMaterial, MaskMaterial, HitResult, HitResult.ImpactPoint);
		return;
	}
	else
	{
		const float MovedDistance = FVector::Dist(LatestPaintedPosition.GetValue(), HitResult.ImpactPoint);

		if (MovedDistance > PaintInterval)
		{
			// 一定距離移動したら、PaintIntervalごとにペイント
			const int32 PaintCount = FMath::FloorToInt(MovedDistance / PaintInterval);
			const FVector PaintDir = (HitResult.ImpactPoint - LatestPaintedPosition.GetValue()).GetSafeNormal();
			for (int32 i = 0; i < PaintCount; ++i)
			{
				const FVector PaintPos = LatestPaintedPosition.GetValue() + PaintDir * PaintInterval * i;
				PaintToPoint(BrushMaterial, MaskMaterial, HitResult, PaintPos);
			}
			LatestPaintedPosition = HitResult.ImpactPoint;
		}
	}
}

void APaintTarget::FinishPaint()
{	 
	LatestPaintedPosition.Reset();

}

void APaintTarget::PaintToPoint(UMaterialInstanceDynamic* BrushMaterial, UMaterialInstanceDynamic* MaskMaterial, const FHitResult& HitResult, const FVector PaintPos)
{
	auto* World = GetWorld();
	check(World);
	check(BrushMaterial);
	check(PaintRenderTarget);

	const FVector Offset = HitResult.Normal * PaintInterval * 2;
	const FVector Start = PaintPos + Offset;
	const FVector End = PaintPos - Offset;
	FHitResult MyHitResult;
	if (StaticMeshComponent->LineTraceComponent(MyHitResult, Start, End, FCollisionQueryParams(SCENE_QUERY_STAT(GetUVFromWorldLocation), true)))
	{
		const auto OutUV = CalcUV(MyHitResult);
		check(OutUV.ContainsNaN() == false);
		UE_LOG(LogTemp, Log, TEXT("Painting %.2f %.2f"), OutUV.X, OutUV.Y);

		// ブラシのMaterialにパラメータを設定
		BrushMaterial->SetVectorParameterValue(FName("PaintUV"), FVector(OutUV.X, OutUV.Y, 0));
		BrushMaterial->SetTextureParameterValue(FName("PaintTargetTexture"), PaintRenderTarget);

		// マスクのMaterialにパラメータを設定
		MaskMaterial->SetVectorParameterValue(FName("PaintUV"), FVector(OutUV.X, OutUV.Y, 0));
		MaskMaterial->SetTextureParameterValue(FName("PaintTargetTexture"), PaintRenderTargetMask);

		UKismetRenderingLibrary::DrawMaterialToRenderTarget(World, PaintRenderTarget, BrushMaterial);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(World, PaintRenderTargetMask, MaskMaterial);

		// MyHitResultをデバッグ描画
		//DrawDebugPoint(World, MyHitResult.ImpactPoint, 10, FColor::Red, false);
	}
}

FVector2f APaintTarget::CalcUV(const FHitResult& HitResult) const
{
	if (HitResult.FaceIndex < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Painting No Face"));
		return FVector2f(0, 0);
	}

	const int32 TriangleIndex = HitResult.FaceIndex;
	const FStaticMeshLODResources& LOD = StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[0];
	const FIndexArrayView Indices = LOD.IndexBuffer.GetArrayView();

	// 面を構成する頂点のインデックスを取得
	const int32 Index0 = Indices[TriangleIndex * 3];
	const int32 Index1 = Indices[TriangleIndex * 3 + 1];
	const int32 Index2 = Indices[TriangleIndex * 3 + 2];

	// 頂点の UV 座標を取得
	const FVector2f UV0 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index0, 0);
	const FVector2f UV1 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index1, 0);
	const FVector2f UV2 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index2, 0);

	const FVector VertexPos0(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index0));
	const FVector VertexPos1(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index1));
	const FVector VertexPos2(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index2));

	// バリセントリック座標を計算して UV を補間
	const FVector Barycentric = FMath::ComputeBaryCentric2D(HitResult.ImpactPoint
		, StaticMeshComponent->GetComponentTransform().TransformPosition(VertexPos0)
		, StaticMeshComponent->GetComponentTransform().TransformPosition(VertexPos1)
		, StaticMeshComponent->GetComponentTransform().TransformPosition(VertexPos2)
	);
	FVector2f OutUV = Barycentric.X * UV0
		+ Barycentric.Y * UV1
		+ Barycentric.Z * UV2;

	return OutUV;
}

void APaintTarget::UpdateHUD()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
		{
			HUD->UpdateTexture(APaintSampleDebugHUD::EIndex::RenderTarget, PaintRenderTarget);
		}
	}
}