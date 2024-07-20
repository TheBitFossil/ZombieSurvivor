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

	UFUNCTION()
	int32 GetScore() const {return Score;}
	
	UFUNCTION()
	void AddScore(int Value);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess="true"))
	FScoreChangedDelegate OnScoreChangedDelegate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	int Score{0};

	UFUNCTION()
	void SetScore(int Value);
	
};
