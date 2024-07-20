// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeTopDown.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChangedDelegate, int, Score);

UCLASS()
class ZOMBIESURVIVOR_API AGameModeTopDown : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGameModeTopDown();

protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeBeforeLevelRestart{1.f};
	
	UFUNCTION()
	int32 GetScore() const {return Score;}
	
	UFUNCTION()
	void AddScore(int Value);

	UFUNCTION()
	void RestartGame();

	UFUNCTION()
	void OnRestartGameTimerTimeout();

private:
	FTimerHandle RestartTimerHandle;

	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess="true"))
	FScoreChangedDelegate OnScoreChangedDelegate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	int Score{0};

	UFUNCTION()
	void SetScore(int Value);
};
