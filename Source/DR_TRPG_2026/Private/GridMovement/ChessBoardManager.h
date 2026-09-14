// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessBoardManager.generated.h"

class AChessGameMode;
class APChessPiece;
class ABoardGridHighlight;

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
	UFUNCTION(BlueprintCallable, Category="Grid")
	void SelectPiece(APChessPiece* Piece);
 
	UFUNCTION(BlueprintCallable, Category="Grid")
	void ClearSelection();
 
	// Moves the currently selected piece to TargetCoord if it's one of its legal moves.
	// Returns true if the move happened.
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool TryMoveSelectedPieceTo(const FIntPoint& TargetCoord);
 
	UFUNCTION(BlueprintCallable, Category="Grid")
	APChessPiece* GetPieceAt(const FIntPoint& GridCoord) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	TArray<APChessPiece*> GetAllPiecesOnBoard();
 
private:
	
	//The ChessBoard Model
	UPROPERTY(EditAnywhere, Category="Mesh")
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY()
	TMap<FIntPoint, APChessPiece*> OccupiedSquares;
 
	UPROPERTY()
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
};
