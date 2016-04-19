#include "platform.h"
#include "tmath.h"


//-----------------------------------------------------------------------------
// Name: _Vector2
// Desc: 
//-----------------------------------------------------------------------------
struct Vector2 _Vector2( float x, float y )
{
	struct Vector2 ret;

	ret.x = x;
	ret.y = y;

	return ret;
}

//-----------------------------------------------------------------------------
// Name: _Vector3
// Desc: 
//-----------------------------------------------------------------------------
struct Vector3 _Vector3( float x, float y, float z )
{
	struct Vector3 ret;

	ret.x = x;
	ret.y = y;
	ret.z = z;

	return ret;
}

//-----------------------------------------------------------------------------
// Name: _Vector4
// Desc: 
//-----------------------------------------------------------------------------
struct Vector4 _Vector4( float x, float y, float z, float w )
{
	struct Vector4 ret;

	ret.x = x;
	ret.y = y;
	ret.z = z;
	ret.w = w;

	return ret;
}

//-----------------------------------------------------------------------------
// Name: distance2d
// Desc: Returns the distance between two 2D points.
//-----------------------------------------------------------------------------
float distance2d( struct Vector2* v1, struct Vector2* v2 )
{
	float d = 0.0f;
	float x = v2->x - v1->x;
	float y = v2->y - v1->y;

	x *= x;
	y *= y;

	d = (float) sqrt(x+y);

	return d;
}

//-----------------------------------------------------------------------------
// Name: distance3d
// Desc: Returns the distance between 2 3D points.
//-----------------------------------------------------------------------------
float distance3d( struct Vector3* v1, struct Vector3* v2 )
{
	float d = 0.0f;
	float x = v2->x - v1->x;
	float y = v2->y - v1->y;
	float z = v2->z - v1->z;

	x *= x;
	y *= y;
	z *= z;

	d = (float) sqrt(x+y+z);

	return d;
}

//-----------------------------------------------------------------------------
// Name: dp3
// Desc: Performs a dot product 3 operation on two vectors.
//-----------------------------------------------------------------------------
float dp3( struct Vector3* v1, struct Vector3* v2 )
{
	return (v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z);
}

//-----------------------------------------------------------------------------
// Name: dp4
// Desc: Performs a dot product 4 operation on two points.
//-----------------------------------------------------------------------------
float dp4( struct Vector4* v1, struct Vector4* v2 )
{
	return (v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z)+(v1->w*v2->w);
}

//-----------------------------------------------------------------------------
// Name: dph
// Desc: Performs a dot product homogeneous operation on two points.
//-----------------------------------------------------------------------------
float dph( struct Vector4* v1, struct Vector4* v2 )
{
	return (v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z)+v2->w;
}

//-----------------------------------------------------------------------------
// Name: cross
// Desc: Calculate's the cross product of a triangle.  Used to calculate face
//		 normals.
//-----------------------------------------------------------------------------
void cross( struct Vector3* out, struct Vector3* a, struct Vector3* b, struct Vector3* c )
{
	struct Vector3 v1, v2;

	v1.x = b->x - a->x;
	v1.y = b->y - a->y;
	v1.z = b->z - a->z;

	v2.x = c->x - a->x;
	v2.y = c->y - a->y;
	v2.z = c->z - a->z;

	out->x = ( v1.y * v2.z ) - ( v1.z * v2.y );
	out->y = -( v2.z * v1.x ) - ( v2.x * v1.z );
	out->z = ( v1.x * v2.y ) - ( v1.y * v2.x );
}

//-----------------------------------------------------------------------------
// Name: magnitude
// Desc: 
//-----------------------------------------------------------------------------
float magnitude( struct Vector3* v )
{
	return (float) sqrt( v->x * v->x + v->y * v->y + v->z * v->z );
}

//-----------------------------------------------------------------------------
// Name: normalize
// Desc: Normalizes a vector.
//-----------------------------------------------------------------------------
void normalize( struct Vector3* v )
{
	float m = magnitude( v );

	if( m != 0.0f )
	{
		v->x /= m;
		v->y /= m;
		v->z /= m;
	}
}

