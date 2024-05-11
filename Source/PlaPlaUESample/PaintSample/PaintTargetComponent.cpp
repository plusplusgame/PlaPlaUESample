// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintTargetComponent.h"
#include "PaintSampleDebugHUD.h"
#include "kismet/KismetRenderingLibrary.h"

UPaintTargetComponent::UPaintTargetComponent()
{
}

void UPaintTargetComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 塗るためのレンダーターゲット作成
	PaintRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor::White);

	StaticMeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (!StaticMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticMeshComponent is not found"));
		return;
	}

	// 初期テクスチャを描画しておく
	auto* OriginalMaterial = StaticMeshComponent->GetMaterial(0);
	if (!OriginalMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("Material is not found"));
		return;
	}

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PaintRenderTarget, OriginalMaterial);

	// マテリアルインスタンスを作成して、テクスチャを設定
	PaintMaterialInstance = UMaterialInstanceDynamic::Create(RenderTargetMaterialOriginal, this);
	PaintMaterialInstance->SetTextureParameterValue(FName("RenderTargetTexture"), PaintRenderTarget);

	// メッシュのマテリアルを置き換え
	StaticMeshComponent->SetMaterial(0, PaintMaterialInstance);
}

void UPaintTargetComponent::PaintToPoint(UMaterialInstanceDynamic* BrushMaterial, const FHitResult& HitResult )
{
	const FVector& PaintPos = HitResult.ImpactPoint;
	
	auto* World = GetWorld();
	check(World);
	check(BrushMaterial);
	check(PaintRenderTarget);

	const FVector Offset = HitResult.Normal * 2;
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

		UKismetRenderingLibrary::DrawMaterialToRenderTarget(World, PaintRenderTarget, BrushMaterial);
	}

	// デバッグ描画
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APaintSampleDebugHUD* HUD = Cast<APaintSampleDebugHUD>(PC->GetHUD()))
		{
			FPaintSampleDebugHUDDrawParam DrawParam;
			DrawParam.ScreenH = 300;
			DrawParam.ScreenW = 300;
			DrawParam.ScreenX = 10;
			DrawParam.ScreenY = 10;
			DrawParam.Texture = PaintRenderTarget;
			HUD->UpdateParam(APaintSampleDebugHUD::EIndex::RenderTarget, DrawParam);
		}
	}
}

FVector2f UPaintTargetComponent::CalcUV(const FHitResult& HitResult) const
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