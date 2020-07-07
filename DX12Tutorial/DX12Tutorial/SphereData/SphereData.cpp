#include "SphereData.h"

SphereData::SphereData(const XMFLOAT3 & position, const XMFLOAT4 & color, float radius)
	: position{ position }
	, color{ color }
	, radius{ radius }
{
}

XMMATRIX SphereData::World() const
{
	return XMMatrixTranslation(position.x, position.y, position.z);
}
