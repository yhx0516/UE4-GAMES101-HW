// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Triangle2.h"
#include "Kismet/KismetSystemLibrary.h"

namespace rst
{
	enum class Buffers
    {
        Color = 1,
        Depth = 2
    };

    inline Buffers operator|(Buffers a, Buffers b)
    {
        return Buffers((int)a | (int)b);
    }

    inline Buffers operator&(Buffers a, Buffers b)
    {
        return Buffers((int)a & (int)b);
    }

    enum class Primitive
    {
        Line,
        Triangle
    };

    /*
     * For the curious : The draw function takes two buffer id's as its arguments. These two structs
     * make sure that if you mix up with their orders, the compiler won't compile it.
     * Aka : Type safety
     * */
    struct pos_buf_id
    {
        int pos_id = 0;
    };

    struct ind_buf_id
    {
        int ind_id = 0;
    };

    struct col_buf_id
    {
        int col_id = 0;
    };

	class GAMES101_API Rasterizer2
	{	
	public:	
    
		Rasterizer2(int w, int h);
		pos_buf_id load_positions(const TArray<FVector>& positions);
		ind_buf_id load_indices(const TArray<FIntVector>& indices);
		col_buf_id load_colors(const TArray<FVector>& colors);

		void set_model(const FMatrix& m);
		void set_view(const FMatrix& v);
		void set_projection(const FMatrix& p);

		void set_pixel(const FVector& point, const FVector& color);

		void clear(Buffers buff);

		void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);

		TArray<FColor>& frame_buffer() { return frame_buf; }

        void SwitchMASS() { bMASS = !bMASS;}

	private:
		//void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);

		void rasterize_triangle(const Triangle2& t);

		// VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

	private:
		FMatrix model;
		FMatrix view;
		FMatrix projection;

		TMap<int,TArray<FVector>> pos_buf;
		TMap<int,TArray<FIntVector>> ind_buf;
		TMap<int,TArray<FVector>> col_buf;

		TArray<FColor> frame_buf;
		TArray<float> depth_buf;

		int get_index(int x, int y);

		int width, height;

		int next_id = 0;
		int get_next_id() { return next_id++; }

        bool bMASS = false;

	};
}