#include "Server.h"
#include <nlohmann/json.hpp>

#include "GameSessionManager.h"
#include "User.h"

#include "Command.h"

#include <atomic>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <vector>

//TODO: MZEGAR: MOVE THIS, WE SHOULDN'T HAVE GLOBALS
UserList users = UserList();

struct GlobalMessage{
	std::string message;
	//something to specifiy private message, broadcast to specifiv session, broadcast globally (all sessions)
	//maybe user id?, session id?
};

static std::atomic<bool> exit_thread_flag{false};
static const std::string SERVER_CONFIGURATION_FILE_LOCATION = "config/ServerProperties.json";
 
static GlobalMessage globalMessage = {""};
static std::pair<bool, Invitation> createSession(networking::Message, User);
static bool joinSession(Command, User);
//main thread
static void OnDisconnect(networking::Connection);
static void OnConnect(networking::Connection);

//these two should definitlly be changed, iirc Hunar is working on them
static std::string getConfigurationFilePath(int, char* []);
static void checkValidConfigurationFile(const nlohmann::json&);

//message thread
static void handleMessages(networking::Server&);
static std::deque<networking::Message> gameServerUpdate(networking::Server&, const std::deque<networking::Message>&);
static std::deque<networking::Message> SendMessageToSession();
static std::deque<networking::Message> processMessages(networking::Server& server, const std::deque<networking::Message>& incoming);

static void AddMessageToCorrectSession(uintptr_t connectionID, std::string &message);
//player authentication. 
int main(int argc, char* argv[]){

	//Read Json
	std::string configurationFilePath = getConfigurationFilePath(argc, argv);
	std::ifstream configurationFile(configurationFilePath, std::ifstream::in);

	nlohmann::json configuration = nlohmann::json::parse(configurationFile);

	checkValidConfigurationFile(configuration);

	unsigned short port = configuration["DefaultPort"];
	std::string htmlpath = configuration["HTML Location"];

	std::cout << "starting server \nport: " << port << "\nhtml path: " << htmlpath << '\n';

	//Configure Server
  std::ifstream htmlFile{htmlpath};
	std::string htmlFileStr{std::istreambuf_iterator<char>(htmlFile),std::istreambuf_iterator<char>()};
  networking::Server server{port, htmlFileStr, OnConnect, OnDisconnect};

	std::thread messageHandling(handleMessages, std::ref(server));

	//while server up, process messages
	std::string message;
	do{
		globalMessage.message = message;
		std::getline(std::cin, message);
	} while (message != "shutdown");

	exit_thread_flag = true;
	messageHandling.join();

	return 0;
}

//teacher provided functions
static void OnConnect(networking::Connection c) {
	std::cout << "New connection found: " << c.id << "\n";

	UserId id(c.id);
	users.onConnect(id);
}

//teacher provided functions
static void OnDisconnect(networking::Connection c) {
	std::cout << "Connection lost: " << c.id << "\n";
	
	UserId id(c.id);
	users.onDisconnect(id);
}

static std::string getConfigurationFilePath(int argc, char* argv[]){
	if(argc <= 1){
		return SERVER_CONFIGURATION_FILE_LOCATION;
		//TODO: if config file does not exist, create a default one
	}
	else{
		return argv[1];
	}
}

static void checkValidConfigurationFile(const nlohmann::json &configurationFile) {
	
	auto port = configurationFile.at("DefaultPort");
	//string to short conversion check
	if(port.get<unsigned short>() != port.get<std::intmax_t>()){
		std::cout << "Port out of range\n";
	    std::exit(-1);
	}

	std::string htmlpath = configurationFile.at("HTML Location");
	//html path check for valid file
	if(access(htmlpath.c_str(), R_OK) == -1){
	    std::cout << "Unable to open HTML index file: " << htmlpath << "\n";
        std::exit(-1);
	}
	
}

#pragma region ClientServerNetworkingThread

