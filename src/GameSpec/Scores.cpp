#include "Scores.h"

using GameSpecification::Scores;
using json = nlohmann::json;
Scores::Scores(): BaseRule("scores"), score(0), ascending(false){}

Scores::Scores(double score, bool ascending): BaseRule("scores"), score(score), ascending(ascending){}

double Scores::getScore() const{
    return score;
}

bool Scores::getAscending() const{
    return ascending;
}

void Scores::process(GameState& gameState){
    
}

void Scores::parseRule(const json &ruleJson){
    try{
        score = ruleJson.at("score").get<int>();
        ascending = ruleJson.at("ascending").get<bool>();
    }catch(json::exception &e){
        std::cout << e.what() << "\n";
    }
}