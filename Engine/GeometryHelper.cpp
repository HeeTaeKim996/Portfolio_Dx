#pragma once
#include "pch.h"
#include "GeometryHelper.h"


void GeometryHelper::CreateQuad(shared_ptr<Geometry<VertexColorData>> geometry, Color color)
{
	vector<VertexColorData> vtx;
	vtx.resize(4);


	vtx[0].position = Vec3(-0.5f, -0.5f, 0.f);
	vtx[0].color = color;

	vtx[1].position = Vec3(-0.5f, 0.5f, 0.f);
	vtx[1].color = color;

	vtx[2].position = Vec3(0.5f, -0.5f, 0.f);
	vtx[2].color = color;

	vtx[3].position = Vec3(0.5f, 0.5f, 0.f);
	vtx[3].color = color;
	geometry->SetVertices(vtx);

	vector<uint32> idx = { 0, 1, 2, 2, 1, 3 };
	geometry->SetIndices(idx);
}




/*------------------------------------
		   VertexTextureData
------------------------------------*/
void GeometryHelper::CreateQuad(shared_ptr<Geometry<VertexTextureData>> geometry)
{
	vector<VertexTextureData> vtx;
	vtx.resize(4);


	vtx[0].position = Vec3(-0.5f, -0.5f, 0.f);
	vtx[0].uv = Vec2(0, 1.f);

	vtx[1].position = Vec3(-0.5f, 0.5f, 0.f);
	vtx[1].uv = Vec2(0, 0);

	vtx[2].position = Vec3(0.5f, -0.5f, 0.f);
	vtx[2].uv = Vec2(1.f, 1.f);

	vtx[3].position = Vec3(0.5f, 0.5f, 0.f);
	vtx[3].uv = Vec2(1.f, 0.f);
	geometry->SetVertices(vtx);

	vector<uint32> idx = { 0, 1, 2, 2, 1, 3 };
	geometry->SetIndices(idx);
}

void GeometryHelper::CreateCube(shared_ptr<Geometry<VertexTextureData>> geometry)
{
	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<VertexTextureData> vtx(24);

	// �ո�
	vtx[0] = VertexTextureData{ Vec3(-w2, -h2, -d2), Vec2(0.f, 1.f) };
	vtx[1] = VertexTextureData{ Vec3(-w2, +h2, -d2), Vec2(0.f, 0.f) };
	vtx[2] = VertexTextureData{ Vec3(+w2, +h2, -d2), Vec2(1.f, 0.f) };
	vtx[3] = VertexTextureData{ Vec3(+w2, -h2, -d2), Vec2(1.f, 1.f) };

	// �޸�
	vtx[4] = VertexTextureData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f) };
	vtx[5] = VertexTextureData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f) };
	vtx[6] = VertexTextureData{ Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f) };
	vtx[7] = VertexTextureData{ Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f) };

	// ����
	vtx[8] = VertexTextureData{ Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f) };
	vtx[9] = VertexTextureData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f) };
	vtx[10] = VertexTextureData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f) };
	vtx[11] = VertexTextureData{ Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f) };

	// �Ʒ���
	vtx[12] = VertexTextureData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f) };
	vtx[13] = VertexTextureData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f) };
	vtx[14] = VertexTextureData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f) };
	vtx[15] = VertexTextureData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f) };
	// ���ʸ�
	vtx[16] = VertexTextureData{ Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f) };
	vtx[17] = VertexTextureData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f) };
	vtx[18] = VertexTextureData{ Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f) };
	vtx[19] = VertexTextureData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f) };
	// �����ʸ�
	vtx[20] = VertexTextureData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f) };
	vtx[21] = VertexTextureData{ Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f) };
	vtx[22] = VertexTextureData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f) };
	vtx[23] = VertexTextureData{ Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f) };

	geometry->SetVertices(vtx);

	vector<uint32> idx(36);

	// �ո�
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// �޸�
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// ����
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// �Ʒ���
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// ���ʸ�
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// �����ʸ�
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	geometry->SetIndices(idx);
}

