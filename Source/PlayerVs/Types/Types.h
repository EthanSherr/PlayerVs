// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types.generated.h"


namespace GameConfigKeys
{
	const FString ServerNameKey = TEXT("ServerName");
	const FString GameKey = TEXT("Game");
	const FString MapKey = TEXT("Map");

	const FString MinimumPlayersKey = TEXT("MinimumPlayers");
	const FString MaximumPlayersKey = TEXT("MaximumPlayers");
	const FString RoundTimeKey = TEXT("RoundTime");
	const FString TimeBetweenMatchesKey = TEXT("TimeBetweenMatches");
	const FString TimeBeforeMatchKey = TEXT("TimeBeforeMatch");
}

USTRUCT(BlueprintType)
struct FGameConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	FString Game;

	UPROPERTY(BlueprintReadOnly)
	FString Map;

	UPROPERTY(BlueprintReadOnly)
	int32 RoundTime;

	UPROPERTY(BlueprintReadOnly)
	int32 MinimumPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 MaximumPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 TimeBetweenMatches;

	UPROPERTY(BlueprintReadOnly)
	int32 TimeBeforeMatch;
};

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Unassigned		UMETA(DisplayName = "Unassigned"),
	Alien			UMETA(DisplayName = "Alien"),
	Innocent		UMETA(DisplayName = "Innocent")
};

USTRUCT()
struct FGrabScanResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FName BoneName;

	UPROPERTY()
	FVector ImpactPoint;

	UPROPERTY()
	UObject* ObjectToGrip;

	UPROPERTY()
	FTransform ObjectTransform;

	UPROPERTY()
	AActor* Actor;

	UPROPERTY()
	UPrimitiveComponent* Component;

	FGrabScanResult()
	{
		BoneName = FName("None");
		ImpactPoint = FVector::ZeroVector;
		ObjectToGrip = NULL;
		Actor = NULL;
		Component = NULL;
	}
};

USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	/** The amount of damage actually applied */
	UPROPERTY()
		float ActualDamage;

	/** The damage type we were hit with. */
	UPROPERTY()
		UClass* DamageTypeClass;

	/** Who hit us */
	UPROPERTY()
		TWeakObjectPtr<class AABCharacter> PawnInstigator;

	/** Who actually caused the damage */
	UPROPERTY()
		TWeakObjectPtr<class AActor> DamageCauser;

	/** Specifies which DamageEvent below describes the damage received. */
	UPROPERTY()
		int32 DamageEventClassID;

	/** Rather this was a kill */
	UPROPERTY()
		uint32 bKilled : 1;

private:

	/** A rolling counter used to ensure the struct is dirty and will replicate. */
	UPROPERTY()
		uint8 EnsureReplicationByte;

	/** Describes general damage. */
	UPROPERTY()
		FDamageEvent GeneralDamageEvent;

	/** Describes point damage, if that is what was received. */
	UPROPERTY()
		FPointDamageEvent PointDamageEvent;

	/** Describes radial damage, if that is what was received. */
	UPROPERTY()
		FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo();

	FDamageEvent& GetDamageEvent();
	void SetDamageEvent(const FDamageEvent& DamageEvent);
	void EnsureReplication();
};


/* Keep in sync with ImpactEffect */
UENUM()
namespace EShooterPhysMaterialType
{
	enum Type
	{
		Unknown,
		Concrete,
		Dirt,
		Water,
		Metal,
		Wood,
		Grass,
		Glass,
		Flesh,
	};
}

#define SHOOTER_SURFACE_Default		SurfaceType_Default
#define SHOOTER_SURFACE_Concrete	SurfaceType1
#define SHOOTER_SURFACE_Dirt		SurfaceType2
#define SHOOTER_SURFACE_Water		SurfaceType3
#define SHOOTER_SURFACE_Metal		SurfaceType4
#define SHOOTER_SURFACE_Wood		SurfaceType5
#define SHOOTER_SURFACE_Grass		SurfaceType6
#define SHOOTER_SURFACE_Glass		SurfaceType7
#define SHOOTER_SURFACE_Flesh		SurfaceType8