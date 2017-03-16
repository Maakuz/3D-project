#include "FrustrumCulling.h"

Plane::Plane(DirectX::XMVECTOR normalVector)
{ 
	DirectX::XMVector3Normalize(normalVector);
	DirectX::XMStoreFloat3(&this->normal, normalVector);
	//normalisera noprmalen? är den redan det?
	//normalen ska vara inåt
}

Plane::Plane()
{
	
}

Plane::~Plane()
{

}

void Plane::setPoints(DirectX::XMFLOAT3 firstPoint, DirectX::XMFLOAT3 secondPoint, DirectX::XMFLOAT3 lastPoint)
{
	// A, B, C hanteras i counterclockwise ordning. Är detta rätt betéende för directX?
	
	DirectX::XMFLOAT3 p0 = firstPoint;
	DirectX::XMFLOAT3 p1 = secondPoint;
	DirectX::XMFLOAT3 p2 = lastPoint;

	this->pOnPlane = p0;

	DirectX::XMFLOAT3 u;
	u.x = p1.x - p0.x;
	u.y = p1.y - p0.y;
	u.z = p1.z - p0.z;

	DirectX::XMFLOAT3 v;
	v.x = p2.x - p0.x;
	v.y = p2.y - p0.y;
	v.z = p2.z - p0.z;

	DirectX::XMVECTOR uVector = DirectX::XMLoadFloat3(&u);
	DirectX::XMVECTOR vVector = DirectX::XMLoadFloat3(&v);

	DirectX::XMVECTOR normalVector;
	normalVector = DirectX::XMVector3Cross(uVector, vVector);
	DirectX::XMVector3Normalize(normalVector);
	DirectX::XMStoreFloat3(&this->normal, normalVector);
}

float Plane::distance(DirectX::XMFLOAT3 pointToTry)
{
	DirectX::XMFLOAT3 A = DirectX::XMFLOAT3(this->normal.x, 0, 0);
	DirectX::XMFLOAT3 B = DirectX::XMFLOAT3(0, this->normal.y, 0);
	DirectX::XMFLOAT3 C = DirectX::XMFLOAT3(0, 0, this->normal.z);

	DirectX::XMVECTOR normalVector = DirectX::XMLoadFloat3(&this->normal);

	float D = ((this->normal.x * -1) * this->pOnPlane.x) + ((this->normal.y * -1) * this->pOnPlane.y) + ((this->normal.z * -1) * this->pOnPlane.z);  //en point i planet //normalen ska va negativ

	float normalDotPointToTry = ((this->normal.x * pointToTry.x) + (this->normal.y * pointToTry.y) + (this->normal.z * pointToTry.z));

	float distance = normalDotPointToTry + D; //om positiv så är på normalens sida, annars andra sidan.

	return distance;
}

DirectX::XMFLOAT3 Plane::getNormal()
{
	return this->normal;
}

FrustrumCulling::FrustrumCulling(CameraClass camera)
{
	this->cameraPosition = camera.getCameraPos();
	this->cameraDirection = camera.getMDirection();
	this->mRight = camera.getMRight();
	this->mUp = camera.getMUp();

	this->fovAngleY = camera.getFovAngleY();
	this->aspectRatio = camera.getAspectRatio();


	this->nearPlaneDistance = camera.getNearPlane();
	this->nearPlaneHeight = 2 * (tan(DirectX::XMConvertToRadians(this->fovAngleY) / 2) * this->nearPlaneDistance);
	this->nearPlaneWidth = this->nearPlaneHeight * this->aspectRatio;

	this->farPlaneDistance = camera.getFarPlane();
	this->farPlaneHeight = 2 * (tan(DirectX::XMConvertToRadians(this->fovAngleY) / 2) * this->farPlaneDistance);
	this->farPlaneWidth = this->farPlaneHeight * this->aspectRatio;	
}

FrustrumCulling::~FrustrumCulling()
{

}

