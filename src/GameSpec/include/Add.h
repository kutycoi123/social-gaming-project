#ifndef ADD_H
#define ADD_H

#include "BaseRule.h"
#include "AbstractSpec.h"
#include <string>
namespace GameSpecification{
class Add : public BaseRule{
    public:
        Add();
        Add(const std::string& to, const SpecValue& value);
        std::string getTo() const;
        SpecValue getValue() const;
        void setTo(const std::string& to);
        void setValue(const SpecValue& value);
    private:
        std::string _to;
        SpecValue _value;
};
}
#endif
