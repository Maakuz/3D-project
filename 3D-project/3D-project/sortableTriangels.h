#pragma once

class sortableTriangels
{
public:
	sortableTriangels();
	~sortableTriangels();
	bool operator<(const sortableTriangels &other) const;
	int startVertex;
	float distance2Camera;
};
