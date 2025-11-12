#pragma once
#include "Monobehaviour.h"

class IReturnMonobehaviour
{
public:
	virtual shared_ptr<Monobehaviour> ReturnMonobehaviour(tinyxml2::XMLDocument doc, tinyxml2::XMLElement* parent) =0;
};

