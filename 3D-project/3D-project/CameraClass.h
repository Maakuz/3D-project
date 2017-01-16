#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include <DirectXMath.h>
#include <d3d11.h>

//http://www.toymaker.info/Games/html/camera.html
//https://books.google.se/books?id=GY-AAwAAQBAJ&pg=PA279&lpg=PA279&dq=DirectX::XMMATRIX+yawMatrix+%3D+DirectX::XMMatrixRotationAxis()&source=bl&ots=q9ZMKlgRDw&sig=ntfQbxD_FG2SzkIqrnMKVmrwvuM&hl=sv&sa=X&ved=0ahUKEwi4nceHs7_RAhXJiiwKHdeaAEAQ6AEIODAC#v=onepage&q=DirectX%3A%3AXMMATRIX%20yawMatrix%20%3D%20DirectX%3A%3AXMMatrixRotationAxis()&f=false

class CameraClass
{
private: 
	// Keyboard
	// Mouse

	const float defaultRotationRate;
	const float defaultMovementRate;
	const float defaultMouseSensitivity;

	const float defaultFovAngleY;
	const float defaultAspectRatio;
	const float defaultZNear;
	const float defaultZFar;

	const float FovAngleY;
	const float aspectRatio;
	const float zNear;	//near plane distance
	const float zFar;

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
	CameraClass(float FovAngleY, float aspectRatio, float zNear, float zFar,
		DirectX::XMVECTOR position, DirectX::XMVECTOR up,
		DirectX::XMVECTOR look,
		DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix);

	virtual ~CameraClass();
	void viewProjectionMatrix(); //kan va bra att ha

	void setPosition(FLOAT X, FLOAT Y, FLOAT Z);
	void setPosition(DirectX::XMVECTOR position);
	void updateViewMatrix(); //view matris uppdateras ofta, prjectionmatrix bara en gång
	void updateProjectionMatrix();
	void resetAll();				//litet frågetecken
	void initiate();
	void rotation(DirectX::XMMATRIX transform); //genomför inte en ändring i position
	void rotation(DirectX::XMFLOAT4X4 &transform); //om man skickar in en float4x4 istället

	//en update som hanterar WASD och mouse movement
	//initiate ska också sätta mMouse och mKeyboard
};








#endif