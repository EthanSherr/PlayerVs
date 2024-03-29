// Fill out your copyright notice in the Description page of Project Settings.

#include "ABTTTGameMode.h"
#include "Online/ABTTTGameState.h"
#include "Player/ABTTTPlayerState.h"
#include "Types/Types.h"
#include "Kismet/GameplayStatics.h"

AABTTTGameMode::AABTTTGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GameStateClass = AABTTTGameState::StaticClass();
	PlayerStateClass = AABTTTPlayerState::StaticClass();
}

void AABTTTGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	AssignTeams(FMath::Max(1, NumPlayers / 3));
}

void AABTTTGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		auto PS = Cast<AABTTTPlayerState>((*It)->PlayerState);
		PS->RevealAll();		
	}
}

void AABTTTGameMode::Killed(AController* KillerController, AController* KilledController, APawn* KilledPawn, const UDamageType* DamageType)
{
	Super::Killed(KillerController, KilledController, KilledPawn, DamageType);
	auto KilledState = Cast<AABTTTPlayerState>(KilledController->PlayerState);
	auto KillerState = Cast<AABTTTPlayerState>(KillerController->PlayerState);
	if (KilledState && KillerState)
	{
		KilledState->SetIsAlive(false);
		if (KilledState->GetTeam() == ETeam::Alien)
		{
			KillerState->IncrementTraitorsKilled();
		}
		else
		{
			KillerState->IncrementInnocentsKilled();
		}
	}
}

void AABTTTGameMode::AssignTeams(int AlienCount)
{
	check(AlienCount <= NumPlayers);

	TArray<AABTTTPlayerState*> Traitors;
	TArray<APlayerState*> PlayerStates = GameState->PlayerArray;

	while (AlienCount > Traitors.Num())
	{
		int AlienIndex = FMath::RandRange(0, PlayerStates.Num() - 1);
		auto PS = Cast<AABTTTPlayerState>(PlayerStates[AlienIndex]);
		PlayerStates.RemoveAt(AlienIndex);
		Traitors.Add(PS);
	}

	for (AABTTTPlayerState* Traitor : Traitors)
	{
		Traitor->SetTeam(ETeam::Alien);
		Traitor->Client_SetTeam(ETeam::Alien);
		Traitor->Client_SetTraitors(Traitors);
	}

	for (APlayerState* NonTraitor : PlayerStates)
	{
		auto PS = Cast<AABTTTPlayerState>(NonTraitor);
		PS->SetTeam(ETeam::Innocent);
		PS->Client_SetTeam(ETeam::Innocent);
	}
}

bool AABTTTGameMode::GameCanStartCountdown()
{
	return true && Super::GameCanStartCountdown();
}

void AABTTTGameMode::DetermineMatchWinner()
{
	int NumInnocent = 0;

	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		auto PS = Cast<AABTTTPlayerState>((*It)->PlayerState);
		if (PS && PS->GetIsAlive() && PS->GetTeam() != ETeam::Alien)
		{
			NumInnocent++;
		}
	}

	bInnocentsWin = NumInnocent > 0;
}

bool AABTTTGameMode::IsWinner(AABPlayerState* PlayerState) const
{
	auto PS = Cast<AABTTTPlayerState>(PlayerState);
	return PS && (
		(PS->GetTeam() == ETeam::Innocent && bInnocentsWin) ||
		(PS->GetTeam() != ETeam::Innocent && !bInnocentsWin)
	);
}