#pragma once
#include "Engine/IReturnMonobehaviour.h"

class MonobehaviourLoader : public IReturnMonobehaviour
{ 
public:
	virtual shared_ptr<Monobehaviour> ReturnMonobehaviour(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
};

