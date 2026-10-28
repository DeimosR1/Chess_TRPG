// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/MultiplayerGameLogicHandler.h"

#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

//Look Into transfering Server to preserve the instances when other player disconnects
//Online Identity interface to get player name on the turn change
static const FName SERVER_NAME_KEY(TEXT("ServerName"));

//Remove if it causes issues
//static const FName GAME_KEY(TEXT("TacticalChessGameKey"));
//static const FString GAME_KEY_VALUE(TEXT("TacticalChess_v1"));
 
void UMultiplayerGameLogicHandler::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (IOnlineSubsystem* Sub = Online::GetSubsystem(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Online subsystem: %s"), *Sub->GetSubsystemName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No online subsystem!"));
	}
	
	if (GEngine)
	{
		OnNetworkFailureHandle = GEngine->OnNetworkFailure().AddUObject(this, &UMultiplayerGameLogicHandler::HandleNetworkFailure);
		OnTravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &UMultiplayerGameLogicHandler::HandleTravelFailure);
	}
	
	if (IOnlineSessionPtr Sessions = GetSessionInterface())
	{
		OnInviteAcceptedHandle = Sessions->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UMultiplayerGameLogicHandler::OnSessionUserInviteAccepted));
	}
		
}
 
void UMultiplayerGameLogicHandler::Deinitialize()
{
	if (GEngine)
	{
		GEngine->OnNetworkFailure().Remove(OnNetworkFailureHandle);
		GEngine->OnTravelFailure().Remove(OnTravelFailureHandle);
	}
	
	if (GetSessionInterface())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(OnInviteAcceptedHandle);
		
	}
	Super::Deinitialize();
}
 
bool UMultiplayerGameLogicHandler::IsLan() const
{
	//If There is no Internet it will Default to Lan Only,
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (Subsystem && Subsystem->GetSubsystemName() == TEXT("NULL"))
	{
		return true;
	}
	return false;
}

FUniqueNetIdPtr UMultiplayerGameLogicHandler::GetLocalNetId() const
{
	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	
	if (LocalPlayer)
	{
		FUniqueNetIdPtr NetIDptr = LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId();
		if (NetIDptr.IsValid())
		{
			return NetIDptr;
		}
	}
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Identity)
		{
			return Identity->GetUniquePlayerId(0);
		}
	}
	return nullptr;
}

IOnlineSessionPtr UMultiplayerGameLogicHandler::GetSessionInterface()
{
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
	return SessionInterface;
}

void UMultiplayerGameLogicHandler::OpenMainMenuLevel()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, FName(TEXT("/Game/_GameFiles/Levels/MainMenu")));
	}
	bIsLeaving = false;
}

void UMultiplayerGameLogicHandler::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver,
	ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Warning, TEXT("Network failure: %s"), *ErrorString);
	LeaveGameAndReturnToMenu();
}

void UMultiplayerGameLogicHandler::HandleTravelFailure(UWorld* World,
	ETravelFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Warning, TEXT("Travel failure: %s"), *ErrorString);
	LeaveGameAndReturnToMenu();
}

void UMultiplayerGameLogicHandler::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
	FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Invite accepted. Success: %d"), bWasSuccessful);

	if (!bWasSuccessful || !InviteResult.IsValid()) { return; }
	
	JoinSessionFromResult(InviteResult);
}

void UMultiplayerGameLogicHandler::JoinSessionFromResult(const FOnlineSessionSearchResult& Result)
{
	if (!GetSessionInterface()) { return; }

	FUniqueNetIdPtr NetId = GetLocalNetId();
	if (!NetId.IsValid()) { return; }
	
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		OnPendingInviteResult = Result;
		PendingAction = EPendingSessionAction::JoinInvite;
		DestroyHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, 
			&UMultiplayerGameLogicHandler::OnDestroySessionComplete));
		SessionInterface->DestroySession(NAME_GameSession);
		return;
	}
	
	JoinHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, 
		&UMultiplayerGameLogicHandler::OnJoinSessionComplete));

	if (!SessionInterface->JoinSession(*NetId, NAME_GameSession, Result))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
		UE_LOG(LogTemp, Warning, TEXT("Invite JoinSession failed to start"));
	}
}


