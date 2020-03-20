<<<<<<< HEAD
#ifndef EXTEND_H
#define EXTEND_H

#include "BaseRule.h"
#include <nlohmann/json.hpp>
#include <string>
namespace GameSpecification{
    class Extend : public BaseRule{
        public:
            Extend(const std::string& target, const ValueType& list);

            void process(GameState&) override; 

        private:
            std::string target;
            ValueType list;
    };
}
#endif
=======
#ifndef EXTEND_H
#define EXTEND_H

#include "BaseRule.h"
#include <string>
namespace GameSpecification{
    class Extend : public BaseRule{
        public:
            Extend(const std::string& target, const ValueType& list);

            void process(GameState&) override; 

        private:
            std::string target;
            ValueType list;
    };
}
#endif
>>>>>>> working on centralized parsing
