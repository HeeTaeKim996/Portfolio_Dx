#pragma once
#include "Engine/Monobehaviour.h"

class Kachujin : public Monobehaviour
{
public:
	virtual void Awake() override;
	virtual void Update() override;



	void Temp();

public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual shared_ptr<Component> Clone() const override;

};

