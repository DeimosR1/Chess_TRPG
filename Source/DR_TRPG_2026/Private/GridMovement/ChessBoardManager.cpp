// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMovement/ChessBoardManager.h"

#include "BoardGridHighlight.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameMode/ChessGameMode.h"
#include "GamePieces/PChessPiece.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AChessBoardManager::AChessBoardManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	//Old Static Mesh, kept so it
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ChessBoardModel");
	RootComponent = StaticMeshComponent;
	
	ProceduralTileLight = CreateDefaultSubobject<UInstancedStaticMeshComponent>("LightTiles");
	ProceduralTileLight->SetupAttachment(RootComponent);
	ProceduralTileLight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProceduralTileLight->SetCanEverAffectNavigation(false);

	ProceduralTileDark = CreateDefaultSubobject<UInstancedStaticMeshComponent>("DarkTiles");
	ProceduralTileDark->SetupAttachment(RootComponent);
	ProceduralTileDark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProceduralTileDark->SetCanEverAffectNavigation(false);
}

// Called when the game starts or when spawned
void AChessBoardManager::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	GenerateBoard();
}

#if WITH_EDITOR
void AChessBoardManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GenerateBoard();
}
#endif

// Called every frame
void AChessBoardManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaTickSpeed = DeltaTime;
	TickInterpolatePieceMovement(DeltaTime);

}

FIntPoint AChessBoardManager::WorldToGrid(const FVector& WorldLocation) const
{
	const int32 X = FMath::RoundToInt((WorldLocation.X - (GridSize / 2)) / GridSize);
	const int32 Y = FMath::RoundToInt((WorldLocation.Y - (GridSize / 2)) / GridSize);
	return FIntPoint(X, Y);
}

FVector AChessBoardManager::GridToWorld(const FIntPoint& GridCoord) const
{
	return FVector((GridCoord.X * GridSize) + (GridSize / 2),(GridCoord.Y * GridSize) + (GridSize / 2),0.0f);
}

void AChessBoardManager::RegisterPiece(APChessPiece* Piece, const FIntPoint& GridCoord)
{
	//Check if the Piece actually exists, if not, not run.
	if (!Piece)
	{
		return;
	}
 
	Piece->SetGridPosition(GridCoord);
	Piece->SetActorLocation(GridToWorld(GridCoord));
	OccupiedSquares.Add(GridCoord, Piece);
}

void AChessBoardManager::SelectPiece(APChessPiece* Piece, AChessPlayerState* RequestingPlayerState)
{
	// Gate to check that the server is the only one doing stuff
	if (!HasAuthority())
	{
		return;
	}
	
	if (Piece && Piece == SelectedPiece && CurrentSelectingPlayer == RequestingPlayerState)
	{
		DeselectPiece(RequestingPlayerState);
		return;
	}
	
	ClearSelection();
 
	if (!Piece){return;}
	
	if (Piece->bHasMoved){return;}
	
	if (IsValid(GameMode) && Piece->bIsPlayerTeam != GameMode->bIsPlayerTurn) {return;}
	
	SelectedPiece = Piece;
	OnSelectedPieceChanged.Broadcast(SelectedPiece, CurrentSelectingPlayer);
	//Update the current selecting Player
	CurrentSelectingPlayer = RequestingPlayerState;
	
	const TArray<FIntPoint> ValidMoves = Piece->GetValidMoves(OccupiedSquares, BoardWidth, BoardHeight);
	SpawnHighlights(ValidMoves);
	
	if (ValidMoves.Num() == 0)
	{
		SelectedPiece->ChangeHasMoved(true);
	}
}

void AChessBoardManager::ClearSelection()
{
	SelectedPiece = nullptr;
	OnSelectedPieceChanged.Broadcast(nullptr, nullptr);
 
	for (ABoardGridHighlight* Tile : ActiveHighlights)
	{
		if (IsValid(Tile))
		{
			Tile->Destroy(true);
		}
	}
	ActiveHighlights.Empty();
}

