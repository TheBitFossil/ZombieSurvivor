// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeTopDown.h"

AGameModeTopDown::AGameModeTopDown()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameModeTopDown::BeginPlay()
{
	Super::BeginPlay();
}

void AGameModeTopDown::SetScore(const int Value)
{
	Score = FMath::Abs(Value);
}

void AGameModeTopDown::AddScore(const int Value)
{
	int NewScore = Score + Value;
	SetScore(NewScore);

	OnScoreChangedDelegate.Broadcast(Score);
}

