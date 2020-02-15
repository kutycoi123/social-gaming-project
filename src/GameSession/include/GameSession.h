#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include <iostream> 
#include <string>
#include <list>
#include "Invitation.h"
#include "Game.h"
#include "User.h"
#include "UserList.h"
#include <queue>
class GameSession { 
    
public:
    GameSession(User& owner);
    Invitation getInvitationCode() const;
    size_t getTotalPlayerCount() const noexcept;
    std::string getSessionName() const;
    bool isGameStarted() const;

    void addUserToSession(User& user);
    void removeUserFromSession(User& user);
    void removeAllUsersfromSession();

    void setConfigurationSettings(std::string jsonSettings);
    void createInviteCode();
    void startGame();
    void clearMessages();
    UserList getUsersInSession();

    bool operator==(const GameSession& gameSession ) const {
        return _invitationCode == gameSession._invitationCode;
    }

    void addMessages(const std::string &message);

    std::queue<std::string> getMessages();
    bool doesUserOwnGame(const User& user) const;

    // WIP
    // std::list<Player> getPlayers();  // TODO: Link with the User class
    // void setPlayerInviteCodes();

private:
    // Game game;

    std::list<std::string> _ListSettingVars;
    std::list<std::string> _ListPerAudience;
    std::list<std::string> _ListRules;
    std::string _currentGame;
    UserList _usersInSession;

    std::queue<std::string> messages;

    std::string _sessionName;
    std::string _JSONSetting;

    const Invitation _invitationCode;

    int _gameID;

    bool _isGameStarted;
    User& _owner;

    // WIP
    // std::list<Player> _playersList;  // TODO: Link with the User class
};

#endif