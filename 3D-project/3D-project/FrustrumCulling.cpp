#include "FrustrumCulling.h"

Plane::Plane(float d, DirectX::XMVECTOR normalVector)
{
	this->d = d; 
	DirectX::XMVector3Normalize(normalVector);
	DirectX::XMStoreFloat3(&this->normal, normalVector);
	//normalisera noprmalen? �r den redan det?
	//normalen ska vara in�t
}

Plane::Plane()
{
	
}

Plane::~Plane()
{

}

void Plane::setPoints(DirectX::XMFLOAT3 firstPoint, DirectX::XMFLOAT3 secondPoint, DirectX::XMFLOAT3 lastPoint)
{
	// A, B, C hanteras i counterclockwise ordning. �r detta r�tt bet�ende f�r directX?
	
	DirectX::XMFLOAT3 p0 = firstPoint;
	DirectX::XMFLOAT3 p1 = secondPoint;
	DirectX::XMFLOAT3 p2 = lastPoint;

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

	DirectX::XMFLOAT3 A = DirectX::XMFLOAT3(this->normal.x, 0, 0);
	DirectX::XMFLOAT3 B = DirectX::XMFLOAT3(0, this->normal.y, 0);
	DirectX::XMFLOAT3 C = DirectX::XMFLOAT3(0, 0, this->normal.z);



	float temp = this->normal * p0;//en point i planet //normalen ska va negativ
}

FrustrumCulling::FrustrumCulling(CameraClass camera)
{
	this->cameraPosition = camera.getMPosition();
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

	//experementelle saker enligt guide
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