void GeometryHelper::CreateSphere(shared_ptr<Geometry<VertexTextureData>> geometry)
{
	float radius = 0.5f;
	uint32 stackCount = 20; 
	uint32 sliceCount = 20; 

	vector<VertexTextureData> vtx;

	VertexTextureData v;

	// �ϱ�
	v.position = Vec3(0.f, radius, 0.f);
	v.uv = Vec2(0.5f, 0.f);
	vtx.push_back(v);



	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);


	
	for (uint32 y = 1; y <= stackCount - 1; y++)
	{
		float phi = y * stackAngle;

		
		for (uint32 x = 0; x <= sliceCount; x++)
		{
			float theta = x * sliceAngle;

			v.position.X = radius * sinf(phi) * cosf(theta);
			v.position.Z = radius * sinf(phi) * sinf(theta);
			v.position.Y = radius * cosf(phi);

			v.uv = Vec2(deltaU * x, deltaV * y);
			
			vtx.push_back(v);
		}
	}


	// ����
	v.position = Vec3(0.f, -radius, 0.f);
	v.uv = Vec2(0.5f, 1.f);
	vtx.push_back(v);

	geometry->SetVertices(vtx);



	//vector<uint32> idx(36);
	vector<uint32> idx;
	idx.reserve(2 * sliceCount * (stackCount - 1));

	// �ϱ� �ε���
	for (uint32 i = 0; i < sliceCount; i++)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// ���� �ε���
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; y++)
	{
		for (uint32 x = 0; x < sliceCount; x++)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1)*ringVertexCount + (x));


			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// ���� �ε���
	uint32 bottomIndex = static_cast<uint32>(vtx.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; i++)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	geometry->SetIndices(idx);
}

void GeometryHelper::CreateGrid(shared_ptr<Geometry<VertexTextureData>> geometry, int32 sizeX, int32 sizeZ)
{
	
	vector<VertexTextureData> vtx;
	for (int32 z = 0; z <= sizeZ; z++)
	{
		for(int32 x = 0; x <= sizeX; x++)
		{
			VertexTextureData v;
			v.position = Vec3(static_cast<float>(x), 0, static_cast<float>(z));
			v.uv = Vec2(static_cast<float>(x), static_cast<float>(sizeZ - z));
			
			vtx.push_back(v);
		}
	}

	geometry->SetVertices(vtx);

	vector<uint32> idx;

	int32 horizonVertexCount = sizeX + 1;

	for (int32 z = 0; z < sizeZ; z++)
	{
		for (int32 x = 0; x < sizeX; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			idx.push_back(horizonVertexCount * (z + 1) + (x));
			idx.push_back(horizonVertexCount * (z) + (x + 1));
			idx.push_back(horizonVertexCount * (z) + (x));


			//  [1] - [2]
			//   	\  |
			//		  [0]
			idx.push_back(horizonVertexCount * (z) + (x + 1));
			idx.push_back(horizonVertexCount * (z + 1) + (x));
			idx.push_back(horizonVertexCount * (z + 1) + (x + 1));
		}
	}

	geometry->SetIndices(idx);
	
}






































/*------------------------------------
		VertexTextureNormalData
------------------------------------*/
void GeometryHelper::CreateQuad(shared_ptr<Geometry<VertexTextureNormalData>> geometry)
{
	vector<VertexTextureNormalData> vtx;
	vtx.resize(4);


	vtx[0].position = Vec3(-0.5f, -0.5f, 0.f);
	vtx[0].uv = Vec2(0, 1.f);
	vtx[0].normal = Vec3(0.f, 0.f, -1.f);

	vtx[1].position = Vec3(-0.5f, 0.5f, 0.f);
	vtx[1].uv = Vec2(0, 0);
	vtx[1].normal = Vec3(0.f, 0.f, -1.f);

	vtx[2].position = Vec3(0.5f, -0.5f, 0.f);
	vtx[2].uv = Vec2(1.f, 1.f);
	vtx[2].normal = Vec3(0.f, 0.f, -1.f);

	vtx[3].position = Vec3(0.5f, 0.5f, 0.f);
	vtx[3].uv = Vec2(1.f, 0.f);
	vtx[3].normal = Vec3(0.f, 0.f, -1.f);

	geometry->SetVertices(vtx);

	vector<uint32> idx = { 0, 1, 2, 2, 1, 3 };
	geometry->SetIndices(idx);
}