//-----------------------------------------------------------------------------
// Name: rotate_vector
// Desc: Rotates a vector with the supplied matrix
//-----------------------------------------------------------------------------
void rotate_vector( float* m, struct Vector3* v, struct Vector3* d )
{
	d->x = ( m[0] * v->x ) + ( m[4] * v->y ) + ( m[8] * v->z );
	d->y = ( m[1] * v->x ) + ( m[5] * v->y ) + ( m[9] * v->z );
	d->z = ( m[2] * v->x ) + ( m[6] * v->y ) + ( m[10] * v->z );
}

//-----------------------------------------------------------------------------
// Name: shadow_matrix
// Desc: Creates a shadow matrix from the light's position and the ground plane.
//-----------------------------------------------------------------------------
void shadow_matrix( float mtx[4][4], struct Vector4* ground_plane, struct Vector4* light_pos )
{
	// Get the dot product between the light position and the ground plane normal
	float dot_product = dp4( ground_plane, light_pos );

	// Create the shadow matrix
	mtx[0][0] = dot_product - light_pos->x * ground_plane->x;
	mtx[1][0] = 0.0f - light_pos->x * ground_plane->y;
	mtx[2][0] = 0.0f - light_pos->x * ground_plane->z;
	mtx[3][0] = 0.0f - light_pos->x * ground_plane->w;

	mtx[0][1] = 0.0f - light_pos->y * ground_plane->x;
	mtx[1][1] = dot_product - light_pos->y * ground_plane->y;
	mtx[2][1] = 0.0f - light_pos->y * ground_plane->z;
	mtx[3][1] = 0.0f - light_pos->y * ground_plane->w;

	mtx[0][2] = 0.0f - light_pos->z * ground_plane->x;
	mtx[1][2] = 0.0f - light_pos->z * ground_plane->y;
	mtx[2][2] = dot_product - light_pos->z * ground_plane->z;
	mtx[3][2] = 0.0f - light_pos->z * ground_plane->w;

	mtx[0][3] = 0.0f - light_pos->w * ground_plane->x;
	mtx[1][3] = 0.0f - light_pos->w * ground_plane->y;
	mtx[2][3] = 0.0f - light_pos->w * ground_plane->z;
	mtx[3][3] = dot_product - light_pos->w * ground_plane->w;
}

//-----------------------------------------------------------------------------
// Name: project_vertex
// Desc: Takes a point in 3D space and returns a 2D screen coordinate. This func-
//		 ion is useful when drawing 2D shapes over 3D objects, picking, etc.
//		 Keep in mind that this code does not work for orthographic projections
//		 and assumes the depth range is 0.0-1.0f.  Also, if win_coord.z's value is 
//		 greater than 1.0, then the object is behind the camera.
//-----------------------------------------------------------------------------
int project_vertex( const struct Vector3* obj, float* modelview, float* projection, int* viewport, struct Vector3* win_coord )
{
	// Transformation vectors
	struct Vector4 tv1, tv2;

	// Modelview transformation
	tv1.x = modelview[0]*obj->x+modelview[4]*obj->y+modelview[8]*obj->z+modelview[12];
	tv1.y = modelview[1]*obj->x+modelview[5]*obj->y+modelview[9]*obj->z+modelview[13];
	tv1.z = modelview[2]*obj->x+modelview[6]*obj->y+modelview[10]*obj->z+modelview[14];
	tv1.w = modelview[3]*obj->x+modelview[7]*obj->y+modelview[11]*obj->z+modelview[15];

	// Projection transformation
	tv2.x = projection[0]*tv1.x+projection[4]*tv1.y+projection[8]*tv1.z+projection[12]*tv1.w;
	tv2.y = projection[1]*tv1.x+projection[5]*tv1.y+projection[9]*tv1.z+projection[13]*tv1.w;
	tv2.z = projection[2]*tv1.x+projection[6]*tv1.y+projection[10]*tv1.z+projection[14]*tv1.w;
	tv2.w = -tv1.z;

	// Normalize result between -1 and 1
	if( tv2.w == 0.0f )
		return 0;

	tv2.w = 1.0f / tv2.w;

	// Perspective division
	tv2.x *= tv2.w;
	tv2.y *= tv2.w;
	tv2.z *= tv2.w;

	// Calculate window coordinates
	win_coord->x = (tv2.x*0.5f+0.5f)*viewport[2]+viewport[0];
	win_coord->y = (tv2.y*0.5f+0.5f)*viewport[3]+viewport[1];
	win_coord->z = (1.0f+tv2.z)*0.5f;

	return 1;
}
