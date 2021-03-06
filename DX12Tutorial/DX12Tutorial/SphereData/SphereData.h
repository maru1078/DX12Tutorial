#ifndef SPHERE_DATA_H_
#define SPHERE_DATA_H_

#include <DirectXMath.h>

using namespace DirectX;

// 球体情報を保持しておくための構造体
struct SphereData
{
public:

	SphereData(const XMFLOAT3& position, const XMFLOAT4& color, float radius);

public:

	XMMATRIX World() const;

public:

	XMFLOAT3 position;
	float radius;
	XMFLOAT4 color;
};

#endif // !SPHERE_DATA_H_

