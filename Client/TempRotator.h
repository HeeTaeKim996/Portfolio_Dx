#pragma once
#include "Engine/Monobehaviour.h"

class TempRotator : public Monobehaviour
{
	
	virtual void Update() override;



public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual shared_ptr<Component> Clone() const override;

};

