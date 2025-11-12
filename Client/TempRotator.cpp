#include "pch.h"
#include "TempRotator.h"
#include "Engine/Transform.h"
#include "Engine/TimeManager.h"
#include "Engine/tinyxml2.h"

void TempRotator::Update()
{
	GetTransform()->AddLocalUnitYRotation(80.f * DT);
}

void TempRotator::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{

}

void TempRotator::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	parent->SetText("TempRotator");
}

shared_ptr<Component> TempRotator::Clone() const
{
	return make_shared<TempRotator>(*this);
}