void GeometryHelper::CreateCube(shared_ptr<Geometry<VertexTextureNormalData>> geometry)
{
	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<VertexTextureNormalData> vtx(24);

	// �ո�
	vtx[0] = VertexTextureNormalData{ Vec3(-w2, -h2, -d2), Vec2(0.f, 1.f), Vec3(0.0f, 0.0f, -1.0f) };
	vtx[1] = VertexTextureNormalData{ Vec3(-w2, +h2, -d2), Vec2(0.f, 0.f), Vec3(0.0f, 0.0f, -1.0f) };
	vtx[2] = VertexTextureNormalData{ Vec3(+w2, +h2, -d2), Vec2(1.f, 0.f), Vec3(0.0f, 0.0f, -1.0f) };
	vtx[3] = VertexTextureNormalData{ Vec3(+w2, -h2, -d2), Vec2(1.f, 1.f), Vec3(0.0f, 0.0f, -1.0f) };

	// �޸�
	vtx[4] = VertexTextureNormalData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f) };
	vtx[5] = VertexTextureNormalData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f) };
	vtx[6] = VertexTextureNormalData{ Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f) };
	vtx[7] = VertexTextureNormalData{ Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f) };

	// ����
	vtx[8] = VertexTextureNormalData{ Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f) };
	vtx[9] = VertexTextureNormalData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f) };
	vtx[10] = VertexTextureNormalData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f) };
	vtx[11] = VertexTextureNormalData{ Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f) };

	// �Ʒ���
	vtx[12] = VertexTextureNormalData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f) };
	vtx[13] = VertexTextureNormalData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f) };
	vtx[14] = VertexTextureNormalData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f) };
	vtx[15] = VertexTextureNormalData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f) };
	// ���ʸ�
	vtx[16] = VertexTextureNormalData{ Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f) };
	vtx[17] = VertexTextureNormalData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f) };
	vtx[18] = VertexTextureNormalData{ Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f) };
	vtx[19] = VertexTextureNormalData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f) };
	// �����ʸ�
	vtx[20] = VertexTextureNormalData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f) };
	vtx[21] = VertexTextureNormalData{ Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f) };
	vtx[22] = VertexTextureNormalData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f) };
	vtx[23] = VertexTextureNormalData{ Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f) };

	geometry->SetVertices(vtx);

	vector<uint32> idx(36);

	// �ո�
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// �޸�
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// ����
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// �Ʒ���
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// ���ʸ�
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// �����ʸ�
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	geometry->SetIndices(idx);
}

void GeometryHelper::CreateSphere(shared_ptr<Geometry<VertexTextureNormalData>> geometry)
{
	float radius = 0.5f;
	uint32 stackCount = 20; // ���� ����
	uint32 sliceCount = 20; // ���� ����

	vector<VertexTextureNormalData> vtx;

	VertexTextureNormalData v;

	// �ϱ�
	v.position = Vec3(0.f, radius, 0.f);
	v.uv = Vec2(0.5f, 0.f);
	v.normal = v.position;
	v.normal.Normalize();
	vtx.push_back(v);



	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);


	// ������� ���鼭 ������ ����Ѵ� (�ϱ�, ������ �������̹Ƿ� ����)
	for (uint32 y = 1; y <= stackCount - 1; y++)
	{
		float phi = y * stackAngle;

		// ����� ��ġ�� ����
		for (uint32 x = 0; x <= sliceCount; x++)
		{
			float theta = x * sliceAngle;

			v.position.X = radius * sinf(phi) * cosf(theta);
			v.position.Z = radius * sinf(phi) * sinf(theta);
			v.position.Y = radius * cosf(phi);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.position;
			v.normal.Normalize();

			vtx.push_back(v);
		}
	}


	// ����
	v.position = Vec3(0.f, -radius, 0.f);
	v.uv = Vec2(0.5f, 1.f);
	v.normal = v.position;
	v.normal.Normalize();

	vtx.push_back(v);

	geometry->SetVertices(vtx);



	//vector<uint32> idx(36);
	vector<uint32> idx;
	idx.reserve(2 * sliceCount * (stackCount - 1));

	// �ϱ� �ε���
	for (uint32 i = 0; i < sliceCount; i++)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// ���� �ε���
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; y++)
	{
		for (uint32 x = 0; x < sliceCount; x++)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));


			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// ���� �ε���
	uint32 bottomIndex = static_cast<uint32>(vtx.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; i++)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	geometry->SetIndices(idx);
}

