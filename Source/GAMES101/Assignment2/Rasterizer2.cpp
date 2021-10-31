// Fill out your copyright notice in the Description page of Project Settings.


#include "Rasterizer2.h"
#include <tuple>
#include <cmath>
#include "Kismet/KismetMathLibrary.h"
// Sets default values

rst::pos_buf_id rst::Rasterizer2::load_positions(const TArray<FVector>& positions)
{
	auto id = get_next_id();
	pos_buf.Emplace(id, positions);

	return { id };
}

rst::ind_buf_id rst::Rasterizer2::load_indices(const TArray<FIntVector>& indices)
{
	auto id = get_next_id();
	ind_buf.Emplace(id, indices);

	return { id };
}

rst::col_buf_id rst::Rasterizer2::load_colors(const TArray<FVector>& cols)
{
	auto id = get_next_id();
	col_buf.Emplace(id, cols);

	return { id };
}

auto to_vec4(const FVector& v3, float w = 1.0f)
{
	return FVector4(v3.X, v3.Y, v3.Z, w);
}


static bool insideTriangle(float x, float y, const TArray<FVector>& _v)
{
	// TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
	FVector2D point=FVector2D(x, y);

	FVector2D v0 = FVector2D(_v[0].X, _v[0].Y);
	FVector2D v1 = FVector2D(_v[1].X, _v[1].Y);
	FVector2D v2 = FVector2D(_v[2].X, _v[2].Y);

	FVector2D AB = v1 - v0;
	FVector2D BC = v2 - v1;
	FVector2D CA = v0 - v2;

	FVector2D AP = point - v0;
	FVector2D BP = point - v1;
	FVector2D CP = point - v2;

	
	return    UKismetMathLibrary::CrossProduct2D(AB, AP) > 0
		&& UKismetMathLibrary::CrossProduct2D(BC, BP) > 0
		&& UKismetMathLibrary::CrossProduct2D(CA, CP) > 0;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const TArray<FVector>& v)
{
	float c1 = (x * (v[1].Y - v[2].Y) + (v[2].X - v[1].X) * y + v[1].X * v[2].Y - v[2].X * v[1].Y) / (v[0].X * (v[1].Y - v[2].Y) + (v[2].X - v[1].X) * v[0].Y + v[1].X * v[2].Y - v[2].X * v[1].Y);
	float c2 = (x * (v[2].Y - v[0].Y) + (v[0].X - v[2].X) * y + v[2].X * v[0].Y - v[0].X * v[2].Y) / (v[1].X * (v[2].Y - v[0].Y) + (v[0].X - v[2].X) * v[1].Y + v[2].X * v[0].Y - v[0].X * v[2].Y);
	float c3 = (x * (v[0].Y - v[1].Y) + (v[1].X - v[0].X) * y + v[0].X * v[1].Y - v[1].X * v[0].Y) / (v[2].X * (v[0].Y - v[1].Y) + (v[1].X - v[0].X) * v[2].Y + v[0].X * v[1].Y - v[1].X * v[0].Y);
	return { c1,c2,c3 };
}

void rst::Rasterizer2::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
	auto& buf = pos_buf[pos_buffer.pos_id];
	auto& ind = ind_buf[ind_buffer.ind_id];
	auto& col = col_buf[col_buffer.col_id];

	float f1 = (50 - 0.1) / 2.0;
	float f2 = (50 + 0.1) / 2.0;

	FMatrix mvp = projection * view * model;
	for (auto& i : ind)
	{
		Triangle2 t;
		TArray<FVector4> v;
		v.Add(mvp.GetTransposed().TransformFVector4(to_vec4(buf[i[0]], 1.0f)));
		v.Add(mvp.GetTransposed().TransformFVector4(to_vec4(buf[i[1]], 1.0f)));
		v.Add(mvp.GetTransposed().TransformFVector4(to_vec4(buf[i[2]], 1.0f)));

		//Homogeneous division
		for (auto& vec : v) {
			vec *= 1 / vec.W;
		}
		//Viewport transformation
		for (auto& vert : v)
		{
			vert.X = 0.5 * width * (vert.X + 1.0);
			vert.Y = 0.5 * height * (vert.Y + 1.0);
			vert.Z = vert.Z * f1 + f2;
		}

		for (int j = 0; j < 3; ++j)
		{
			t.setVertex(j, UKismetMathLibrary::Conv_Vector4ToVector(v[j]));
			//t.setVertex(j, UKismetMathLibrary::Conv_Vector4ToVector(v[j]));
			//t.setVertex(j, UKismetMathLibrary::Conv_Vector4ToVector(v[j]));
		}

		auto col_x = col[i[0]];
		auto col_y = col[i[1]];
		auto col_z = col[i[2]];

		t.setColor(0, col_x[0], col_x[1], col_x[2]);
		t.setColor(1, col_y[0], col_y[1], col_y[2]);
		t.setColor(2, col_z[0], col_z[1], col_z[2]);

		rasterize_triangle(t);
	}
}

