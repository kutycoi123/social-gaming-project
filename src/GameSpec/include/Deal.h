#ifndef DEAL_H
#define DEAL_H

#include "BaseRule.h"
#include "AbstractSpec.h"
#include <nlohmann/json.hpp>
#include <string>
namespace GameSpecification{
class Deal : public BaseRule{
    public:
        Deal();
        Deal(const nlohmann::json&);
        std::string getFrom() const;
        SpecValue getTo() const;
        int getCount() const;
		void process(GameState&) override;
        void parseRule(const nlohmann::json&);
    private:
        std::string from;
        SpecValue to;
        int count;
};
}
#endif