void GeometryHelper::CreateGrid(shared_ptr<Geometry<VertexTextureNormalData>> geometry, int32 sizeX, int32 sizeZ)
{

	vector<VertexTextureNormalData> vtx;
	for (int32 z = 0; z <= sizeZ; z++)
	{
		for (int32 x = 0; x <= sizeX; x++)
		{
			VertexTextureNormalData v;
			v.position = Vec3(static_cast<float>(x), 0, static_cast<float>(z));
			v.uv = Vec2(static_cast<float>(x), static_cast<float>(sizeZ - z));
			v.normal = Vec3(0.f, 1.f, 0.f);

			vtx.push_back(v);
		}
	}

	geometry->SetVertices(vtx);

	vector<uint32> idx;

	int32 horizonVertexCount = sizeX + 1;

	for (int32 z = 0; z < sizeZ; z++)
	{
		for (int32 x = 0; x < sizeX; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			idx.push_back(horizonVertexCount * (z + 1) + (x));
			idx.push_back(horizonVertexCount * (z)+(x + 1));
			idx.push_back(horizonVertexCount * (z)+(x));


			//  [1] - [2]
			//   	\  |
			//		  [0]
			idx.push_back(horizonVertexCount * (z)+(x + 1));
			idx.push_back(horizonVertexCount * (z + 1) + (x));
			idx.push_back(horizonVertexCount * (z + 1) + (x + 1));
		}
	}

	geometry->SetIndices(idx);

}
































/*-------------------------------------
 	 VertexTextureNormalTangentData
--------------------------------------*/
void GeometryHelper::CreateQuad(shared_ptr<Geometry<VertexTextureNormalTangentData>> geometry)
{
	vector<VertexTextureNormalTangentData> vtx;
	vtx.resize(4);


	vtx[0].position = Vec3(-0.5f, -0.5f, 0.f);
	vtx[0].uv = Vec2(0, 1.f);
	vtx[0].normal = Vec3(0.f, 0.f, -1.f);
	vtx[0].tangent = Vec3(1.f, 0.f, 0.f);

	vtx[1].position = Vec3(-0.5f, 0.5f, 0.f);
	vtx[1].uv = Vec2(0, 0);
	vtx[1].normal = Vec3(0.f, 0.f, -1.f);
	vtx[1].tangent = Vec3(1.f, 0.f, 0.f);

	vtx[2].position = Vec3(0.5f, -0.5f, 0.f);
	vtx[2].uv = Vec2(1.f, 1.f);
	vtx[2].normal = Vec3(0.f, 0.f, -1.f);
	vtx[3].tangent = Vec3(1.f, 0.f, 0.f);

	vtx[3].position = Vec3(0.5f, 0.5f, 0.f);
	vtx[3].uv = Vec2(1.f, 0.f);
	vtx[3].normal = Vec3(0.f, 0.f, -1.f);
	vtx[3].tangent = Vec3(1.f, 0.f, 0.f);

	geometry->SetVertices(vtx);

	vector<uint32> idx = { 0, 1, 2, 2, 1, 3 };
	geometry->SetIndices(idx);
}

void GeometryHelper::CreateCube(shared_ptr<Geometry<VertexTextureNormalTangentData>> geometry)
{
	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<VertexTextureNormalTangentData> vtx(24);

	// �ո�
	vtx[0] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, -d2), Vec2(0.f, 1.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[1] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, -d2), Vec2(0.f, 0.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[2] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, -d2), Vec2(1.f, 0.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[3] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, -d2), Vec2(1.f, 1.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };

	// �޸�
	vtx[4] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.0f, 0.0f, 0.0f) };
	vtx[5] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[6] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[7] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.f, 0.f, 0.f) };

	// ����
	vtx[8] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[9] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[10] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[11] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };

	// �Ʒ���
	vtx[12] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[13] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[14] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[15] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	// ���ʸ�
	vtx[16] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	vtx[17] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	vtx[18] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	vtx[19] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	// �����ʸ�
	vtx[20] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };
	vtx[21] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };
	vtx[22] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };
	vtx[23] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };

	geometry->SetVertices(vtx);

	vector<uint32> idx(36);

	// �ո�
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// �޸�
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// ����
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// �Ʒ���
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// ���ʸ�
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// �����ʸ�
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	geometry->SetIndices(idx);
}

