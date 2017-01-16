#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include "Definitions.h"


//http://www.toymaker.info/Games/html/camera.html
//https://books.google.se/books?id=GY-AAwAAQBAJ&pg=PA279&lpg=PA279&dq=DirectX::XMMATRIX+yawMatrix+%3D+DirectX::XMMatrixRotationAxis()&source=bl&ots=q9ZMKlgRDw&sig=ntfQbxD_FG2SzkIqrnMKVmrwvuM&hl=sv&sa=X&ved=0ahUKEwi4nceHs7_RAhXJiiwKHdeaAEAQ6AEIODAC#v=onepage&q=DirectX%3A%3AXMMATRIX%20yawMatrix%20%3D%20DirectX%3A%3AXMMatrixRotationAxis()&f=false

class CameraClass
{
private: 
	// Keyboard
	// Mouse

	matrixStruct matrices;
	ID3D11Device* gDevice;

	float defaultRotationRate;
	float defaultMovementRate;
	float defaultMouseSensitivity;

	float fovAngleY;
	float aspectRatio;
	float zNear;
	float zFar;

	DirectX::XMFLOAT3 standardPosition; //standard värden //används inte än
	DirectX::XMFLOAT3 standardUp;		
	DirectX::XMFLOAT3 standardLook;		
	DirectX::XMFLOAT3 standardRight;	

	//man sparar vectorer och matriser som floats pga performance
	// då kan man använda "SIMD stands for "single instruction, multiple data""

	DirectX::XMFLOAT3 mPosition; //positin va kamera
	DirectX::XMFLOAT3 mUp;		//up pekar upp y //XMFLOAT3 dessa ändras hela tiden
	DirectX::XMFLOAT3 mLook;	//look pekar ner z
	DirectX::XMFLOAT3 mRight;	//right pekar ner x			//cross produkt av de andra 2
	DirectX::XMFLOAT4X4 mViewMatrix;		//sparad viewmatrix
	DirectX::XMFLOAT4X4 mProjectionMatrix;	//sparad projectionmatris


	//DirectX::XMMATRIX view;
	//float pitch; //rotation RUNT x/right
	//float yaw;	//rotation RUNT y/up
	//float roll;	//rotation RUNT z/look
public:
	CameraClass(ID3D11Device* gDevice);

	virtual ~CameraClass();
	DirectX::XMMATRIX viewProjectionMatrix(); //kan va bra att ha

	void setPosition(float x, float y, float z);
	void setPosition(DirectX::XMVECTOR position);
	void updateViewMatrix(); //view matris uppdateras ofta, prjectionmatrix bara en gång
	void updateProjectionMatrix();
	void resetAll();				//litet frågetecken
	void initiate();

	void rotation(DirectX::XMFLOAT4X4 &transform); //genomför inte en ändring i position

	D3D11_SUBRESOURCE_DATA getMatricesSubresource();
	matrixStruct initiateMatrices();
	ID3D11Buffer* createConstantBuffer();

	//en update som hanterar WASD och mouse movement
	//initiate ska också sätta mMouse och mKeyboard
};








#endif