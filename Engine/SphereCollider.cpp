#include "pch.h"
#include "SphereCollider.h"
#include "MyMathUtils.h"
#include "ResourceManager.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Physics.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"

SphereCollider::SphereCollider(shared_ptr<Shader> shader) : Super(ColliderType::Sphere, shader)
{
	_mesh = RESOURCES->Get<Mesh>(L"Sphere");

}

SphereCollider::~SphereCollider()
{

}



void SphereCollider::Render()
{
	assert(_mesh != nullptr);

	float scaleFloat = _boundingSphere.radius * 2.f;

	Vec3 scale(scaleFloat, scaleFloat, scaleFloat);

	shared_ptr<Transform> transform = GetGameObject()->GetTransform();

	assert(transform != nullptr);

	Quaternion rot = transform->GetRotation();
	Vec3 translation = transform->GetPosition() + _offset;

	Matrix world(scale, rot, translation);
	RENDER->PushTransformData(TransformDesc{ world });

	uint32 stride = _mesh->GetVertexBuffer()->GetStride();
	uint32 offset = _mesh->GetVertexBuffer()->GetOffset();

	DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(2, (uint8)MeshRenderer::Pass::WIREFRAME, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
}

void SphereCollider::UpdateColliderTransform(shared_ptr<Transform> transform)
{
	_boundingSphere.center = transform->GetPosition() + _offset;

	Vec3 scale = transform->GetScale();
	float maxScale = max(max(fabs(scale.X), fabs(scale.Y)), fabs(scale.Z));

	_boundingSphere.radius = _radius * maxScale;


	if (proxyId == PROXY_ID_IS_NOT_ASSIGNED)
		return;

	Vec3 minAABB(_boundingSphere.center.X - _boundingSphere.radius, _boundingSphere.center.Y - _boundingSphere.radius,
		_boundingSphere.center.Z - _boundingSphere.radius);
	Vec3 maxAABB(_boundingSphere.center.X + _boundingSphere.radius, _boundingSphere.center.Y + _boundingSphere.radius,
		_boundingSphere.center.Z + _boundingSphere.radius);


	//ClProxy& proxy = PHYSICS->GetProxy(proxyId);
	ClProxy* proxy;
	if (!PHYSICS->GetProxy(proxyId, proxy))
		return;
	proxy->AABB.min = minAABB;
	proxy->AABB.max = maxAABB;
	proxy->isDirty = true;
}


#if 1
bool SphereCollider::Intersects(const myRay& ray, OUT float& distance)
{
	return _boundingSphere.Intersects(ray, distance);

	return false;
}
#endif

#if 0 // Obsolate
bool SphereCollider::Intersects_AssumeNormal(const Vec3& rayOrigin, const Vec3& NORMALIZED_rayDirection, OUT float& distance)
{
	return _boundingSphere.Intersects_AssumeNomral(rayOrigin, NORMALIZED_rayDirection, distance);
}
#endif

bool SphereCollider::Intersects(SphereCollider* otherSphere, ContactInfo& penetrationDepth)
{
	return _boundingSphere.Intersects(otherSphere->GetBoundingSphere(), penetrationDepth);
}

bool SphereCollider::Intersects(BoxCollider* otherBox, ContactInfo& collisionInfo)
{
	bool ret = otherBox->Intersects(this, collisionInfo);

	collisionInfo.normal = -collisionInfo.normal;

	return ret;
}

bool SphereCollider::Intersects(CapsuleCollider* otherCapsule, ContactInfo& collisionInfo)
{
	bool ret = otherCapsule->Intersects(this, collisionInfo);

	collisionInfo.normal = -collisionInfo.normal;

	return ret;
}

Vec3 SphereCollider::GetLowestPoint() const
{
	return _boundingSphere.LosestPoint();
}

shared_ptr<Component> SphereCollider::Clone() const
{
	shared_ptr<SphereCollider> copy = make_shared<SphereCollider>(*this);

	return copy;
}
