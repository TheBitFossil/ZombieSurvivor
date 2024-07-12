// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class ZOMBIESURVIVOR_API AGun : public AActor
{
	GENERATED_BODY()

public:
	AGun();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlipBookComponent;

	// FlipBook Animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Walk;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsEquipped {};
	
	// Change animations on player state
	UFUNCTION()
	void SetAnimation(const bool bIsMoving, const EDirectionFacing& Direction) const;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void RotateTowardsMouse(const FVector2D& MouseLocation);

};
