#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include "Definitions.h"
#include "Keyboard.h"
#include "Mouse.h"

class CameraClass
{
private: 
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	
	HWND window;
	int width;
	int height;

	matrixStruct matrices;
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	float defaultRotationRate;
	float defaultMovementRate;
	float defaultMouseSensitivity;
	DirectX::XMINT2 previousMouseLocation;

	float fovAngleY;
	float aspectRatio;
	float zNear;
	float zFar;


	//man sparar vectorer och matriser som floats pga performance
	DirectX::XMFLOAT3 mDirection; //Hållet man tittar
	DirectX::XMFLOAT3 mPosition; //position på kamera
	DirectX::XMFLOAT3 mUp;		//up pekar upp y //XMFLOAT3 dessa ändras hela tiden
	DirectX::XMFLOAT3 mLook;	//look pekar ner z	//direction
	DirectX::XMFLOAT3 mRight;	//right pekar ner x			//cross produkt av de andra 2
	DirectX::XMFLOAT4X4 mViewMatrix;		//sparad viewmatrix
	DirectX::XMFLOAT4X4 mProjectionMatrix;	//sparad projectionmatris

	float mPitch; //rotation RUNT x/right
	float mYaw;	//rotation RUNT y/up

	bool escapePressed;

	float rotationValue;
public:
	CameraClass(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, HWND window, int width, int height);

	virtual ~CameraClass();

	void setPosition(float x, float y, float z);
	void setPosition(DirectX::XMVECTOR position);
	void updateViewMatrix(); //view matris uppdateras ofta, prjectionmatrix bara en gång

	D3D11_SUBRESOURCE_DATA getMatricesSubresource();
	matrixStruct initiateMatrices(int width, int height);
	ID3D11Buffer* createConstantBuffer();
	void updateConstantBuffer(ID3D11Buffer* VSConstantBuffer);

	void update(float dt);

	matrixStruct getMatrix() const;
	float getNearPlane();
	float getFarPlane();
	float getFovAngleY();
	float getAspectRatio();
	DirectX::XMFLOAT3 getMPosition();
	DirectX::XMFLOAT3 getMDirection();
	DirectX::XMFLOAT3 getMRight();
	DirectX::XMFLOAT3 getMUp();
};

#endif