#pragma once
#include "Definitions.h"

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
			if (length < 0.0001f)
			{
				this->point.x /= length;
				this->point.y /= length;
				this->point.z /= length;
			}
		}
	};

	mPlane planes[6];
	Frustrum();

	~Frustrum();
	bool AABBVsFrustrum(AABB box) const;
	void constructFrustrum(DirectX::XMFLOAT4X4 projMatrix, DirectX::XMFLOAT4X4 viewMatrix);

private:

};
