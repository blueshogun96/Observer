#pragma once

#include <math.h>

//-----------------------------------------------------------------------------
// 2D vector
//-----------------------------------------------------------------------------
struct Vector2
{
	float x, y;
};

//-----------------------------------------------------------------------------
// 3D vector
//-----------------------------------------------------------------------------
struct Vector3
{
	float x, y, z;
};


//-----------------------------------------------------------------------------
// 4D vector
//-----------------------------------------------------------------------------
struct Vector4
{
	float x, y, z, w;
};

//-----------------------------------------------------------------------------
// 4x4 Matrix
//-----------------------------------------------------------------------------
struct Matrix4x4
{
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
};


//-----------------------------------------------------------------------------
// Interesting math functions
//-----------------------------------------------------------------------------
struct Vector2 _Vector2( float x, float y );
struct Vector3 _Vector3( float x, float y, float z );
struct Vector4 _Vector4( float x, float y, float z, float w );

float distance2d( struct Vector2* v1, struct Vector2* v2 );
float distance3d( struct Vector3* v1, struct Vector3* v2 );

float dp3( struct Vector3* v1, struct Vector3* v2 );
float dp4( struct Vector4* v1, struct Vector4* v2 );
float dph( struct Vector4* v1, struct Vector4* v2 );

void cross( struct Vector3* out, struct Vector3* a, struct Vector3* b, struct Vector3* c );
float magnitude( struct Vector3* v );
void normalize( struct Vector3* v );

void rotate_vector( float* m, struct Vector3* v, struct Vector3* d );
void shadow_matrix( float mtx[4][4], struct Vector4* ground_plane, struct Vector4* light_pos );
int project_vertex( const struct Vector3* obj, float* modelview, float* projection, int* viewport, struct Vector3* win_coord );
//void inverse_matrix