#pragma once

class BaseCollider;

struct Collision
{
	shared_ptr<BaseCollider> other;
	Vec3 normal;
};