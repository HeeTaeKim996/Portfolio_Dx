#include "pch.h"
#include "BoxCollider.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "SphereCollider.h"
#include "Physics.h"
#include "CapsuleCollider.h"


BoxCollider::BoxCollider(shared_ptr<Shader> shader) : Super(ColliderType::OBB, shader)
{
	_mesh = RESOURCES->Get<Mesh>(L"Cube");
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::Render()
{
	assert(_mesh != nullptr);

	shared_ptr<Transform> transform = GetGameObject()->GetTransform();
	assert(transform != nullptr);

	Vec3 scale = _boundingBox.halfExtents * 2.f;
	Quaternion rot(_boundingBox.axes);
	//Vec3 translation = transform->GetPosition() + rot.RotateVector(_offset);
	Vec3 translation = _boundingBox.center;

	myMatrix4x4 world(scale, rot, translation);
	RENDER->PushTransformData(TransformDesc{ world });

	uint32 stride = _mesh->GetVertexBuffer()->GetStride();
	uint32 offset = _mesh->GetVertexBuffer()->GetOffset();

	DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(2, (uint8)MeshRenderer::Pass::WIREFRAME, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
}

void BoxCollider::UpdateColliderTransform(shared_ptr<Transform> transform)
{
	//Quaternion quat = transform->GetRotation() * _collidersQuaternion;
	Quaternion quat =  _collidersQuaternion * transform->GetRotation();


	Vec3 transformScale = transform->GetScale();

	//_boundingBox.center = transform->GetPosition() + quat.RotateVector(_offset);
	_boundingBox.center = transform->GetPosition() + quat.RotateVector((_offset * transformScale));



	_boundingBox.halfExtents 
		= Vec3(transformScale.X * _halfExtents.X, transformScale.Y * _halfExtents.Y, transformScale.Z * _halfExtents.Z);


	_boundingBox.axes = quat.ToRotateMatrix();
	

	if (proxyId == PROXY_ID_IS_NOT_ASSIGNED)
		return;

	//ClProxy& proxy = PHYSICS->GetProxy(proxyId);
	ClProxy* proxy;
	if (!PHYSICS->GetProxy(proxyId, proxy))
		return;

	_boundingBox.ComputeAABB(proxy->AABB.min, proxy->AABB.max);
	
	proxy->isDirty = true;
}

#if 1
bool BoxCollider::Intersects(const myRay& ray, OUT float& distance)
{
	return _boundingBox.Intersects(ray, distance);
}
#endif

#if 0 // Obsolate
bool BoxCollider::Intersects_AssumeNormal(const Vec3& rayOrigin, const Vec3& NORMALIZED_rayDirection, OUT float& distance)
{
	return _boundingBox.Intersects_AssumeNormal(rayOrigin, NORMALIZED_rayDirection, distance);
}
#endif

bool BoxCollider::Intersects(SphereCollider* otherSphere, ContactInfo& peneDepth)
{
	return _boundingBox.Intersects_Sphere(otherSphere->GetBoundingSphere(), peneDepth);
}

bool BoxCollider::Intersects(BoxCollider* otherBox, ContactInfo& peneDepth)
{
	return _boundingBox.Intersects_OBB(otherBox->GetBoundingBox(), peneDepth);
}

bool BoxCollider::Intersects(CapsuleCollider* otherCapsule, ContactInfo& collisionInfo)
{
	bool ret = otherCapsule->Intersects(this, collisionInfo);

	collisionInfo.normal = -collisionInfo.normal;

	return ret;
}

Vec3 BoxCollider::GetLowestPoint() const
{
	return _boundingBox.LowestPoint();
}

shared_ptr<Component> BoxCollider::Clone() const
{
	shared_ptr<BoxCollider> copy = make_shared<BoxCollider>(*this);

	return copy;
}