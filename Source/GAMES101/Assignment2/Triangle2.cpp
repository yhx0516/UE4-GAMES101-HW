// Fill out your copyright notice in the Description page of Project Settings.


#include "Triangle2.h"

Triangle2::Triangle2()
{
	v.Add(FVector::ZeroVector);
	v.Add(FVector::ZeroVector);
	v.Add(FVector::ZeroVector);

	color[0] = color[1] = color[2] = FVector::ZeroVector;
	tex_coords[0] = tex_coords[1] = tex_coords[2] = FVector2D::ZeroVector;

}

Triangle2::~Triangle2()
{
}

void Triangle2::setVertex(int ind, FVector ver) {
	v[ind] = ver;
}
void Triangle2::setNormal(int ind, FVector n) {
	normal[ind] = n;
}
void Triangle2::setColor(int ind, float r, float g, float b) {
	if ((r < 0.0) || (r > 255.) ||
		(g < 0.0) || (g > 255.) ||
		(b < 0.0) || (b > 255.)) {
		fprintf(stderr, "ERROR! Invalid color values");
		fflush(stderr);
		exit(-1);
	}

	color[ind] = FVector((float)r / 255., (float)g / 255., (float)b / 255.);
	return;
}
void Triangle2::setTexCoord(int ind, float s, float t) {
	tex_coords[ind] = FVector2D(s, t);
}

TArray<FVector4> Triangle2::toVector4() const
{
	TArray<FVector4> res;
	for(int i = 0; i < 3; i++)
	{
		res.Add(FVector4(v[i].X, v[i].Y, v[i].Z, 1.0f));
	}
	return res;
}