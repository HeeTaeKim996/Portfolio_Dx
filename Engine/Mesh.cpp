#include "pch.h"
#include "Mesh.h"
#include "GeometryHelper.h"

Mesh::Mesh() : Super(ResourceType::Mesh)
{

}

Mesh::~Mesh()
{
}

void Mesh::CreateQuad()
{
	_geometry = make_shared<Geometry<VertexTextureNormalTangentData>>();
	GeometryHelper::CreateQuad(_geometry);
	_name = L"Quad";
	CreateBuffers();
}

void Mesh::CreateCube()
{
	_geometry = make_shared<Geometry<VertexTextureNormalTangentData>>();
	GeometryHelper::CreateCube(_geometry);
	_name = L"Cube";
	CreateBuffers();
}

void Mesh::CreateCube(float sizeX, float sizeY, float sizeZ)
{
	_geometry = make_shared<Geometry<VertexTextureNormalTangentData>>();
	GeometryHelper::CreateCube(_geometry, sizeX, sizeY, sizeZ);
	_name = L"Cube_sizable";
	CreateBuffers();
}

void Mesh::CreateGrid(int32 sizeX, int32 sizeZ)
{
	_geometry = make_shared<Geometry<VertexTextureNormalTangentData>>();
	GeometryHelper::CreateGrid(_geometry, sizeX, sizeZ);
	_name = L"Grid";
	CreateBuffers();
}

void Mesh::CreateSphere()
{
	_geometry = make_shared<Geometry<VertexTextureNormalTangentData>>();
	GeometryHelper::CreateSphere(_geometry);
	_name = L"Sphere";
	CreateBuffers();
}

void Mesh::CreateCylinder()
{
	_geometry = make_shared<Geometry<VertexTextureNormalTangentData>>();
	GeometryHelper::CreateCylinder(_geometry);
	_name = L"Cylinder";
	CreateBuffers();
}

void Mesh::CreateBuffers()
{
	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_geometry->GetVertices());
	
	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_geometry->GetIndices());
}