void GeometryHelper::CreateCube(shared_ptr<Geometry<VertexTextureNormalTangentData>> geometry, 
	float sizeX, float sizeY, float sizeZ)
{
	float w2 = sizeX / 2.f;
	float h2 = sizeY / 2.f;
	float d2 = sizeZ / 2.f;

	vector<VertexTextureNormalTangentData> vtx(24);

	// �ո�
	vtx[0] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, -d2), Vec2(0.f, 1.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[1] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, -d2), Vec2(0.f, 0.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[2] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, -d2), Vec2(1.f, 0.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[3] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, -d2), Vec2(1.f, 1.f), Vec3(0.0f, 0.0f, -1.0f),
	Vec3(1.f, 0.f, 0.f) };

	// �޸�
	vtx[4] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.0f, 0.0f, 0.0f) };
	vtx[5] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[6] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[7] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f),
	Vec3(-1.f, 0.f, 0.f) };

	// ����
	vtx[8] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[9] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[10] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };
	vtx[11] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f),
	Vec3(1.f, 0.f, 0.f) };

	// �Ʒ���
	vtx[12] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[13] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[14] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	vtx[15] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f),
	Vec3(-1.f, 0.f, 0.f) };
	// ���ʸ�
	vtx[16] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	vtx[17] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	vtx[18] = VertexTextureNormalTangentData{ Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	vtx[19] = VertexTextureNormalTangentData{ Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f),
	Vec3(0.f, 0.f, -1.f) };
	// �����ʸ�
	vtx[20] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };
	vtx[21] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };
	vtx[22] = VertexTextureNormalTangentData{ Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };
	vtx[23] = VertexTextureNormalTangentData{ Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f),
	Vec3(0.0f, 0.0f, 1.0f) };

	geometry->SetVertices(vtx);

	vector<uint32> idx(36);

	// �ո�
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// �޸�
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// ����
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// �Ʒ���
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// ���ʸ�
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// �����ʸ�
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	geometry->SetIndices(idx);
}

void GeometryHelper::CreateSphere(shared_ptr<Geometry<VertexTextureNormalTangentData>> geometry)
{
	float radius = 0.5f;
	uint32 stackCount = 20; // ���� ����
	uint32 sliceCount = 20; // ���� ����

	vector<VertexTextureNormalTangentData> vtx;

	VertexTextureNormalTangentData v;

	// �ϱ�
	v.position = Vec3(0.f, radius, 0.f);
	v.uv = Vec2(0.5f, 0.f);
	v.normal = v.position;
	v.normal.Normalize();
	v.tangent = Vec3(1.f, 0.f, 0.f);
	v.tangent.Normalize();
	vtx.push_back(v);



	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);


	
	for (uint32 y = 1; y <= stackCount - 1; y++)
	{
		float phi = y * stackAngle;

		// ����� ��ġ�� ����
		for (uint32 x = 0; x <= sliceCount; x++)
		{
			float theta = x * sliceAngle;

			v.position.X = radius * sinf(phi) * cosf(theta);
			v.position.Z = radius * sinf(phi) * sinf(theta);
			v.position.Y = radius * cosf(phi);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.position;
			v.normal.Normalize();

			v.tangent.X = -radius * sinf(phi) * sinf(theta);
			v.tangent.Y = 0.0f;
			v.tangent.Z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();



			vtx.push_back(v);
		}
	}


	// ����
	v.position = Vec3(0.f, -radius, 0.f);
	v.uv = Vec2(0.5f, 1.f);
	v.normal = v.position;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);
	v.tangent.Normalize();

	vtx.push_back(v);

	geometry->SetVertices(vtx);



	//vector<uint32> idx(36);
	vector<uint32> idx;
	idx.reserve(2 * sliceCount * (stackCount - 1));

	
	for (uint32 i = 0; i < sliceCount; i++)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// ���� �ε���
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; y++)
	{
		for (uint32 x = 0; x < sliceCount; x++)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));


			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// ���� �ε���
	uint32 bottomIndex = static_cast<uint32>(vtx.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; i++)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	geometry->SetIndices(idx);
}

