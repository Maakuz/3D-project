#include "Frustrum.h"
#include <DirectXCollision.h>


Frustrum::Frustrum()
{
}

Frustrum::~Frustrum()
{

}

bool Frustrum::AABBVsFrustrum(AABB box) const
{

	const int nrOfPlanes = 6;
	bool test = true;

	DirectX::XMFLOAT3 center = DirectX::XMFLOAT3((box.p1.x + box.p0.x) / 2, (box.p1.y + box.p0.y) / 2, (box.p1.z + box.p0.z) / 2);
	DirectX::XMFLOAT3 half = DirectX::XMFLOAT3((box.p1.x - box.p0.x) / 2, (box.p1.y - box.p0.y) / 2, (box.p1.z - box.p0.z) / 2);

	int inside = 0;
	for (size_t i = 0; i < nrOfPlanes; i++)
	{
		if (this->boxVsPlane(this->planes[i].point, center, half))
		{
			inside++;
		}
	}
	if (inside != 6)
	{
		test = false;
	}
	return test;
}

bool Frustrum::AABBVsFrustrum(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 half) const
{
	const int nrOfPlanes = 6;
	bool test = true;

	int inside = 0;
	for (size_t i = 0; i < nrOfPlanes; i++)
	{
		if (this->boxVsPlane(this->planes[i].point, center, half))
		{
			inside++;
		}
	}
	if (inside != 6)
	{
		test = false;
	}
	return test;
}


bool Frustrum::boxVsPlane(DirectX::XMFLOAT4 plane, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 half) const
{
	bool test = true;

	float extent = half.x * abs(plane.x) + half.y * abs(plane.y) + half.z * abs(plane.z);
	float s = (center.x * plane.x + center.y * plane.y + center.z * plane.z) + plane.w;

	if ((s - extent) > 0)
	{
		test = false;
	}
	if (s + extent < 0)
	{
		test = true;
	}

	return test;
}


void Frustrum::constructFrustrum(DirectX::XMFLOAT4X4 projMatrix, DirectX::XMFLOAT4X4 viewMatrix)
{

	DirectX::XMMATRIX temp;

	DirectX::XMMATRIX tempP = DirectX::XMLoadFloat4x4(&projMatrix);
	DirectX::XMMATRIX tempV = DirectX::XMLoadFloat4x4(&viewMatrix);

	temp = DirectX::XMMatrixMultiply(tempV, tempP);
	

	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, temp);


	//left
	planes[0].point.x = matrix._14 + matrix._11;
	planes[0].point.y = matrix._24 + matrix._21;
	planes[0].point.z = matrix._34 + matrix._31;
	planes[0].point.w = matrix._44 + matrix._41;

	//right
	planes[1].point.x = matrix._14 - matrix._11;
	planes[1].point.y = matrix._24 - matrix._21;
	planes[1].point.z = matrix._34 - matrix._31;
	planes[1].point.w = matrix._44 - matrix._41;


	//top
	planes[2].point.x = matrix._14 - matrix._12;
	planes[2].point.y = matrix._24 - matrix._22;
	planes[2].point.z = matrix._34 - matrix._32;
	planes[2].point.w = matrix._44 - matrix._42;

	//bottom
	planes[3].point.x = matrix._14 + matrix._12;
	planes[3].point.y = matrix._24 + matrix._22;
	planes[3].point.z = matrix._34 + matrix._32;
	planes[3].point.w = matrix._44 + matrix._42;

	//near
	planes[4].point.x = matrix._14 + matrix._13;
	planes[4].point.y = matrix._24 + matrix._23;
	planes[4].point.z = matrix._34 + matrix._33;
	planes[4].point.w = matrix._44 + matrix._43;


	//far
	planes[5].point.x = matrix._14 - matrix._13;
	planes[5].point.y = matrix._24 - matrix._23;
	planes[5].point.z = matrix._34 - matrix._33;
	planes[5].point.w = matrix._44 - matrix._43;


	planes[0].normalize();
	planes[1].normalize();
	planes[2].normalize();
	planes[3].normalize();
	planes[4].normalize();
	planes[5].normalize();

}