static void handleMessages(networking::Server& server){
	
	while (exit_thread_flag == false) {

		bool errorWhileUpdating = false;

		try {
			server.update();
		} 
		catch (std::exception& e) {
			std::cerr << "Exception from Server update:\n"
						<< " " << e.what() << "\n\n";
			errorWhileUpdating = true;
		}

		auto incoming = server.receive();
		

		auto outgoing = gameServerUpdate(server, incoming);
	 

		//auto outgoing = SendMessageToSession();
		server.send(outgoing);
	 
		if (errorWhileUpdating) {
			break;
		}

		sleep(1);
  	}

	return;
}
#pragma endregion end

 
std::deque<networking::Message> SendMessageToSession() {
	std::deque<networking::Message> commandResult;

    //TODO: Consider how this should be implemented with the new way users are
    // being handled
//	for (auto&  inviteGameElement : GameSessionManager::_invitationToGameSessionMap) {
//		//std::pair<UserId, User>
//
//		for(auto& user:inviteGameElement.second.getUsersInSession()) {
//
//			std::cout<<inviteGameElement.second.getUsersInSession().size()<<std::endl;
//			std::queue messages = inviteGameElement.second.getMessages();
//
//			int total =  messages.size();
//			std::cout<<"total messages:"<<total<<std::endl;
//			for( int i = 0; i < total;  i++) {
//				 	std::cout<< "sending all messages to user"<<std::endl;
//				 	commandResult.push_back(networking::Message{user.first.getId(), messages.front()});
//			 }
//		}
//		inviteGameElement.second.clearMessages();
//	}
 
	return commandResult;
}
	 
 //paramters connection Id message
 void AddMessageToCorrectSession(const uintptr_t userID, const std::string &message) {

	 auto iter =  GameSessionManager::userToInviteCode.find(userID);
	std::cout<<"looking for invitation"<<"\n";
	if(iter != GameSessionManager::userToInviteCode.end()) {
			std::cout<<"found invitation"<<"\n";

			auto it = GameSessionManager::_invitationToGameSessionMap.find(iter->second);
				
			std::cout<<iter->second.toString()<<std::endl;
				
			if(it == GameSessionManager::_invitationToGameSessionMap.end()) {
					std::cout<<"can't find session of user"<<"\n";
			}
				else {
					std::cout<<"added message to session queue";
					it->second.addMessages(message);
				}

		} 
 }
	
 

static std::deque<networking::Message> processMessages(networking::Server& server, const std::deque<networking::Message>& incoming) {
	std::deque<networking::Message> commandResult;
	
	
	for (networking::Message message : incoming) {

		std::string text = message.text;

		Command command = Command(text);
		UserId id(message.connection.id);
		User user(id);
		if (message.text.find("Configurations") != std::string::npos) {
			// call game engine
			try { nlohmann::json gameConfig = nlohmann::json::parse(message.text);
			} catch( nlohmann::json gameConfig ) {
				std::cout<< "incorrect json";
			}
		}

		if (message.text.find("set Owner") != std::string::npos) {
			// call game engine
			try { nlohmann::json gameConfig = nlohmann::json::parse(message.text);
			} catch( nlohmann::json gameConfig ) {
				std::cout<< "incorrect json";
			}
		}

		 
		switch (command.getCommandType())
		{
			case Command::CommandType::DISCONNECT:
			{
				server.disconnect(message.connection);
				break;
			}
			break;
			case Command::CommandType::SHUTDOWN:
			{
				std::cout << "shutdown game\n";
				// TODO: Requires Matthew's User MR
			}
			break;
			case Command::CommandType::START_GAME:
			{
				//print out those string games.
				//find gameSession based on code, push back message to queue

		
		 
				//AddMessageToCorrectSession(message.connection.id, "start game\n");
				std::cout << "start game\n";
				// TODO: Requires Matthew's User MR
			}
			break;
			case Command::CommandType::CREATE_SESSION:
			{
				std::pair<bool, Invitation> session = createSession(message, user);
				if (!session.first){
					message.text.append(" Error, Could not create lobby!");
				}
				else
				{
					message.text.append("\n Creating lobby: \n");
					message.text.append(" Here is the invitation code for your lobby: ");
					message.text.append(session.second.toString());
				}
			}
			break;
			case Command::CommandType::JOIN_SESSION:
			{
				if(!joinSession(command, user)){
					message.text.append(" Error, cannot join lobby!");
				}
				else{
					message.text.append("\n joining lobby!");
				}
			}	
			break;
			case Command::CommandType::USERNAME:
			{
			    auto name = command.getCommandArgument();
			    if (name.has_value()) {
                    users.getUserRef(user).setUserName(UserName(name.value()));
                }
			}
			break;
			case Command::CommandType::HELP:
			{
				message.text.append("\n List of user commands: \n");
				message.text.append(Command::getAllCommandDescriptions());
			}
			break;
			case Command::CommandType::NULL_COMMAND:
			{
				if(!command.isCommandProperlyFormatted()){
					std::cout << " Error, Invalid user command" << '\n';
					message.text.append(" Error, Invalid user command.");
				} else {
					message.text.append(" Error, command not found.");
				}
				std::cout << command.getCommandAsString() << '\n';
				break;
			}
			//for example something 
			//game[connection].message = blahblahblah

			//dummy code, remove later
			default:
			{
					//do nothing
			}
			break;
			}
		
		
		commandResult.push_back(networking::Message{message.connection, message.text});
	}
	return commandResult;
}

