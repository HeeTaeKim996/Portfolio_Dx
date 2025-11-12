#include "pch.h"
#include "Camera.h"
#include "RenderManager.h"
#include "tinyxml2.h"

bool Camera::readyRender;
Matrix Camera::S_MatView = Matrix::Identity;
Matrix Camera::S_MatProjection = Matrix::Identity;
Matrix Camera::S_MatVInv = Matrix::Identity;

Camera::Camera() : Super(ComponentType::Camera)
{
	const GameDesc& gameDesc = GAME->GetGameDesc();
	_width = static_cast<float>(gameDesc.width);
	_height = static_cast<float>(gameDesc.height);
}

Camera::~Camera()
{

}

void Camera::Update()
{
	UpdateMatrix();

	RENDER->Update();

	readyRender = true;
}

void Camera::UpdateMatrix()
{
	Vec3 eyePosition = GetTransform()->GetPosition();

	//Vec3 focusPosition = eyePosition + GetTransform()->GetZAxis();
	//Vec3 upDirection = GetTransform()->GetYAxis();


	myMatrix3x3 mat = GetTransform()->GetRotationMatrix();
	Vec3 focusPosition = eyePosition + mat[2];
	Vec3 upDirection = mat[1];

	SetViewMatrix();
	SetProjectionMatrix();
	SetVInvMatrix(); // Pos 구하려 강의에서 구한 거라, 사실 GetTransform()->GetPosition(); 로 충분하지만, 혹시 모르니 처리
}

void Camera::SetViewMatrix()
{
	myMatrix3x3 rotMatrix = GetTransform()->GetRotationMatrix();
	myMatrix3x3 rotTransposedMatrix = rotMatrix.Transpose();
	myVec3 cameraInvPos = -GetTransform()->GetPosition();

	S_MatView = Matrix(
		Vec4(rotTransposedMatrix[0], 0),
		Vec4(rotTransposedMatrix[1], 0),
		Vec4(rotTransposedMatrix[2], 0),
		Vec4(rotMatrix[0].Dot(cameraInvPos), rotMatrix[1].Dot(cameraInvPos), rotMatrix[2].Dot(cameraInvPos), 1)
	);
}

void Camera::SetProjectionMatrix()
{
	float d = 1.f / tanf(_fov / 2.f);
	float a = _width / _height;
	float k = _far / (_far - _near);
	float l = (_near * _far) / (_near - _far);

	S_MatProjection = myMatrix4x4(
		myVec4(d / a, 0, 0, 0),
		myVec4(0, d, 0, 0),
		myVec4(0, 0, k, 1),
		myVec4(0, 0, l, 0)
	);
}

void Camera::SetVInvMatrix()
{
	myMatrix3x3 rotMatrix = GetTransform()->GetRotationMatrix();
	
	S_MatVInv = myMatrix4x4(
		myVec4(rotMatrix[0], 0),
		myVec4(rotMatrix[1], 0),
		myVec4(rotMatrix[2], 0),
		myVec4(GetTransform()->GetPosition(), 1)
	);
}

void Camera::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	XmlElement* cameraValuesElement = parent->FirstChildElement("CameraValues");

	_near = cameraValuesElement->FloatAttribute("Near");
	_far = cameraValuesElement->FloatAttribute("Far");
	_fov = cameraValuesElement->FloatAttribute("FOV");
}

void Camera::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	XmlElement* cameraElement = doc.NewElement("Camera");
	parent->LinkEndChild(cameraElement);

	XmlElement* cameraValuesElement = doc.NewElement("CameraValues");
	cameraElement->LinkEndChild(cameraValuesElement);

	cameraValuesElement->SetAttribute("Near", _near);
	cameraValuesElement->SetAttribute("Far", _far);
	cameraValuesElement->SetAttribute("FOV", _fov);

	// ※ 필드멤버 Width 와 Height 는 GAME 의 GameDesc 에서 참조하기 때문에 필요 없음

}

shared_ptr<Component> Camera::Clone() const
{
	shared_ptr<Camera> copy = make_shared<Camera>(*this);

	// TODO (깊은 복사가 필요한 부분 수동 입력)

	return copy;
}

