// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class GAMES101_API Triangle2 
{
	
public:
	Triangle2();
	~Triangle2();

public:
	TArray<FVector> v; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
	/*Per vertex values*/
	FVector color[3]; //color at each vertex;
	FVector2D tex_coords[3]; //texture u,v
	FVector normal[3]; //normal vector for each vertex

	void setVertex(int ind, FVector ver); /*set i-th vertex coordinates */
	void setNormal(int ind, FVector n); /*set i-th vertex normal vector*/
	void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/
	FVector getColor() const { return color[0] * 255; } // Only one color per triangle.
	void setTexCoord(int ind, float s, float t); /*set i-th vertex texture coordinate*/
	TArray<FVector4> toVector4() const;
};