static std::deque<networking::Message> getGlobalMessages(){
	std::deque<networking::Message> result {};
	
	if(globalMessage.message != ""){

		for(auto& entry : users){
			User user = entry.second;
			result.push_back({networking::Connection{user.getUserIdValue()}, globalMessage.message});
		}

		globalMessage.message = "";
	}

	return result;
}

std::pair<bool, Invitation> createSession(networking::Message m, User user){
	GameSession init = GameSessionManager::createGameSession(user);
	GameSession &initRef = init;

	Invitation code = initRef.getInvitationCode();
	//TODO: add mapUserIDToInvitation

	//investigate  GameSessionManager::_invitationToGameSessionMap.insert(std::make_pair(userProvidedCode, session)); why doesn't it update invitation code
	AddMessageToCorrectSession(m.connection.id,  "joining lobby: " + code.toString());
	return std::make_pair(true, code);
 }

bool joinSession(Command command, User user){
	if(command.getCommandArgument()){
		Invitation userProvidedCode = Invitation::createInvitationFromStringInput(command.getCommandArgument().value());
		if(GameSessionManager::joinGameSession(user, userProvidedCode)){
			//AddMessageToCorrectSession(m.connection.id,m.text);
			return true;
		}
	}
	return false;
}


static std::deque<networking::Message> gameServerUpdate(networking::Server& server, const std::deque<networking::Message>& incoming) {
	std::deque<networking::Message> allMessages = {};

	std::deque<networking::Message> globalMessages = getGlobalMessages();
	allMessages.insert(allMessages.end(), globalMessages.begin(), globalMessages.end());

	//doesn't really make sense for command messages to be broadcasted to everyone, so only the person creating the command needs to see the server reply
	std::deque<networking::Message> commandMessages = processMessages(server, incoming);
	allMessages.insert(allMessages.end(), commandMessages.begin(), commandMessages.end());

	//TODO: update all games based on game logic (probs in gamesessionmanager)

	std::deque<networking::Message> gameMessages = GameSessionManager::getAllGameMessages();
	allMessages.insert(allMessages.end(), gameMessages.begin(), gameMessages.end());
	
	std::deque<networking::Message> lobbyMessages = GameSessionManager::getAllLobbyMessages();
	allMessages.insert(allMessages.end(), lobbyMessages.begin(), lobbyMessages.end());

	return allMessages;
}

#pragma endregion
