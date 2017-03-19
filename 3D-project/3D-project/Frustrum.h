#pragma once
#include "Definitions.h"
#include <DirectXCollision.h>

class Frustrum
{
public:

	class mPlane
	{
	public:
		DirectX::XMFLOAT4 point;
		mPlane(DirectX::XMFLOAT4 point) { this->point = point; };
		mPlane() {};
		~mPlane() {};
		void normalize() {
			float length = 0.0f;
			length = sqrt(this->point.x * this->point.x + this->point.y * this->point.y + this->point.z * this->point.z);
			if (length < 0.00001f)
			{
				this->point.x /= length;
				this->point.y /= length;
				this->point.z /= length;
				this->point.w /= length;
			}
		}
	};

	
	Frustrum();

	~Frustrum();
	bool AABBVsFrustrum(AABB box) const;
	bool AABBVsFrustrum(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 half) const;
	bool boxVsPlane(DirectX::XMFLOAT4 plane, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 half) const;
	void constructFrustrum(DirectX::XMFLOAT4X4 projMatrix, DirectX::XMFLOAT4X4 viewMatrix);

private:
	mPlane planes[6];
};
