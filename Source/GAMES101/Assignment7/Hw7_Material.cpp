// Fill out your copyright notice in the Description page of Project Settings.


#include "Hw7_Material.h"
#include "Kismet/KismetMathLibrary.h"


Material::Material(EMaterialType t, FVector e)
{
	m_type = t;
	m_emission = e;
}

FVector Material::sample(const FVector& wi, const FVector& N)
{
	switch (m_type) {
	case EMaterialType::DIFFUSE:
		{
			// uniform sample on the hemisphere
			float x_1 = UKismetMathLibrary::RandomFloat(), x_2 = UKismetMathLibrary::RandomFloat();
			float z = std::fabs(1.0f - 2.0f * x_1);
			float r = std::sqrt(1.0f - z * z), phi = 2 * UKismetMathLibrary::GetPI() * x_2;
			FVector localRay(r * std::cos(phi), r * std::sin(phi), z);
			return toWorld(localRay, N);

			break;
		}
	}
	return FVector::ZeroVector;
}

FVector Material::toWorld(const FVector& a, const FVector& N)
{
	FVector B, C;
	if (std::fabs(N.X) > std::fabs(N.Y)) {
		float invLen = 1.0f / std::sqrt(N.X * N.X + N.Z * N.Z);
		C = FVector(N.Z * invLen, 0.0f, -N.X * invLen);
	}
	else {
		float invLen = 1.0f / std::sqrt(N.Y * N.Y + N.Z * N.Z);
		C = FVector(0.0f, N.Z * invLen, -N.Y * invLen);
	}
	B = FVector::CrossProduct(C, N);
	return a.X * B + a.Y * C + a.Z * N;
}

float Material::pdf(const FVector& wi, const FVector& wo, const FVector& N)
{
	switch (m_type) {
		case EMaterialType::DIFFUSE:
		{
			// uniform sample probability 1 / (2 * PI)
			if (FVector::DotProduct(wo, N) > 0.0f)
				return 0.5f / UKismetMathLibrary::GetPI();
			else
				return 0.0f;
			break;
		}
	}
	return 0.0f;
}

FVector Material::eval(const FVector& wi, const FVector& wo, const FVector& N)
{
	switch (m_type) {
		case EMaterialType::DIFFUSE:
		{
			// calculate the contribution of diffuse   model
			float cosalpha = FVector::DotProduct(N, wo);
			if (cosalpha > 0.0f) {
				FVector diffuse = Kd / UKismetMathLibrary::GetPI();
				return diffuse;
			}
			else
				return FVector(0.0f);
			break;
		}
	}
	return FVector(0.0f);
}

FVector Material::reflect(const FVector& I, const FVector& N)
{
	FVector res = I - 2 * FVector::DotProduct(I, N) * N;
	res.Normalize();
	return res;
}

FVector Material::refract(const FVector& I, const FVector& N)
{
	float cosi = UKismetMathLibrary::FClamp(FVector::DotProduct(I, N), -1, 1);
	float etai = 1, etat = ior;
	FVector n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	FVector res = k < 0 ? FVector::ZeroVector : eta * I + (eta * cosi - sqrtf(k)) * n;
	res.Normalize();
	return res;

}

float Material::fresnel(const FVector& I, const FVector& N)
{

	float cosi = UKismetMathLibrary::FClamp(FVector::DotProduct(I, N), -1, 1);
	float etai = 1, etat = ior;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		return 1;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		return (Rs * Rs + Rp * Rp) / 2.0;
	}
}


