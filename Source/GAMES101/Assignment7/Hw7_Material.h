// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hw7_Global.h"

// 材质类型
enum  EMaterialType { DIFFUSE };

// 材质
class Material {
public:
	EMaterialType m_type;
	//FVector m_color;
	FVector m_emission;
	float ior;
	FVector Kd, Ks;
	float specularExponent;
	//Texture tex;

	Material(EMaterialType t=EMaterialType::DIFFUSE, FVector e=FVector::ZeroVector);
	

public:

	inline EMaterialType getType() { return m_type; }
	//inline FVector getColor() { return m_color; }
	inline FVector getColorAt(double u, double v) { return FVector(); }
	FVector getEmission() {return m_emission;}
	bool hasEmission() {
		if (m_emission.Size() > EPSILON) 
			return true;
		else 
			return false;
	}

	 // sample a ray by Material properties
	// 按照该材质的性质，给定入射方向与法向量，用某种分布采样一个出射方向
	FVector sample(const FVector& wi, const FVector& N);

	// given a ray, calculate the PdF of this ray 
	// 给定一对入射、出射方向与法向量，计算sample 方法得到该出射方向的概率密度
	float pdf(const FVector& wi, const FVector& wo, const FVector& N);
	
	// given a ray, calculate the contribution of this ray
	// 给定一对入射、出射方向与法向量，计算这种情况下的f_r 值
	FVector eval(const FVector& wi, const FVector& wo, const FVector& N);

	FVector reflect(const FVector& I, const FVector& N); //反射
	FVector refract(const FVector& I, const FVector& N); //折射
	float fresnel(const FVector& I, const FVector& N); //菲涅尔
	FVector toWorld(const FVector& a, const FVector& N);
};