void FrustrumCulling::makePoints()
{
	//near Plane
	DirectX::XMFLOAT3 nearCenter;
	nearCenter.x = this->cameraPosition.x;
	nearCenter.y = this->cameraPosition.y;
	nearCenter.z = this->cameraPosition.z + (this->cameraDirection.z * this->nearPlaneDistance);

	this->nearUpLeft = nearCenter;
	this->nearUpLeft.y = this->nearUpLeft.y - (this->mUp.y * (this->nearPlaneHeight / 2));
	this->nearUpLeft.x = this->nearUpLeft.x - (this->mRight.x * (this->nearPlaneWidth / 2));
		  
	this->nearUpRight = this->nearUpLeft;
	this->nearUpRight.x = this->nearUpRight.x + (this->mRight.x * this->nearPlaneWidth);
		  
	this->nearDownRight = this->nearUpRight;
	this->nearDownRight.y = nearDownRight.y + (this->mUp.y * this->nearPlaneHeight);
		  
	this->nearDownLeft = this->nearDownRight;
	this->nearDownLeft.x = this->nearDownLeft.x - (this->mRight.x * this->nearPlaneWidth);

	//far Plane
	DirectX::XMFLOAT3 farCenter;
	farCenter.x = this->cameraPosition.x;
	farCenter.y = this->cameraPosition.y;
	farCenter.z = this->cameraPosition.z + (this->cameraDirection.z * this->farPlaneDistance);

	this->farUpLeft = farCenter;
	this->farUpLeft.y = this->farUpLeft.y - (this->mUp.y * (this->farPlaneHeight / 2));
	this->farUpLeft.x = this->farUpLeft.x - (this->mRight.x * (this->farPlaneWidth / 2));

	this->farUpRight = this->farUpLeft;
	this->farUpRight.x = this->farUpRight.x + (this->mRight.x * this->farPlaneWidth);

	this->farDownRight = this->farUpRight;
	this->farDownRight.y = farDownRight.y + (this->mUp.y * this->farPlaneHeight);

	this->farDownLeft = this->farDownRight;
	this->farDownLeft.x = this->farDownLeft.x - (this->mRight.x * this->farPlaneWidth);
}

void FrustrumCulling::makePlanes()
{
	Plane topPlane;
	Plane botPlane;
	Plane leftPlane;
	Plane rightPlane;
	Plane nearPlane;
	Plane farPlane;

	topPlane.setPoints(this->nearUpRight, this->nearUpLeft, this->farUpLeft);
	botPlane.setPoints(this->nearDownLeft, this->nearDownRight, this->farDownRight);
	leftPlane.setPoints(this->nearUpLeft, this->nearDownLeft, this->farDownLeft);
	rightPlane.setPoints(this->nearDownRight, this->nearUpRight, this->farDownRight);
	nearPlane.setPoints(this->nearUpLeft, this->nearUpRight, this->nearDownRight);
	farPlane.setPoints(this->farUpRight, this->farUpLeft, this->farDownLeft);

	this->plane[0] = topPlane;
	this->plane[1] = botPlane;
	this->plane[2] = leftPlane;
	this->plane[3] = rightPlane;
	this->plane[4] = nearPlane;
	this->plane[5] = farPlane;
}

bool FrustrumCulling::comparePointToFrustrum(DirectX::XMFLOAT3 &p)
{
	bool inside = true;

	for (int i = 0; i < 6; i++)
	{
		if (this->plane[i].distance(p) < 0)
		{
			inside = false;
		}
	}
	return inside;

	//testa mot alla planes, alla planes måste vara inside, men om en är outside så är objektet helt utanför.

	//använda cirklar för stora objekt med extremt många polygoner. mindre accurate test men snabbare
}

bool FrustrumCulling::compareBoxToFrustrum(AABB box)
{
	bool inside = true;

	DirectX::XMFLOAT3 point[8];
	float distance[8];

	int min = 0;
	int max = 0;

	point[0] = box.p0;
	point[1] = box.p1;
	point[2] = DirectX::XMFLOAT3(box.p1.x, box.p1.y, box.p0.z);
	point[3] = DirectX::XMFLOAT3(box.p0.x, box.p1.y, box.p0.z);
	point[4] = DirectX::XMFLOAT3(box.p0.x, box.p1.y, box.p1.z);
	point[5] = DirectX::XMFLOAT3(box.p1.x, box.p0.y, box.p0.z);
	point[6] = DirectX::XMFLOAT3(box.p1.x, box.p0.y, box.p1.z);
	point[7] = DirectX::XMFLOAT3(box.p0.x, box.p0.y, box.p1.z);

	for (int i = 0; i < 6 && inside == true; i++) //planes
	{
		for (int j = 0; j < 8; j++)
		{
			DirectX::XMFLOAT3 temp;
			temp.x = point[j].x - plane[i].getNormal().x;
			temp.y = point[j].y - plane[i].getNormal().y;
			temp.z = point[j].z - plane[i].getNormal().z;
			float temp2 = sqrt(pow(temp.x, 2) + pow(temp.y, 2) + pow(temp.z, 2));
			distance[j] = temp2;
		}

		for (int k = 0; k < 8; k++)
		{
			if (distance[k] <= distance[min])
				min = k;
			if (distance[k] >= distance[max])
				max = k;
		}

		if (comparePointToFrustrum(point[min]) == true)
		{
			if (comparePointToFrustrum(point[max]) == true)
			{
				// all in, innanför frustrum
				inside = true;
			}
			else
			{
				inside = false; //någon utanför
			}
		}
		else
		{
			inside = true; //Same
		}
	}
	return inside;
}