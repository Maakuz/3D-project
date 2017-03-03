#ifndef FRUSTRUMCULLING_H
#define FRUSTRUMCULLING_H

#include "CameraClass.h"

class FrustrumCulling
{
private:
	float nearPlaneDistance;
	float nearPlaneWidth;
	float nearPlaneHeight;

	float farPlaneDistance;
	float farPlaneWidth;
	float farPlaneHeight;

	float fovAngleY;
	float aspectRatio;

	DirectX::XMFLOAT3 cameraPosition;
	DirectX::XMFLOAT3 cameraDirection; //normaliserad
	DirectX::XMFLOAT3 mRight;	//normalizerad	//ska var iaf på alla
	DirectX::XMFLOAT3 mUp;	//normaliserad

	//points
	DirectX::XMFLOAT4 closeUpLeft;
	DirectX::XMFLOAT4 closeUpRight;
	DirectX::XMFLOAT4 closeDownRight;
	DirectX::XMFLOAT4 closeDownLeft;

	DirectX::XMFLOAT4 farUpLeft;
	DirectX::XMFLOAT4 farUpRight;
	DirectX::XMFLOAT4 farDownRight;
	DirectX::XMFLOAT4 farDownLeft;

public:
	FrustrumCulling(CameraClass camera);
	virtual ~FrustrumCulling();
	
	void makePoints();
};








#endif
