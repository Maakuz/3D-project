#include "FrustrumCulling.h"

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
}

FrustrumCulling::~FrustrumCulling()
{

}

void FrustrumCulling::makePoints()
{
	DirectX::XMFLOAT4 nearCenter;
	nearCenter.x = this->cameraPosition.x;
	nearCenter.y = this->cameraPosition.y;
	nearCenter.z = this->cameraPosition.z + (this->cameraDirection.z * this->nearPlaneDistance);
	nearCenter.w = 1;

	this->closeUpLeft = nearCenter;
	this->closeUpLeft.y = this->closeUpLeft.y - (this->mUp.y * (this->nearPlaneHeight / 2));
	this->closeUpLeft.x = this->closeUpLeft.x - (this->mRight.x * (this->nearPlaneWidth / 2));

	this->closeUpRight = this->closeUpLeft;
	this->closeUpRight.x = this->closeUpRight.x + (this->mRight.x * this->nearPlaneWidth);

	this->closeDownRight = this->closeUpRight;
	this->closeDownRight.y = closeDownRight.y + (this->mUp.y * this->nearPlaneHeight);

	this->closeDownLeft = this->closeDownRight;
	this->closeDownLeft.x = this->closeDownLeft.x - (this->mRight.x * this->nearPlaneWidth);



	DirectX::XMFLOAT4 farCenter;
	farCenter.x = this->cameraPosition.x;
	farCenter.y = this->cameraPosition.y;
	farCenter.z = this->cameraPosition.z + (this->cameraDirection.z * this->farPlaneDistance);
	farCenter.w = 1;

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