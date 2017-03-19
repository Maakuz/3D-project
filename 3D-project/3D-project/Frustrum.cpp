#include "Frustrum.h"
#include <DirectXCollision.h>


Frustrum::Frustrum()
{
	this->frustrum = new DirectX::BoundingFrustum();
}

Frustrum::~Frustrum()
{
	delete frustrum;
}

bool Frustrum::AABBVsFrustrum(AABB box) const
{
	//const int nrOfPoints = 8;
	const int nrOfPlanes = 6;
	//DirectX::XMFLOAT3 point[nrOfPoints];


	//point[0] = box.p0;
	//point[1] = DirectX::XMFLOAT3(box.p1.x, box.p0.y, box.p0.z);
	//point[2] = DirectX::XMFLOAT3(box.p0.x, box.p0.y, box.p1.z);
	//point[3] = DirectX::XMFLOAT3(box.p1.x, box.p0.y, box.p1.z);
	//point[4] = DirectX::XMFLOAT3(box.p0.x, box.p1.y, box.p0.z);
	//point[5] = DirectX::XMFLOAT3(box.p1.x, box.p1.y, box.p0.z);
	//point[6] = DirectX::XMFLOAT3(box.p0.x, box.p1.y, box.p1.z);
	//point[7] = box.p1;

	/*DirectX::XMFLOAT3 center = DirectX::XMFLOAT3((box.p1.x + box.p0.x) / 2, (box.p1.y + box.p0.y) / 2, (box.p1.z + box.p0.z) / 2);
	DirectX::XMFLOAT3 extents = DirectX::XMFLOAT3((box.p1.x - box.p0.x) / 2, (box.p1.y - box.p0.y) / 2, (box.p1.z - box.p0.z) / 2);*/
	//DirectX::BoundingBox *tempBox = new DirectX::BoundingBox(center, extents);
	//bool intersect = this->frustrum->Intersects(*tempBox);

	//return intersect;*/
	//
	//int inside = 0;
	//float test = 0.0f;
	//for (size_t i = 0; i < nrOfPoints; i++)
	//{
	//	inside = 0;
	//	for (size_t j = 0; j < nrOfPlanes; j++)
	//	{
	//		test = (point[i].x * planes[j].point.x + point[i].y * planes[j].point.y + point[i].z * planes[j].point.z) + planes[j].point.w;
	//		if (test >= 0)
	//		{
	//			inside++;
	//		}
	//		else
	//		{
	//			break;
	//		}
	//	}
	//	if (inside == nrOfPlanes)
	//	{
	//		return true;
	//	}
	//}
	//return false;

	DirectX::XMFLOAT3 center = DirectX::XMFLOAT3((box.p1.x + box.p0.x) / 2, (box.p1.y + box.p0.y) / 2, (box.p1.z + box.p0.z) / 2);
	DirectX::XMFLOAT3 half = DirectX::XMFLOAT3((box.p1.x - box.p0.x) / 2, (box.p1.y - box.p0.y) / 2, (box.p1.z - box.p0.z) / 2);

	for (size_t i = 0; i < nrOfPlanes; i++)
	{
		float extent = half.x * abs(planes[i].point.x) + half.y * abs(planes[i].point.y) + half.z * abs(planes[i].point.z);
		float s = (center.x * planes[i].point.x + center.y * planes[i].point.y + center.z * planes[i].point.z) + planes[i].point.w;

		if ((s - extent) > 0)
		{
			return false;
		}
		if (s + extent < 0)
		{
			return true;
		}
	}
	return true;

}

bool Frustrum::AABBVsFrustrum(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 half) const
{
	const int nrOfPlanes = 6;

	for (size_t i = 0; i < nrOfPlanes; i++)
	{
		float extent = half.x * abs(planes[i].point.x) + half.y * abs(planes[i].point.y) + half.z * abs(planes[i].point.z);
		float s = (center.x * planes[i].point.x + center.y * planes[i].point.y + center.z * planes[i].point.z) + planes[i].point.w;

		if ((s - extent) > 0)
		{
			return false;
		}
		if (s + extent < 0)
		{
			return true;
		}
	}
	return true;
}

bool Frustrum::PointVSFrustum(DirectX::XMFLOAT3 point) const
{
	//const int nrOfPlanes = 6;
	//int inside = 0;
	//float test = 0.0f;
	//for (size_t j = 0; j < nrOfPlanes; j++)
	//{
	//	test = (point.x * planes[j].point.x + point.y * planes[j].point.y + point.z * planes[j].point.z) + planes[j].point.w;
	//	if (test >= 0)
	//	{
	//		inside++;
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
	//if (inside == nrOfPlanes)
	//{
	//	return true;
	//}
	//return false;
	DirectX::BoundingSphere temp = DirectX::BoundingSphere(point, 1.0f);

	return this->frustrum->Intersects(temp);
}

void Frustrum::constructFrustrum(DirectX::XMFLOAT4X4 projMatrix, DirectX::XMFLOAT4X4 viewMatrix)
{

	DirectX::XMMATRIX temp;

	DirectX::XMMATRIX tempP = DirectX::XMLoadFloat4x4(&projMatrix);
	DirectX::XMMATRIX tempV = DirectX::XMLoadFloat4x4(&viewMatrix);

	

	

	temp = DirectX::XMMatrixMultiply(tempV, tempP);
	//DirectX::XMVECTOR determinant;
	//DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(&determinant, tempP);
	/*DirectX::XMVECTOR determinant;
	DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(&determinant, tempV);*/

	/*temp = DirectX::XMMatrixMultiply(inverse2,inverse);*/

	/*delete this->frustrum;
	this->frustrum = new DirectX::BoundingFrustum(temp);*/

	//this->frustrum->Transform(*this->frustrum, inverse);
	


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
	planes[4].point.x = matrix._13;
	planes[4].point.y = matrix._23;
	planes[4].point.z = matrix._33;
	planes[4].point.w = matrix._43;
	

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

void Frustrum::constructFrustrum(float screenDepth, DirectX::XMFLOAT4X4 projMatrix, DirectX::XMFLOAT4X4 viewMatrix)
{
	DirectX::XMMATRIX temp;
	float r, zMin;

	zMin = -projMatrix._43 / projMatrix._33;
	r = screenDepth / (screenDepth / -zMin);
	projMatrix._33 = r;
	projMatrix._43 = -r * zMin;

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