void UMultiplayerGameLogicHandler::CreateServerAsListener(const FString& ServerName, const FString& LevelPath)
{
	if (!GetSessionInterface()) {return;}
 
	PendingServerName = ServerName;
	PendingLevelPath = LevelPath;
 
	// Only one session of a given name can exist, so remove any old one first
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		//bCreateAfterDestroy = true;
		PendingAction = EPendingSessionAction::CreateSession;
		DestroyHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, 
			&UMultiplayerGameLogicHandler::OnDestroySessionComplete));
		SessionInterface->DestroySession(NAME_GameSession);
		return;
	}
 
	CreateSessionInternal();
}
 
void UMultiplayerGameLogicHandler::CreateSessionInternal()
{
	FUniqueNetIdPtr NetId = GetLocalNetId();
	if (!NetId)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid Local Net ID. Steam is either not running or there is another issue."));
		return;
	}
 
	FOnlineSessionSettings Settings;
	Settings.bIsLANMatch = IsLan();
	Settings.NumPublicConnections = 2; //This is an exclusively Two Player Game, we don't need more.
	Settings.bShouldAdvertise = true;
	Settings.bAllowJoinInProgress = true; //Look into Starting Session through UI for Host
	Settings.bAllowJoinViaPresence = true;
	Settings.bUsesPresence = true;          // Required for Steam sessions to show up in searches
	Settings.bUseLobbiesIfAvailable = true; // Steam lobbies set to False for now. Look into Lobbies later
	Settings.bAllowJoinViaPresenceFriendsOnly = bIsFriendsOnly;  // Steam lobby type "Friends Only"
	Settings.bAllowInvites = true;
	Settings.Set(SERVER_NAME_KEY, PendingServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//Settings.Set(GAME_KEY, GAME_KEY_VALUE, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
 
	CreateHandle = GetSessionInterface()->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, 
		&UMultiplayerGameLogicHandler::OnCreateSessionComplete));
 
	if (!SessionInterface->CreateSession(*NetId, NAME_GameSession, Settings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
		UE_LOG(LogTemp, Warning, TEXT("Session Failed to be Created"));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Session Created"));
}
 
void UMultiplayerGameLogicHandler::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (IOnlineSessionPtr Sessions = GetSessionInterface())
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
	}

	const EPendingSessionAction Action = PendingAction;
	PendingAction = EPendingSessionAction::None;

	switch (Action)
	{
	case EPendingSessionAction::CreateSession:
		if (bWasSuccessful) { CreateSessionInternal(); }
		break;

	case EPendingSessionAction::LeaveToMenu:
		OpenMainMenuLevel();
		break;
		
	case EPendingSessionAction::JoinInvite:
		if (bWasSuccessful) { JoinSessionFromResult(OnPendingInviteResult); }
		break;

	default:
		break;
	}
}

void UMultiplayerGameLogicHandler::LeaveGameAndReturnToMenu()
{
	if (bIsLeaving) { return; }
	bIsLeaving = true;
	PendingAction = EPendingSessionAction::LeaveToMenu;

	IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions && Sessions->GetNamedSession(NAME_GameSession))
	{
		DestroyHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerGameLogicHandler::OnDestroySessionComplete));

		if (!Sessions->DestroySession(NAME_GameSession))
		{
			// Destroy did not start so we leave anyway
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
			PendingAction = EPendingSessionAction::None;
			OpenMainMenuLevel();
		}
		return;
	}
	
	PendingAction = EPendingSessionAction::None;
	OpenMainMenuLevel();
}

void UMultiplayerGameLogicHandler::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GetSessionInterface()->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
 
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Create session failed"));
		return;
	}
 
	// Session exists on Steam, now actually load the map as a listen server
	if (UWorld* World = GetWorld())
	{
		// Ensure options string properly formats the listen parameter
		FString TravelURL = FString::Printf(TEXT("%s?listen"), *PendingLevelPath);
		UGameplayStatics::OpenLevel(World, FName(*TravelURL));
		UE_LOG(LogTemp, Warning, TEXT("Session Fully Created and Travelling to: %s"), *TravelURL);
	}
}
 
