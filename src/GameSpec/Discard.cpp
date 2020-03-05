#include "Discard.h"
using GameSpecification::Discard;
using GameSpecification::BaseRule;
using json = nlohmann::json;
Discard::Discard() : BaseRule("discard"), from(""), count(0){}

Discard::Discard(const std::string& from, const int count) : BaseRule("discard"), from(from), count(count){}

std::string Discard::getFrom() const{
    return from;
}

int Discard::getCount() const{
    return count;
}

void Discard::process(GameState& gameState) {
	//TODO: Add code to process discard rule
}

void Discard::parseRule(const json& ruleJson){
    try{
       this->from = ruleJson.at("from").get<std::string>();
       this->count = ruleJson.at("count").get<int>();
    }catch(json::exception& e){
        std::cout << e.what() << "\n";
    }
}
