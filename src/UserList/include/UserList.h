#ifndef USERLIST_H
#define USERLIST_H

#include "User.h"
#include "Invitation.h"
#include <optional>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <memory>

struct UserIdHash
{
    size_t operator()(const UserId& id) const { 
        return std::hash<uintptr_t>()(id.getId());
    } 
};

class UserList {
public:
    UserList();
    bool onConnect(const UserId& id);
    bool onDisconnect(const UserId& id);

    std::optional<User> getUser(const UserId& id);
    std::optional<std::unique_ptr<User>> getUserRef(const UserId& id);

    void removeUsersFromGameSession(const Invitation& code);

    std::unordered_map<UserId, User, UserIdHash>::iterator begin();
    std::unordered_map<UserId, User, UserIdHash>::iterator end();
    size_t size() const noexcept;
private:
    std::unordered_map<UserId, User, UserIdHash> _idToUserMap;
};

#endif