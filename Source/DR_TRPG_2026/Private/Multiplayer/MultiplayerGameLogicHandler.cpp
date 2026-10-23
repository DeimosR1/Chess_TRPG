// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/MultiplayerGameLogicHandler.h"

#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"

//Look Into transfering Server to preserve the instances when other player disconnects
//Online Identity interface to get player name on the turn change
static const FName SERVER_NAME_KEY(TEXT("ServerName"));
 
void UMultiplayerGameLogicHandler::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
 
void UMultiplayerGameLogicHandler::Deinitialize()
{
	if (GetSessionInterface())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
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
		return LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId();
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


void UMultiplayerGameLogicHandler::CreateServerAsListener(const FString& ServerName, const FString& LevelPath)
{
	if (!GetSessionInterface()) {return;}
 
	PendingServerName = ServerName;
	PendingLevelPath = LevelPath;
 
	// Only one session of a given name can exist, so remove any old one first
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		bCreateAfterDestroy = true;
		DestroyHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerGameLogicHandler::OnDestroySessionComplete));
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
	Settings.bUseLobbiesIfAvailable = true; // Steam lobbies
	Settings.bAllowJoinViaPresenceFriendsOnly = bIsFriendsOnly;  // Steam lobby type "Friends Only"
	Settings.bAllowInvites = true;
	Settings.Set(SERVER_NAME_KEY, PendingServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
 
	CreateHandle = GetSessionInterface()->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerGameLogicHandler::OnCreateSessionComplete));
 
	if (!SessionInterface->CreateSession(*NetId, NAME_GameSession, Settings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateHandle);
		UE_LOG(LogTemp, Warning, TEXT("Session Failed to be Created"));
	}
}
 
void UMultiplayerGameLogicHandler::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GetSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(DestroyHandle);
	if (bCreateAfterDestroy)
	{
		bCreateAfterDestroy = false;
		if (bWasSuccessful) {CreateSessionInternal();}
	}
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
		World->ServerTravel(PendingLevelPath + TEXT("?listen"));
	}
}
 
void UMultiplayerGameLogicHandler::FindGameSessions()
{
	if (!GetSessionInterface()) { OnServersFound.Broadcast(false); return; }

	IOnlineFriendsPtr Friends = Online::GetSubsystem(GetWorld())->GetFriendsInterface();
	if (!Friends.IsValid()) { OnServersFound.Broadcast(false); return; }

	// Make sure the friend list is loaded first
	TObjectPtr<UMultiplayerGameLogicHandler> LogicHandlerRef(this);
	Friends->ReadFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), FOnReadFriendsListComplete::CreateLambda([LogicHandlerRef](int32, bool bOk, const FString&, const FString&)
	{
		if (LogicHandlerRef){LogicHandlerRef->StartGameSessionSearch();}
	}));
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
 
	FindHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerGameLogicHandler::OnFindSessionsComplete));
 
	if (!SessionInterface->FindSessions(*NetId, SearchSettings.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);
		OnServersFound.Broadcast(false);
	}
}

void UMultiplayerGameLogicHandler::OnFindSessionsComplete(bool bWasSuccessful)
{
	GetSessionInterface()->ClearOnFindSessionsCompleteDelegate_Handle(FindHandle);

	if (bWasSuccessful && SearchSettings.IsValid())
	{
		IOnlineFriendsPtr Friends = Online::GetSubsystem(GetWorld())->GetFriendsInterface();
		const FString ListName = EFriendsLists::ToString(EFriendsLists::Default);

		SearchSettings->SearchResults.RemoveAll(
			[&](const FOnlineSessionSearchResult& R)
			{
				const FUniqueNetIdPtr Owner = R.Session.OwningUserId;
				return !Owner.IsValid() || !Friends.IsValid()
					|| !Friends->IsFriend(0, *Owner, ListName);
			});
	}

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
	if (!SessionInterface || !SearchSettings.IsValid()) {return;}
	if (!SearchSettings->SearchResults.IsValidIndex(Index)) {return;}
 
	FUniqueNetIdPtr NetId = GetLocalNetId();
	if (!NetId.IsValid()) {return;}
 
	JoinHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerGameLogicHandler::OnJoinSessionComplete));
 
	if (!SessionInterface->JoinSession(*NetId, NAME_GameSession, SearchSettings->SearchResults[Index]))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinHandle);
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
			PC->ClientTravel(ConnectString, TRAVEL_Absolute);
		}
	}
}
 
