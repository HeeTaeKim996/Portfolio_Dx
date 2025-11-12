#include "pch.h"
#include "Kachujin.h"
#include "Engine/RenderManager.h"
#include "Engine/TimeManager.h"
#include "Engine/tinyxml2.h"
#include "Engine/Utils.h"
#include "Engine/Transform.h"
#include "Engine/Scene.h"
//#include "Engine/Scene.h"

void Kachujin::Awake()
{
	DoTimer(1000, &Kachujin::Temp);
}

void Kachujin::Update()
{
	//GetTransform()->AddPosition(myVec3(20.f, 0.f, 0.f) * DT);
}

void Kachujin::Temp()
{
	//GetGameObject()->GetScene()->DestroyGameObject(GetGameObject()->shared_from_this());
}

void Kachujin::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{

}

void Kachujin::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	parent->SetText("Kachujin");
}

shared_ptr<Component> Kachujin::Clone() const
{
	shared_ptr<Kachujin> copy = make_shared<Kachujin>(*this);

	return copy;
}
