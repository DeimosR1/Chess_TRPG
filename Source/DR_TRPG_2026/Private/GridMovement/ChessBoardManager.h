// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessBoardManager.generated.h"

class AChessPlayerState;
class AChessGameMode;
class APChessPiece;
class ABoardGridHighlight;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedPieceChanged, APChessPiece*, NewSelectedPiece);

UCLASS()
class AChessBoardManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessBoardManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	#if WITH_EDITOR
		// Regenerates the board tiles live in the editor whenever a relevant property changes.
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// World-space size of one grid square. Must match GridSize on APChessPiece.
	UPROPERTY(EditAnywhere, Category="Grid")
	float GridSize = 100.0f;
 
	UPROPERTY(EditAnywhere, Category="Grid")
	int32 BoardWidth = 8;
 
	UPROPERTY(EditAnywhere, Category="Grid")
	int32 BoardHeight = 8;
 
	// Blueprint class to spawn on each reachable square when a piece is selected.
	UPROPERTY(EditAnywhere, Category="Grid")
	TSubclassOf<ABoardGridHighlight> HighlightTileClass;
 
	UFUNCTION(BlueprintCallable, Category="Grid")
	FIntPoint WorldToGrid(const FVector& WorldLocation) const;
 
	UFUNCTION(BlueprintCallable, Category="Grid")
	FVector GridToWorld(const FIntPoint& GridCoord) const;
 
	// Called once per piece (from APChessPiece::BeginPlay) so the board knows where everyone starts.
	UFUNCTION(BlueprintCallable, Category="Grid")
	void RegisterPiece(APChessPiece* Piece, const FIntPoint& GridCoord);
 
	// Selects a piece, computes its legal moves, and spawns highlight tiles on them.
	// For Multiplayer it asks for the Player State of the requesting Client.
	UFUNCTION(BlueprintCallable, Category="Grid")
	void SelectPiece(APChessPiece* Piece, AChessPlayerState* RequestingPlayerState);
 
	UFUNCTION(BlueprintCallable, Category="Grid")
	void ClearSelection();
 
	// Moves the currently selected piece to TargetCoord if it's one of its legal moves.
	// Returns true if the move happened.
	// The Player state is to check if the one trying to move it is the current player.
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool TryMoveSelectedPieceTo(const FIntPoint& TargetCoord, AChessPlayerState* RequestingPlayerState);
 
	UFUNCTION(BlueprintCallable, Category="Grid")
	APChessPiece* GetPieceAt(const FIntPoint& GridCoord) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	TArray<APChessPiece*> GetAllPiecesOnBoard();
	
	// For Smooth Movement
	UFUNCTION()
	void StartSmoothMove(APChessPiece* Piece, const FVector& TargetLocation);
	
	// For Piece Deselection
	UFUNCTION(BlueprintCallable, Category="Grid")
	void DeselectPiece(AChessPlayerState* RequestingPlayerState);
	
	//For UI
	UPROPERTY(BlueprintAssignable, Category="Grid")
	FOnSelectedPieceChanged OnSelectedPieceChanged;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 
private:
	
	//The ChessBoard Model
	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY()
	TMap<FIntPoint, APChessPiece*> OccupiedSquares;
 
	UPROPERTY(ReplicatedUsing=OnRep_SelectedPiece)
	TObjectPtr<APChessPiece> SelectedPiece;
 
	UPROPERTY()
	TArray<TObjectPtr<ABoardGridHighlight>> ActiveHighlights;
 
	void SpawnHighlights(const TArray<FIntPoint>& ValidMoves);
	
	UPROPERTY(EditAnywhere, Category="Board Generation")
	TObjectPtr<UStaticMesh> TileMesh;
	
	UPROPERTY(EditAnywhere, Category="Board Generation")
	float TileMeshSize = 100.0f;

	UPROPERTY(EditAnywhere, Category="Board Generation")
	TObjectPtr<UMaterialInterface> LightTileMaterial;

	UPROPERTY(EditAnywhere, Category="Board Generation")
	TObjectPtr<UMaterialInterface> DarkTileMaterial;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Board Generation")
	void GenerateBoard();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Board Generation")
	void ClearBoard();
	
	UPROPERTY()
	TObjectPtr<AChessGameMode> GameMode;
	
	//These are the ones generated so the board generates. This way the Board size can be freely changed.
	UPROPERTY(VisibleAnywhere, Category="Board Generation")
	TObjectPtr<UInstancedStaticMeshComponent> ProceduralTileLight;

	UPROPERTY(VisibleAnywhere, Category="Board Generation")
	TObjectPtr<UInstancedStaticMeshComponent> ProceduralTileDark;
	
	//These are to try and smoothout the piece movement
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float DeltaTickSpeed = 0.0f;
	
	// How fast do the piece move? (In Seconds)
	UPROPERTY(EditAnywhere, Category="Movement")
	float MoveDuration = 0.3f;
 
	// Boolean to check when a piece is being interpolated (moved) to a new square
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	bool bIsMovingPiece = false;
 
	// We need to save the piece that is moving as selection is cleared when movement starts
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	TObjectPtr<APChessPiece> MovingPiece;
	
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	FVector MoveStartLocation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	FVector MoveTargetLocation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float MoveElapsedTime = 0.0f;
	
	UFUNCTION()
	void TickInterpolatePieceMovement(float DeltaTime); //To Add to tick so it is cleaner
	
	//For Multiplayer. This is to hold the current Player whose turn it is so the other player cannot interfere
	UPROPERTY()
	TObjectPtr<AChessPlayerState> CurrentSelectingPlayer;
	
	//For UI
	UFUNCTION()
	void OnRep_SelectedPiece();
};