void UMultiplayerGameLogicHandler::FindGameSessions()
{
	if (!GetSessionInterface()) { OnServersFound.Broadcast(false); return; }

	//IOnlineFriendsPtr Friends = Online::GetSubsystem(GetWorld())->GetFriendsInterface();
	//if (!Friends.IsValid()) { OnServersFound.Broadcast(false); return; }

	// Make sure the friend list is loaded first
	TObjectPtr<UMultiplayerGameLogicHandler> LogicHandlerRef(this);
	/*Friends->ReadFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), FOnReadFriendsListComplete::CreateLambda([LogicHandlerRef](int32, bool bOk, const FString&, const FString&)
	{
		if (LogicHandlerRef){LogicHandlerRef->StartGameSessionSearch();}
	}));*/
	if (LogicHandlerRef){LogicHandlerRef->StartGameSessionSearch();}
}

void UMultiplayerGameLogicHandler::StartGameSessionSearch()
{
	if (!GetSessionInterface()) { OnServersFound.Broadcast(false); return; }
 
	FUniqueNetIdPtr NetId = GetLocalNetId();
	if (!NetId.IsValid()) { OnServersFound.Broadcast(false); return; }
 
	SearchSettings = MakeShared<FOnlineSessionSearch>();
	SearchSettings->bIsLanQuery = IsLan();
	SearchSettings->MaxSearchResults = 10000; // Steam test App ID 480 is shared by everyone, so use a big number and filter
	SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	//SearchSettings->QuerySettings.Set(GAME_KEY, GAME_KEY_VALUE, EOnlineComparisonOp::Equals);
 
	FindHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerGameLogicHandler::OnFindSessionsComplete));
 
	if (!SessionInterface->FindSessions(*NetId, SearchSettings.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached the On Servers Found Broadcast as a Failed"));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
		OnServersFound.Broadcast(false);
	}
}

void UMultiplayerGameLogicHandler::OnFindSessionsComplete(bool bWasSuccessful)
{
	GetSessionInterface()->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);

	if (bWasSuccessful && SearchSettings.IsValid())
	{
		//IOnlineFriendsPtr Friends = Online::GetSubsystem(GetWorld())->GetFriendsInterface();
		//const FString ListName = EFriendsLists::ToString(EFriendsLists::Default);

		/*SearchSettings->SearchResults.RemoveAll(
			[&](const FOnlineSessionSearchResult& R)
			{
				const FUniqueNetIdPtr Owner = R.Session.OwningUserId;
				return !Owner.IsValid() || !Friends.IsValid()
					|| !Friends->IsFriend(0, *Owner, ListName);
			});*/
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Reached On Servers found %hs"), bWasSuccessful ? "true" : "false");
	OnServersFound.Broadcast(bWasSuccessful);
}

FString UMultiplayerGameLogicHandler::GetFoundSessionsName(int32 Index) const

{
	if (!SearchSettings.IsValid() || !SearchSettings->SearchResults.IsValidIndex(Index)) return FString();
 
	const FOnlineSessionSearchResult& Result = SearchSettings->SearchResults[Index];
	FString Name;
	if (!Result.Session.SessionSettings.Get(SERVER_NAME_KEY, Name))
	{
		Name = Result.Session.OwningUserName; // Fallback
	}
	return Name;
}

void UMultiplayerGameLogicHandler::JoinSessionsByIndex(int32 Index)
{
	if (!GetSessionInterface() || !SearchSettings.IsValid()) {return;}
	if (!SearchSettings->SearchResults.IsValidIndex(Index)) {return;}
 
	FUniqueNetIdPtr NetId = GetLocalNetId();
	if (!NetId.IsValid()) {return;}
	
	JoinHandle = GetSessionInterface()->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, 
		&UMultiplayerGameLogicHandler::OnJoinSessionComplete));
 
	if (!GetSessionInterface()->JoinSession(*NetId, NAME_GameSession, SearchSettings->SearchResults[Index]))
	{
		GetSessionInterface()->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
	}
}
 


int32 UMultiplayerGameLogicHandler::GetFoundSessionsNumber() const
{
	if (SearchSettings.IsValid())
	{
		return SearchSettings->SearchResults.Num();
	}
	
	return 0;

}

void UMultiplayerGameLogicHandler::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
 
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join session failed (%d)"), static_cast<int32>(Result));
		return;
	}
 
	// Steam gives us a connect string (a Steam ID address), not a normal IP
	FString ConnectString;
	if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
		{
			UE_LOG(LogTemp, Warning, TEXT("Join session successful"))
			PC->ClientTravel(ConnectString, TRAVEL_Absolute);
		}
	}
}
 
