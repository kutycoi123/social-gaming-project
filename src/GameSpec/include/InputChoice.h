#ifndef INPUTCHOICERULE_H
#define INPUTCHOICERULE_H

#include "GameSpec.h"
#include "Message.h"

/*
	{ "rule": "input-choice",
	  "to": << a single player or audience member >>,
	  "prompt": << Message to send with request, as in "output" below  >>,
	  "choices": << list or name of a list to choose from >>
	  "result": << variable name in which to store the response >>
	
	  OPTIONAL
	  "timeout": << duration to wait for a response >>
	}

*/

namespace GameSpecification{
	class InputChoice: public BaseRule{
		
		public:
			InputChoice() : BaseRule("input-choice") {}
			InputChoice(const std::string& to, const Message& prompt, const SpecValue& choices, 
							const std::string& result, const std::string& timeout) 
							:  BaseRule("input-choice"), _to(to), _prompt(prompt), _choices(choices), 
							   _result(result), _timeout(timeout) {}
			std::string getTo() const;
			std::string getResult() const;	
			std::string getTimeout() const;
			Message getPrompt() const;
			SpecValue getChoices() const;
			void process() override; 
		private:
			std::string _to;	//represents field "to" in input choice rule
			Message _prompt; 
			SpecValue _choices;
			std::string _result; //represents field "result" in input choice rule
			std::string _timeout;
			
	};

}

#endif