#include "sortableTriangels.h"
#include "Definitions.h"
sortableTriangels::sortableTriangels()
{
	this->startVertex = 0;
	this->distance2Camera = INFINITY;
}

sortableTriangels::~sortableTriangels()
{
}
bool sortableTriangels::operator>(const sortableTriangels &other) const
{
	if (this->distance2Camera > other.distance2Camera)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool sortableTriangels::operator<(const sortableTriangels &other) const
{
	if (this->distance2Camera < other.distance2Camera)
	{
		return true;
	}
	else
	{
		return false;
	}
}