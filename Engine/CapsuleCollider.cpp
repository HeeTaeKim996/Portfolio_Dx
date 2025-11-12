#include "pch.h"
#include "CapsuleCollider.h"

#include "ResourceManager.h"
#include "RenderManager.h"
#include "SphereCollider.h"
#include "Physics.h"
#include "BoxCollider.h"


CapsuleCollider::CapsuleCollider(shared_ptr<Shader> shader) : Super(ColliderType::Capsule, shader)
{
	_mesh = RESOURCES->Get<Mesh>(L"Sphere");
	tempAdditionalMesh = RESOURCES->Get<Mesh>(L"Cylinder");
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::Render()
{
	assert(_mesh != nullptr);

	shared_ptr<Transform> transform = GetGameObject()->GetTransform();
	assert(transform != nullptr);

	for (int i = 0; i < 2; i++)
	{
		float scaleFloat = _boundingCapsule.radius * 2.f;
		Vec3 scale(scaleFloat, scaleFloat, scaleFloat);

		Quaternion quat = transform->GetRotation() * _capsuleQuaternion; // top - bottom 정보 이용해서, quat 구하고 싶은데, 생각이 안남

		//Vec3 move = i == 0 ? _boundingCapsule.top - _center : _boundingCapsule.bottom - _center;
		Vec3 translation = i == 0 ? _boundingCapsule.top : _boundingCapsule.bottom;

		Matrix world(scale, quat, translation);
		RENDER->PushTransformData(TransformDesc{ world });

		uint32 stride = _mesh->GetVertexBuffer()->GetStride();
		uint32 offset = _mesh->GetVertexBuffer()->GetOffset();

		DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
		DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

		_shader->DrawIndexed(2, (uint8)MeshRenderer::Pass::WIREFRAME, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
	}



	{
		float yScale = (_boundingCapsule.top - _boundingCapsule.bottom).Size();
		float xzScale = _boundingCapsule.radius * 2.f;

		Vec3 scale = myVec3(xzScale, yScale, xzScale);
		Quaternion quat = transform->GetRotation() * _capsuleQuaternion; // top - bottom 정보 이용해서, quat 구하고 싶은데, 생각이 안남
		Vec3 translation = (_boundingCapsule.top + _boundingCapsule.bottom) / 2.f;

		Matrix world(scale, quat, translation);
		RENDER->PushTransformData(TransformDesc{ world });

		uint32 stride = tempAdditionalMesh->GetVertexBuffer()->GetStride();
		uint32 offset = tempAdditionalMesh->GetVertexBuffer()->GetOffset();

		DC->IASetVertexBuffers(0, 1, tempAdditionalMesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
		DC->IASetIndexBuffer(tempAdditionalMesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

		_shader->DrawIndexed(2, (uint8)MeshRenderer::Pass::WIREFRAME, tempAdditionalMesh->GetIndexBuffer()->GetCount(), 0, 0);

	}

}

void CapsuleCollider::UpdateColliderTransform(shared_ptr<Transform> transform)
{
	Vec3 center = transform->GetPosition() + _offset;
	
	Vec3 transformScale = transform->GetScale();

	float xzMaxScale = ::fmax(transformScale.X, transformScale.Z);

	_boundingCapsule.radius = _radius * xzMaxScale;

	Quaternion quat = transform->GetRotation() * _capsuleQuaternion;
	Vec3 halfVec = quat.RotateVector(myVec3(0.f, _halfHeight * transformScale.Y, 0.f));

	_boundingCapsule.top = center + halfVec;
	_boundingCapsule.bottom = center - halfVec;


	if (proxyId == PROXY_ID_IS_NOT_ASSIGNED)
		return;

	//ClProxy& proxy = PHYSICS->GetProxy(proxyId);
	ClProxy* proxy;
	if (!PHYSICS->GetProxy(proxyId, proxy))
		return;
	_boundingCapsule.ComputeAABB(proxy->AABB.min, proxy->AABB.max);
	proxy->isDirty = true;
} 

bool CapsuleCollider::Intersects(const myRay& ray, OUT float& distance)
{
	return _boundingCapsule.Intersects(ray, distance);
}

bool CapsuleCollider::Intersects(SphereCollider* otherSphere, ContactInfo& penerationDepth)
{
	return _boundingCapsule.Intersects(otherSphere->GetBoundingSphere(), penerationDepth);
}

bool CapsuleCollider::Intersects(BoxCollider* otherBox, ContactInfo& penetrationDepth)
{
	return _boundingCapsule.Intersects(otherBox->GetBoundingBox(), penetrationDepth);
}

bool CapsuleCollider::Intersects(CapsuleCollider* otherCapsule, ContactInfo& peneDepth)
{
	return _boundingCapsule.Intersects(otherCapsule->GetBoundingCapsule(), peneDepth);
}

Vec3 CapsuleCollider::GetLowestPoint() const
{
	return _boundingCapsule.LowestPoint();
}

shared_ptr<Component> CapsuleCollider::Clone() const
{
	shared_ptr<CapsuleCollider> copy = make_shared<CapsuleCollider>(*this);

	return copy;
}

