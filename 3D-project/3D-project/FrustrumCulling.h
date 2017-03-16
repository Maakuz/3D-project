#ifndef FRUSTRUMCULLING_H
#define FRUSTRUMCULLING_H

#include "CameraClass.h"

class Plane
{
private:
	float d; //eh? gör inget nu
	DirectX::XMFLOAT3 normal;
public:
	Plane(float d, DirectX::XMVECTOR normal);
	Plane();
	~Plane();
	void setPoints(DirectX::XMFLOAT3 firstPoint, DirectX::XMFLOAT3 secondPoint, DirectX::XMFLOAT3 lastPoint);
};

class FrustrumCulling
{
private:
	Plane plane[6];

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
	DirectX::XMFLOAT3 nearUpLeft;
	DirectX::XMFLOAT3 nearUpRight;
	DirectX::XMFLOAT3 nearDownRight;
	DirectX::XMFLOAT3 nearDownLeft;

	DirectX::XMFLOAT3 farUpLeft;
	DirectX::XMFLOAT3 farUpRight;
	DirectX::XMFLOAT3 farDownRight;
	DirectX::XMFLOAT3 farDownLeft;

	//experementella saker enligt guide
	
	//kolla hur planet jämförs med punkterna och vad som definierar ett plan

public:
	FrustrumCulling(CameraClass camera);
	virtual ~FrustrumCulling();
	
	void makePoints();
	void makePlanes();
};








#endif
