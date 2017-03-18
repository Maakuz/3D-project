#include "Frustrum.h"


Frustrum::Frustrum()
{
}

Frustrum::~Frustrum()
{
}

bool Frustrum::AABBVsFrustrum(AABB box) const
{
	DirectX::XMFLOAT3 point[8];

	point[0] = box.p0;
	point[1] = DirectX::XMFLOAT3(box.p1.x, box.p0.y, box.p0.z);
	point[2] = DirectX::XMFLOAT3(box.p0.x, box.p0.y, box.p1.z);
	point[3] = DirectX::XMFLOAT3(box.p1.x, box.p0.y, box.p1.z);
	point[4] = DirectX::XMFLOAT3(box.p0.x, box.p1.y, box.p0.z);
	point[5] = DirectX::XMFLOAT3(box.p1.x, box.p1.y, box.p0.z);
	point[6] = DirectX::XMFLOAT3(box.p0.x, box.p1.y, box.p1.z);
	point[7] = box.p1;

	int inside = 0;
	for (size_t i = 0; i < 6; i++)
	{
		inside = 0;
		for (size_t j = 0; j < 8; j++)
		{
			//DirectX::XMFLOAT3 p2p = DirectX::XMFLOAT3(point[j].x - this->planes[i].point.x, point[j].y - this->planes[i].point.y, point[j].z - this->planes[i].point.z);
			float test = (point[i].x * this->planes[i].point.x + point[i].y * this->planes[i].point.y + point[i].z * this->planes[i].point.z);
			test += planes[i].point.w;
			if (test >= 0)
			{
				inside++;
			}
		}
		if (inside != 0)
		{
			return true;
		}
	}


	return false;
}

void Frustrum::constructFrustrum(DirectX::XMFLOAT4X4 projMatrix, DirectX::XMFLOAT4X4 viewMatrix)
{
	float zMin, r;
	DirectX::XMMATRIX temp;

	/*zMin = -projMatrix._43 / projMatrix._33;
	r = screenDepth / (screenDepth - zMin);
	projMatrix._33 = r;
	projMatrix._43 = -r * zMin;*/

	DirectX::XMMATRIX tempP = DirectX::XMLoadFloat4x4(&projMatrix);
	DirectX::XMMATRIX tempV = DirectX::XMLoadFloat4x4(&viewMatrix);

	temp = DirectX::XMMatrixMultiply(tempV, tempP);

	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, temp);

	//near
	/*planes[0].point.x = matrix._14 + matrix._13;
	planes[0].point.y = matrix._24 + matrix._23;
	planes[0].point.z = matrix._34 + matrix._33;
	planes[0].point.w = matrix._44 + matrix._43;
	planes[0].normalize();*/

	planes[0].point.x = matrix._13;
	planes[0].point.y = matrix._23;
	planes[0].point.z = matrix._33;
	planes[0].point.w = matrix._43;
	planes[0].normalize();

	 //far
	planes[1].point.x = matrix._14 - matrix._13;
	planes[1].point.y = matrix._24 - matrix._23;
	planes[1].point.z = matrix._34 - matrix._33;
	planes[1].point.w = matrix._44 - matrix._43;
	planes[1].normalize();

	//left
	planes[2].point.x = matrix._14 + matrix._11;
	planes[2].point.y = matrix._24 + matrix._21;
	planes[2].point.z = matrix._34 + matrix._31;
	planes[2].point.w = matrix._44 + matrix._41;
	planes[2].normalize();

	//right
	planes[3].point.x = matrix._14 - matrix._11;
	planes[3].point.y = matrix._24 - matrix._21;
	planes[3].point.z = matrix._34 - matrix._31;
	planes[3].point.w = matrix._44 - matrix._41;
	planes[3].normalize();


	//top
	planes[4].point.x = matrix._14 - matrix._12;
	planes[4].point.y = matrix._24 - matrix._22;
	planes[4].point.z = matrix._34 - matrix._32;
	planes[4].point.w = matrix._44 - matrix._42;
	planes[4].normalize();

	//bottom
	planes[5].point.x = matrix._14 + matrix._12;
	planes[5].point.y = matrix._24 + matrix._22;
	planes[5].point.z = matrix._34 + matrix._32;
	planes[5].point.w = matrix._44 + matrix._42;
	planes[5].normalize();



}