bool AChessBoardManager::TryMoveSelectedPieceTo(const FIntPoint& TargetCoord, AChessPlayerState* RequestingPlayerState)
{
	// Gate to check that the server is the only one doing stuff
	if (!HasAuthority())
	{
		return false;
	}
	
	if (!SelectedPiece)
	{
		return false;
	}
	
	// To check if the Player that selected the piece is the one confirming the movement, this way the opponent
	// cannot interfere
	if (CurrentSelectingPlayer != RequestingPlayerState)
	{
		return false;
	}
	
	const TArray<FIntPoint> ValidMoves = SelectedPiece->GetValidMoves(OccupiedSquares, BoardWidth, BoardHeight);
	if (!ValidMoves.Contains(TargetCoord))
	{
		return false;
	}
 
	// If capturing, remove the occupant first.
	if (APChessPiece* Occupant = OccupiedSquares.FindRef(TargetCoord))
	{
		if (IsValid(Occupant))
		{
			Occupant->ReceiveDamage(SelectedPiece->GetDamage());
			if (Occupant->GetCurrentHealth() > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Enemy Still alive."))
				SelectedPiece->ChangeHasMoved(true);
				SelectedPiece->ChangeActiveColor(true);
 
				ClearSelection();
	
				if (GameMode)
				{
					GameMode->CheckIfTurnChange();
				}
				return false;
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Enemy Dead alive."))
			(Occupant)->Destroy();
		}
	}
 
	OccupiedSquares.Remove(SelectedPiece->GridPosition);
	OccupiedSquares.Add(TargetCoord, SelectedPiece);
 
	SelectedPiece->SetGridPosition(TargetCoord);
	
	//Do a Lerp or a timeline to smoothout movement
	
	StartSmoothMove(SelectedPiece, GridToWorld(TargetCoord));
	
	SelectedPiece->ChangeHasMoved(true);
	SelectedPiece->ChangeActiveColor(true);
 
	ClearSelection();
	
	if (GameMode)
	{
		GameMode->CheckIfTurnChange();
	}
	
	return true;
}

APChessPiece* AChessBoardManager::GetPieceAt(const FIntPoint& GridCoord) const
{
	if (APChessPiece* const* Occupant = OccupiedSquares.Find(GridCoord))
	{
		return *Occupant;
	}
	return nullptr;
}

TArray<APChessPiece*> AChessBoardManager::GetAllPiecesOnBoard()
{
	TArray<APChessPiece*> AllPiecesOnBoard;
	OccupiedSquares.GenerateValueArray(AllPiecesOnBoard);
	
	return AllPiecesOnBoard;
}

void AChessBoardManager::SpawnHighlights(const TArray<FIntPoint>& ValidMoves)
{
	if (!HighlightTileClass)
	{
		return;
	}
 
	for (const FIntPoint& Move : ValidMoves)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (ABoardGridHighlight* Tile = GetWorld()->
			SpawnActor<ABoardGridHighlight>(HighlightTileClass, GridToWorld(Move), FRotator::ZeroRotator, SpawnParams))
		{
			Tile->GridCoord = Move;
			Tile->OwningGridManager = this;
			ActiveHighlights.Add(Tile);
		}
	}
}

void AChessBoardManager::StartSmoothMove(APChessPiece* Piece, const FVector& TargetLocation)
{
	if (!IsValid(Piece))
	{
		return;
	}

	MovingPiece = Piece;
	MoveStartLocation = Piece->GetActorLocation();
	MoveTargetLocation = TargetLocation;
	MoveElapsedTime = 0.0f;
	bIsMovingPiece = true;
}

void AChessBoardManager::DeselectPiece(AChessPlayerState* RequestingPlayerState)
{
	if (!HasAuthority() || !SelectedPiece)
	{
		return;
	}

	// The opponent must not be able to cancel your selection
	if (CurrentSelectingPlayer != RequestingPlayerState)
	{
		return;
	}

	ClearSelection();
	CurrentSelectingPlayer = nullptr;
}

void AChessBoardManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChessBoardManager, SelectedPiece);
}

void AChessBoardManager::GenerateBoard()
{
	ClearBoard();

	if (!TileMesh || TileMeshSize <= 0.0f || BoardWidth <= 0 || BoardHeight <= 0)
	{
		return;
	}

	ProceduralTileLight->SetStaticMesh(TileMesh);
	ProceduralTileDark->SetStaticMesh(TileMesh);

	if (LightTileMaterial)
	{
		ProceduralTileLight->SetMaterial(0, LightTileMaterial);
	}
	if (DarkTileMaterial)
	{
		ProceduralTileDark->SetMaterial(0, DarkTileMaterial);
	}

	//This makes each tile be the size of the actual tile mesh so it doesn't matter.
	const float TileScale = GridSize / TileMeshSize;

	for (int32 X = 0; X < BoardWidth; ++X)
	{
		for (int32 Y = 0; Y < BoardHeight; ++Y)
		{
			// Uses the same function as the pieces going on the floor so they line up perfectly.
			const FVector TileLocation = GridToWorld(FIntPoint(X, Y));
			const FTransform TileTransform(FRotator::ZeroRotator, TileLocation, FVector(TileScale));

			// This is to create the alternation of each tile so it is like a checkboard.
			const bool bIsLightSquare = ((X + Y) % 2) == 0;
			UInstancedStaticMeshComponent* TargetTile = bIsLightSquare ? ProceduralTileLight : ProceduralTileDark;

			// Create the tile.
			TargetTile->AddInstance(TileTransform, true);
		}
	}
}

void AChessBoardManager::ClearBoard()
{
	if (ProceduralTileLight)
	{
		ProceduralTileLight->ClearInstances();
	}
	if (ProceduralTileDark)
	{	
		ProceduralTileDark->ClearInstances();
	}
}

void AChessBoardManager::TickInterpolatePieceMovement(float DeltaTime)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Tick Interpolate Piece works in Client"));
	}
	if (bIsMovingPiece)
	{
		if (!IsValid(MovingPiece))
		{
			// Piece got destroyed mid-move (e.g. captured), just stop.
			bIsMovingPiece = false;
			return;
		}

		MoveElapsedTime += DeltaTime;

		const float Alpha = (MoveDuration > 0.0f) ? FMath::Clamp(MoveElapsedTime / MoveDuration, 0.0f, 1.0f) : 1.0f;
		const float EasedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);

		const FVector NewLocation = FMath::Lerp(MoveStartLocation, MoveTargetLocation, EasedAlpha);
		MovingPiece->SetActorLocation(NewLocation);

		if (Alpha >= 1.0f)
		{
			bIsMovingPiece = false;
			MovingPiece = nullptr;
		}
	}
}

void AChessBoardManager::OnRep_SelectedPiece()
{
	OnSelectedPieceChanged.Broadcast(SelectedPiece, CurrentSelectingPlayer);
}

