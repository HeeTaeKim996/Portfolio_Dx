#include "pch.h"
#include "MonobehaviourLoader.h"
#include "Engine/tinyxml2.h"

#include "CameraScript.h"
#include "Kachujin.h"
#include "TempRotator.h"

shared_ptr<Monobehaviour> MonobehaviourLoader::ReturnMonobehaviour(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	string className = parent->GetText();

	if (className == "CameraScript")
	{
		shared_ptr<CameraScript> cameraScript = make_shared<CameraScript>();
		cameraScript->LoadXML(doc, parent);
		return cameraScript;
	}
	else if (className == "Kachujin")
	{
		shared_ptr<Kachujin> kachujin = make_shared<Kachujin>();
		kachujin->LoadXML(doc, parent);
		return kachujin;
	}
	else if (className == "TempRotator")
	{
		shared_ptr<TempRotator> tempRotator = make_shared<TempRotator>();
		tempRotator->LoadXML(doc, parent);
		return tempRotator;
	}

	assert(0);
	return nullptr;
}
