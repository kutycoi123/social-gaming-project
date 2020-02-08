#ifndef REVERSE_H
#define REVERSE_H

#include "BaseRule.h"
#include <string>
namespace GameSpecification{
class Reverse : public BaseRule{
    public:
        Reverse();
        Reverse(const std::string& list);
        std::string getList() const;
        void setList(const std::string& list);
    private:
        std::string _list;
};
}
#endif
