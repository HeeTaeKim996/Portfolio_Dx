#pragma once
#include "ResourceBase.h"
#include "Geometry.h"

class Mesh : public ResourceBase
{
	using Super = ResourceBase;

public:
	Mesh();
	virtual ~Mesh();

	void CreateQuad();
	void CreateCube();
	void CreateCube(float sizeX, float sizeY, float sizeZ);
	void CreateGrid(int32 sizeX, int32 sizeZ);
	void CreateSphere();
	void CreateCylinder();

	shared_ptr<VertexBuffer> GetVertexBuffer() { return _vertexBuffer; }
	shared_ptr<IndexBuffer> GetIndexBuffer() { return _indexBuffer; }

private:
	void CreateBuffers();

private:
	shared_ptr<Geometry<VertexTextureNormalTangentData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
};

