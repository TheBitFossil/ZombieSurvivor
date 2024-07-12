// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class ABullet;
class ATopDownCharacter;

UCLASS()
class ZOMBIESURVIVOR_API AGun : public AActor
{
	GENERATED_BODY()

public:
	AGun();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlipBookComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FlipBooks)
	TArray<TObjectPtr<UPaperFlipbook>> FB_Walk;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Weapon)
	TArray<TObjectPtr<USceneComponent>> GunMuzzles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	TSubclassOf<ABullet> BulletClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsEquipped {};

	UFUNCTION()
	void RotateTowardsMouse(const FVector2D& MouseLocation);
	
	UFUNCTION()
	void SetAnimation(const bool bIsMoving, const EDirectionFacing& Direction) const;

	UFUNCTION()
	void Shoot(const EDirectionFacing& Facing);
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
