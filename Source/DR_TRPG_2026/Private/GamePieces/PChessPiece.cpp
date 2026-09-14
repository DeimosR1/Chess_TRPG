// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePieces/PChessPiece.h"

#include "Components/WidgetComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GridMovement/ChessBoardManager.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/OverheadHealthStatus.h"

// Sets default values
APChessPiece::APChessPiece()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("UnitModel");
	RootComponent = StaticMeshComponent;
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComponent->OnClicked.AddDynamic(this, &APChessPiece::HandleClicked);
	
	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Overhead Widget Component");
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());
	
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnFloating"));
	
	DisableComponentsSimulatePhysics();

}

// Called when the game starts or when spawned
void APChessPiece::BeginPlay()
{
	Super::BeginPlay();
	ChessBoardManager = Cast<AChessBoardManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AChessBoardManager::StaticClass()));
	if (IsValid(ChessBoardManager))
	{
		// Snap wherever this piece was placed in the level to the logical board grid
		// and register it with the GridManager so it counts toward occupancy.
		const FIntPoint StartingGridPosition = ChessBoardManager->WorldToGrid(GetActorLocation());
		ChessBoardManager->RegisterPiece(this, StartingGridPosition);
	}
	
	ConfigureOverheadWidgetComponent();
}

// Called every frame
void APChessPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APChessPiece::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

TArray<FIntPoint> APChessPiece::GetValidMoves(const TMap<FIntPoint, APChessPiece*>& OccupiedSquares, int32 BoardWidth, int32 BoardHeight) const
{
	TArray<FIntPoint> ValidMoves;
 
	switch (PieceType)
	{
	case EChessPieceType::Pawn:
	{
		// Player pieces advance toward +Y, enemy pieces advance toward -Y.
		const int32 Direction = bIsPlayerTeam ? 1 : -1;
 
		const FIntPoint Forward(GridPosition.X, GridPosition.Y + Direction);
		if (Forward.Y >= 0 && Forward.Y < BoardHeight && !OccupiedSquares.Contains(Forward))
		{
			ValidMoves.Add(Forward);
		}
 
		for (int32 XOffset : { -1, 1 })
		{
			const FIntPoint DiagonalCapture(GridPosition.X + XOffset, GridPosition.Y + Direction);
			if (DiagonalCapture.X >= 0 && DiagonalCapture.X < BoardWidth && DiagonalCapture.Y >= 0 && DiagonalCapture.Y < BoardHeight)
			{
				if (APChessPiece* const* PieceInSpace = OccupiedSquares.Find(DiagonalCapture))
				{
					if (IsValid(*PieceInSpace) && (*PieceInSpace)->bIsPlayerTeam != bIsPlayerTeam)
					{
						ValidMoves.Add(DiagonalCapture);
					}
				}
			}
		}
		break;
	}
	case EChessPieceType::Knight:
	{
		const TArray<FIntPoint> Offsets = {
			{1, 2}, {2, 1}, {2, -1}, {1, -2},
			{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
		};
		AddStepMoves(ValidMoves, Offsets, OccupiedSquares, BoardWidth, BoardHeight);
		break;
	}
	case EChessPieceType::Bishop:
	{
		const TArray<FIntPoint> Directions = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
		AddSlidingMoves(ValidMoves, Directions, OccupiedSquares, BoardWidth, BoardHeight);
		break;
	}
	case EChessPieceType::Rook:
	{
		const TArray<FIntPoint> Directions = { {1,0}, {-1,0}, {0,1}, {0,-1} };
		AddSlidingMoves(ValidMoves, Directions, OccupiedSquares, BoardWidth, BoardHeight);
		break;
	}
	case EChessPieceType::Queen:
	{
		const TArray<FIntPoint> Directions = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };
		AddSlidingMoves(ValidMoves, Directions, OccupiedSquares, BoardWidth, BoardHeight);
		break;
	}
	case EChessPieceType::King:
	{
		const TArray<FIntPoint> Offsets = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };
		AddStepMoves(ValidMoves, Offsets, OccupiedSquares, BoardWidth, BoardHeight);
		break;
	}
	}
 
	return ValidMoves;
}

FVector APChessPiece::GetGridLocation(FVector Location)
{
	float newY = (Location.Y - (GridSize / 2))/GridSize;
	float newX = (Location.X - (GridSize / 2))/GridSize;
	float X2 = FMath::RoundToInt(newX);
	float Y2 = FMath::RoundToInt(newY);
	
	FVector NewLocation = FVector((X2 * GridSize)+(GridSize/2), (Y2 * GridSize)+(GridSize/2), 0.0f);
	
	return NewLocation;
}

void APChessPiece::HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	//Gate of not moving if it moved already.
	if (bHasMoved){ return; }
	
	if (IsValid(ChessBoardManager))
	{
		ChessBoardManager->SelectPiece(this);
	}
}

void APChessPiece::AddSlidingMoves(TArray<FIntPoint>& OutMoves, const TArray<FIntPoint>& Directions,
	const TMap<FIntPoint, APChessPiece*>& OccupiedSquares, int32 BoardWidth, int32 BoardHeight) const
{
	for (const FIntPoint& Direction : Directions)
	{
		FIntPoint Current = GridPosition + Direction;
		while (Current.X >= 0 && Current.X < BoardWidth && Current.Y >= 0 && Current.Y < BoardHeight)
		{
			if (APChessPiece* const* Occupant = OccupiedSquares.Find(Current))
			{
				if (IsValid(*Occupant) && (*Occupant)->bIsPlayerTeam != bIsPlayerTeam)
				{
					OutMoves.Add(Current); // Can capture here, but the slide stops.
				}
				break;
			}
			OutMoves.Add(Current);
			Current += Direction;
		}
	}
}

void APChessPiece::AddStepMoves(TArray<FIntPoint>& OutMoves, const TArray<FIntPoint>& Offsets,
	const TMap<FIntPoint, APChessPiece*>& OccupiedSquares, int32 BoardWidth, int32 BoardHeight) const
{
	for (const FIntPoint& Offset : Offsets)
	{
		const FIntPoint Target = GridPosition + Offset;
		if (Target.X < 0 || Target.X >= BoardWidth || Target.Y < 0 || Target.Y >= BoardHeight)
		{
			continue;
		}
 
		if (APChessPiece* const* Occupant = OccupiedSquares.Find(Target))
		{
			if (IsValid(*Occupant) && (*Occupant)->bIsPlayerTeam == bIsPlayerTeam)
			{
				//Occupied by Ally, can't continue.
				continue;
			}
		}
		OutMoves.Add(Target);
	}
}

void APChessPiece::ReceiveDamage(int32 Attack)
{
	int32 NewHealth = GetCurrentHealth() - (Attack/(GetDefense()));
	UnitStats.UnitCurrentHealth = FMath::Clamp(NewHealth, 0, GetMaxHealth());
	UOverheadHealthStatus* OverheadStatusGauge = Cast<UOverheadHealthStatus>(OverheadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatusGauge)
	{
		OverheadStatusGauge->SetValueOfHealthBar(GetCurrentHealth(), GetMaxHealth());
	}
}

void APChessPiece::ConfigureOverheadWidgetComponent()
{
	if (!OverheadWidgetComponent)
	{
		return;
	}
	
	OverheadWidgetComponent->InitWidget();
	
	UOverheadHealthStatus* OverheadStatusGauge = Cast<UOverheadHealthStatus>(OverheadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatusGauge)
	{
		OverheadStatusGauge->SetValueOfHealthBar(GetCurrentHealth(), GetMaxHealth());
	}
	OverheadWidgetComponent->SetHiddenInGame(false);
}