void GeometryHelper::CreateGrid(shared_ptr<Geometry<VertexTextureNormalTangentData>> geometry, int32 sizeX, int32 sizeZ)
{

	vector<VertexTextureNormalTangentData> vtx;
	for (int32 z = 0; z <= sizeZ; z++)
	{
		for (int32 x = 0; x <= sizeX; x++)
		{
			VertexTextureNormalTangentData v;
			v.position = Vec3(static_cast<float>(x), 0, static_cast<float>(z));
			v.uv = Vec2(static_cast<float>(x), static_cast<float>(sizeZ - z));
			v.normal = Vec3(0.f, 1.f, 0.f);
			v.tangent = Vec3(1.f, 0.f, 0.f);

			vtx.push_back(v);
		}
	}

	geometry->SetVertices(vtx);

	vector<uint32> idx;

	int32 horizonVertexCount = sizeX + 1;

	for (int32 z = 0; z < sizeZ; z++)
	{
		for (int32 x = 0; x < sizeX; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			idx.push_back(horizonVertexCount * (z + 1) + (x));
			idx.push_back(horizonVertexCount * (z)+(x + 1));
			idx.push_back(horizonVertexCount * (z)+(x));


			//  [1] - [2]
			//   	\  |
			//		  [0]
			idx.push_back(horizonVertexCount * (z)+(x + 1));
			idx.push_back(horizonVertexCount * (z + 1) + (x));
			idx.push_back(horizonVertexCount * (z + 1) + (x + 1));
		}
	}

	geometry->SetIndices(idx);

}

void GeometryHelper::CreateCylinder(shared_ptr<Geometry<VertexTextureNormalTangentData>> geometry)
{
	/* input */
	float height = 1.f;
	float radius = 0.5f;

	int vertSliceCount = 20;
	int sliceCount = 20;
	/* input end */
	

	vector<VertexTextureNormalTangentData> vtx;
	VertexTextureNormalTangentData v;

	float verticalInterval = height / (vertSliceCount - 1);
	float sliceAngle = XM_2PI / sliceCount;

	float sumHeight = -height * 0.5f;


	// ����
	v.position = myVec3(0.f, sumHeight, 0.f);
	v.normal = v.position.GetNormalize();
	vtx.push_back(v);

	// ����
	for (int i = 0; i < vertSliceCount - 1; i++)
	{
		for (int j = 0; j <= sliceCount; j++)
		{
			float theta = j * sliceAngle;

			v.position.X = radius * cosf(theta);
			v.position.Z = radius * sinf(theta);
			v.position.Y = sumHeight;

			v.normal = (v.position - Vec3(0.f, sumHeight, 0.f)).GetNormalize();

			vtx.push_back(v);
		}

		sumHeight += verticalInterval;
	}


	// �ϱ�
	v.position = myVec3(0.f, sumHeight, 0.f);
	v.normal = v.position.GetNormalize();
	vtx.push_back(v);

	geometry->SetVertices(vtx);


	vector<uint32> idx;
	

	// ���� �ε���
	for (int i = 0; i < sliceCount; i++)
	{
		/*	
			[i+1][i+2]
			|    /
			[0]
		*/

		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// ���� �ε���
	int ringVertexCount = sliceCount + 1;
	for (int y = 0; y < vertSliceCount - 2; y++)
	{
		for (int x = 0; x < sliceCount; x++)
		{
			/*	
				[y+1, x]  [y+1, x+1]
				  |     /
				[y, x]
			*/

			idx.push_back(1 + (y) * ringVertexCount + (x));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));


			/*
						[y+1, x+1]
					 /		|
				[y, x]	  [y, x+1]
			*/
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y)*ringVertexCount + (x));
		}
	}


	// �ϱ� �ε���
	int topIndex = vtx.size() - 1;
	int lastRingStartIndex = topIndex - ringVertexCount;
	for (int i = 0; i < sliceCount; i++)
	{
		/*
			 [top]
			  |      \
			[last+i]  [last+i+1]  
		*/
		idx.push_back(topIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	geometry->SetIndices(idx);
}
