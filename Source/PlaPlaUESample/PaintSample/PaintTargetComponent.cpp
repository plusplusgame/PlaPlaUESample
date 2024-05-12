// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintTargetComponent.h"
#include "PaintSampleDebugHUD.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"

UPaintTargetComponent::UPaintTargetComponent()
{
}

void UPaintTargetComponent::BeginPlay()
{
	Super::BeginPlay();
	
	StaticMeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (!StaticMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("StaticMeshComponent is not found"));
		return;
	}

	// �����e�N�X�`����`�悵�Ă���
	auto* OriginalMaterial = StaticMeshComponent->GetMaterial(0);
	if (!OriginalMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("Material is not found"));
		return;
	}

	// �h�邽�߂̃����_�[�^�[�Q�b�g�쐬
	{
		// �e�N�X�`�����擾
		UTexture* Texture;
		OriginalMaterial->GetTextureParameterValue(FName("BaseColor"), Texture);
		if (!Texture)
		{
			UE_LOG(LogTemp, Error, TEXT("Texture is not found"));
			return;
		}
		CopyTextureToRenderTarget(Cast<UTexture2D>(Texture), OriginalMaterial, PaintRenderTarget);
	}

	// �}�e���A���C���X�^���X���쐬���āA�e�N�X�`����ݒ�
	PaintMaterialInstance = UMaterialInstanceDynamic::Create(RenderTargetMaterialOriginal, this);
	PaintMaterialInstance->SetTextureParameterValue(FName("RenderTargetTexture"), PaintRenderTarget);

	// ���b�V���̃}�e���A����u������
	StaticMeshComponent->SetMaterial(0, PaintMaterialInstance);
}


void UPaintTargetComponent::CopyTextureToRenderTarget(UTexture2D* SourceTexture, UMaterialInterface* CopyMaterial, UTextureRenderTarget2D*& OutRenderTarget)
{
	if (!SourceTexture)
	{
		UE_LOG(LogTemp, Warning, TEXT("SourceTexture is null"));
		return;
	}

	// Render Target �̍쐬
	OutRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, SourceTexture->GetSizeX(), SourceTexture->GetSizeY(), RTF_RGBA8);
	if (!OutRenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create RenderTarget"));
		return;
	}

	// Render Target �փe�N�X�`�����R�s�[
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, OutRenderTarget, nullptr);
	if (CopyMaterial)
	{
		FTextureRenderTargetResource* RenderTargetResource = OutRenderTarget->GameThread_GetRenderTargetResource();
		UWorld* const World = GetWorld();
		const auto FeatureLevel = World->Scene->GetFeatureLevel();

		FCanvas Canvas(
			OutRenderTarget->GameThread_GetRenderTargetResource(),
			nullptr,
			World,
			FeatureLevel
		);
		FCanvasTileItem TileItem(FVector2D(0, 0)
			, SourceTexture->GetResource()
			, FVector2D(OutRenderTarget->SizeX, OutRenderTarget->SizeY)
			, FLinearColor::White);
		TileItem.MaterialRenderProxy = CopyMaterial->GetRenderProxy();
		Canvas.DrawItem(TileItem);
		Canvas.Flush_GameThread();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CopyMaterial is not loaded"));
	}
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

		// �u���V��Material�Ƀp�����[�^��ݒ�
		BrushMaterial->SetVectorParameterValue(FName("PaintUV"), FVector(OutUV.X, OutUV.Y, 0));
		BrushMaterial->SetTextureParameterValue(FName("PaintTargetTexture"), PaintRenderTarget);

		UKismetRenderingLibrary::DrawMaterialToRenderTarget(World, PaintRenderTarget, BrushMaterial);
	}

	// �f�o�b�O�`��
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

	// �ʂ��\�����钸�_�̃C���f�b�N�X���擾
	const int32 Index0 = Indices[TriangleIndex * 3];
	const int32 Index1 = Indices[TriangleIndex * 3 + 1];
	const int32 Index2 = Indices[TriangleIndex * 3 + 2];

	// ���_�� UV ���W���擾
	const FVector2f UV0 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index0, 0);
	const FVector2f UV1 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index1, 0);
	const FVector2f UV2 = LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(Index2, 0);

	const FVector VertexPos0(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index0));
	const FVector VertexPos1(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index1));
	const FVector VertexPos2(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Index2));

	// �o���Z���g���b�N���W���v�Z���� UV ����
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