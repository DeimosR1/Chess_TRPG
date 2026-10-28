// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerGameLogicHandler.generated.h"

UENUM()
enum class EPendingSessionAction : uint8
{
	None,
	CreateSession,
	LeaveToMenu,
	JoinInvite
};

// This is a Script that will hold all the Joining and Creating Level ata neccesarry for the system to open and close the game levels

/**
 * 
 */

// Broadcast when a server search finishes. UI can bind to this and read the results.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServersFoundNative, bool, bHasFound);
//static const FName GAME_KEY(TEXT("TacticalChessGamekey"));
//static const FString GAME_KEY_VALUE(TEXT("TacticalChest_v1"));

UCLASS()
class UMultiplayerGameLogicHandler : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
 
	// This is for the host, creates a server as a listener
	void CreateServerAsListener(const FString& ServerName, const FString& LevelPath);
 
	// This will be when the client will search for servers. It is Binded to the Delegate
	void FindGameSessions();
	
	void StartGameSessionSearch();
 
	// Join one of the session by the index
	void JoinSessionsByIndex(int32 Index);
 
	int32 GetFoundSessionsNumber() const;
	FString GetFoundSessionsName(int32 Index) const;
 
	FOnServersFoundNative OnServersFound;
	
	UPROPERTY()
	bool bIsFriendsOnly = false;
	
	UFUNCTION()
	void LeaveGameAndReturnToMenu();
 
private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
 
	void CreateSessionInternal();
	bool IsLan() const;
	FUniqueNetIdPtr GetLocalNetId() const;
 
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
 
	FDelegateHandle CreateHandle;
	FDelegateHandle DestroyHandle;
	FDelegateHandle FindHandle;
	FDelegateHandle JoinHandle;
 
	// Stored while we wait for an old session to be destroyed
	FString PendingServerName;
	FString PendingLevelPath;
	
	IOnlineSessionPtr GetSessionInterface();
	
	UPROPERTY()
	bool bIsLeaving = false;
	EPendingSessionAction PendingAction = EPendingSessionAction::None;
	
	UPROPERTY(EditDefaultsOnly)
	UWorld* MainMenu;
	
	UFUNCTION()
	void OpenMainMenuLevel();
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
	
	FDelegateHandle OnNetworkFailureHandle;
	FDelegateHandle OnTravelFailureHandle;
	
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
	FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void JoinSessionFromResult(const FOnlineSessionSearchResult& Result);

	FDelegateHandle OnInviteAcceptedHandle;
	FOnlineSessionSearchResult OnPendingInviteResult;
	
};