//Screen space rasterization
void rst::Rasterizer2::rasterize_triangle(const Triangle2& t) {

	// TODO : Find out the bounding box of current triangle.
	// iterate through the pixel and find if the current pixel is inside the triangle

	// If so, use the following code to get the interpolated z value.
	//auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
	//float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
	//float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
	//z_interpolated *= w_reciprocal;

	// TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
	
	auto v = t.toVector4();
	// 画出三角形所在边界
	// x_l = x_min ; x_r = x_max ; y_b = y_min ; y_t = y_max
	int x_l = std::floor(std::min(v[0].X, std::min(v[1].X, v[2].X)));   //floor向下取整
	int x_r = std::ceil(std::max(v[0].X, std::max(v[1].X, v[2].X)));    //ceil向上取整
	int y_b = std::floor(std::min(v[0].Y , std::min(v[1].Y, v[2].Y)));
	int y_t = std::ceil(std::max(v[0].Y, std::max(v[1].Y, v[2].Y)));

	if (bMASS) {
		// 四等分后中心点，如果像素间隔扩大，中心也相对变化
		TArray<FVector2D> posOffset = { {0.25,0.25},{0.25,0.75},{0.75,0.25},{0.75,0.75} };

		//由于过于清晰，可在此处将递增改为 5 或 10，相应四等分中心点也会变化
		for (int x = x_l; x <= x_r; x++)
			for (int y = y_b; y <= y_t; y++) {

				float minDepth = TNumericLimits<float>::Max();
				float percentage = 0;

				for (int i = 0; i < 4; i++) {
					if (insideTriangle((float)x + posOffset[i].X, (float)y + posOffset[i].Y, t.v)) {
						//重心坐标插值
						auto BarycentricParam = computeBarycentric2D((float)x + posOffset[i].X, (float)y + posOffset[i].Y, t.v);  
						float alpha = std::get<0>(BarycentricParam);
						float beta = std::get<1>(BarycentricParam);
						float gamma = std::get<2>(BarycentricParam);

						float w_reciprocal = 1.0f / (alpha / v[0].W + beta / v[1].W + gamma / v[2].W);
						float z_interpolated = alpha * v[0].Z / v[0].W + beta * v[1].Z / v[1].W + gamma * v[2].Z / v[2].W;
						z_interpolated *= w_reciprocal;
						minDepth = std::min(minDepth, z_interpolated);
						percentage += 0.25f;
					}
				}
				if (percentage > 0 &&  depth_buf[get_index(x, y)] > minDepth) {

					// 递增改变的话，应补充未被扫面的部分
					FVector color = t.getColor() * percentage;
					FVector point = FVector((float)x, (float)y, minDepth);
					depth_buf[get_index(x, y)] = minDepth;
					set_pixel(point, color);
				}

			}
	}
	else {
		for (int x = x_l; x <= x_r; x++)
			for (int y = y_b; y <= y_t; y++) {

				if (insideTriangle((float)x + 0.5, (float)y + 0.5, t.v)) {
					//重心坐标插值
					auto BarycentricParam = computeBarycentric2D((float)x + 0.5f, (float)y + 0.5f, t.v);  
					float alpha = std::get<0>(BarycentricParam);
					float beta = std::get<1>(BarycentricParam);
					float gamma = std::get<2>(BarycentricParam);

					float w_reciprocal = 1.0f / (alpha / v[0].W + beta / v[1].W + gamma / v[2].W);
					float z_interpolated = alpha * v[0].Z / v[0].W + beta * v[1].Z / v[1].W + gamma * v[2].Z / v[2].W;
					z_interpolated *= w_reciprocal;

					if (depth_buf[get_index(x, y)] > z_interpolated) {
						FVector color = t.getColor();
						FVector point = FVector((float)x, (float)y, z_interpolated);
						depth_buf[get_index(x, y)] = z_interpolated;
						set_pixel(point, color);
					}
				}
			}
	}
}

void rst::Rasterizer2::set_model(const FMatrix& m)
{
	model = m;
}

void rst::Rasterizer2::set_view(const FMatrix& v)
{
	view = v;
}

void rst::Rasterizer2::set_projection(const FMatrix& p)
{
	projection = p;
}

void rst::Rasterizer2::clear(rst::Buffers buff)
{
	if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
	{
		for (FColor& item : frame_buf) {
			item = FColor(0, 0, 0, 0);
		}
	}
	if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
	{
		for (float& item : depth_buf) {
			item = TNumericLimits<float>::Max();
		}
	}
}

rst::Rasterizer2::Rasterizer2(int w, int h) : width(w), height(h)
{
	frame_buf.SetNum(w * h);
	depth_buf.SetNum(w * h);
}

int rst::Rasterizer2::get_index(int x, int y)
{
	return (height - 1 - y) * width + x;
}

void rst::Rasterizer2::set_pixel(const FVector& point, const FVector& color)
{
	//old index: auto ind = point.y() + point.x() * width;
	auto ind = (height - 1 - point.Y) * width + point.X;
	frame_buf[ind] = FColor(color.X, color.Y, color.Z, 255);

}

