//
// Cylinder.cpp
// Copyright (c) Charles Baker. All rights reserved.
//

#include "stdafx.hpp"
#include "Cylinder.hpp"
#include "Grid.hpp"
#include "Value.hpp"
#include <math/vec2.ipp>
#include <math/vec3.ipp>
#include <math/mat4x4.ipp>
#include "assert.hpp"
#include <algorithm>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

using std::min;
using std::max;
using std::list;
using std::vector;
using std::shared_ptr;
using namespace math;
using namespace reyes;

Cylinder::Cylinder( float radius, float zmin, float zmax, float thetamax )
: Geometry(vec2(0.0f, 1.0f), vec2(0.0f, 1.0f)),
  radius_( radius ),
  zmin_( zmin ),
  zmax_( zmax ),
  thetamax_( thetamax )
{   
}

Cylinder::Cylinder( const Cylinder& cylinder, const math::vec2& u_range, const math::vec2& v_range )
: Geometry(u_range, v_range),
  radius_( cylinder.radius_ ),
  zmin_( cylinder.zmin_ ),
  zmax_( cylinder.zmax_ ),
  thetamax_( cylinder.thetamax_ )
{   
}

bool Cylinder::boundable() const
{
    return true;
}

void Cylinder::bound( const math::mat4x4& transform, vec3* minimum, vec3* maximum ) const
{
    REYES_ASSERT( minimum );
    REYES_ASSERT( maximum );
    
    *minimum = vec3( FLT_MAX, FLT_MAX, FLT_MAX );
    *maximum = vec3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    
    Grid grid;
    dice( transform, 8, 8, &grid );
    const vec3* positions = grid["P"].vec3_values();
    const vec3* positions_end = positions + grid["P"].size();
    for ( const vec3* i = positions; i != positions_end; ++i )
    {
        minimum->x = min( minimum->x, i->x );
        minimum->y = min( minimum->y, i->y );
        minimum->z = min( minimum->z, i->z );        
        maximum->x = max( maximum->x, i->x );
        maximum->y = max( maximum->y, i->y );
        maximum->z = max( maximum->z, i->z );
    }
}

bool Cylinder::splittable() const
{
    return true;
}

void Cylinder::split( std::list<std::shared_ptr<Geometry> >* primitives ) const
{
    REYES_ASSERT( primitives );
    REYES_ASSERT( u_range().y >= u_range().x );
    REYES_ASSERT( v_range().y >= v_range().x );

    const vec2& u_range = Geometry::u_range();
    float u0 = u_range.x;
    float u1 = (u_range.x + u_range.y) / 2.0f;
    float u2 = u_range.y;

    const vec2& v_range = Geometry::v_range();
    float v0 = v_range.x;
    float v1 = (v_range.x + v_range.y) / 2.0f;
    float v2 = v_range.y;

    shared_ptr<Geometry> cylinder;
    cylinder.reset( new Cylinder(*this, vec2(u0, u1), vec2(v0, v1)) );
    primitives->push_back( cylinder );
    cylinder.reset( new Cylinder(*this, vec2(u0, u1), vec2(v1, v2)) );
    primitives->push_back( cylinder );
    cylinder.reset( new Cylinder(*this, vec2(u1, u2), vec2(v0, v1)) );
    primitives->push_back( cylinder );
    cylinder.reset( new Cylinder(*this, vec2(u1, u2), vec2(v1, v2)) );
    primitives->push_back( cylinder );
}

bool Cylinder::diceable() const
{
    return true;
}

void Cylinder::dice( const math::mat4x4& transform, int width, int height, Grid* grid ) const
{
    REYES_ASSERT( width > 0 );
    REYES_ASSERT( height > 0 );
    REYES_ASSERT( grid );
    
    const vec2& u_range = Geometry::u_range();
    const vec2& v_range = Geometry::v_range();

    grid->resize( width, height );
    grid->du_ = (u_range.y - u_range.x) / float(width - 1);
    grid->dv_ = (v_range.y - v_range.x) / float(height - 1);
    
    vec3* positions = grid->value( "P", TYPE_POINT ).vec3_values();
    float* s = grid->value( "s", TYPE_FLOAT ).float_values();
    float* t = grid->value( "t", TYPE_FLOAT ).float_values();
    
    int vertex = 0;
    float v = v_range.x;
    float dv = (v_range.y - v_range.x) / float(height - 1);
    for ( int j = 0; j < height; ++j )
    {
        float u = u_range.x;
        float du = (u_range.y - u_range.x) / float(width - 1);
        for ( int i = 0; i < width; ++i )
        {
            positions[vertex] = vec3( transform * vec4(position(u, v), 1.0f) );
            s[vertex] = u;
            t[vertex] = v;
            u = min( u + du, u_range.y );
            ++vertex;
        }        
        v = min( v + dv, v_range.y );
    }    
}

math::vec3 Cylinder::position( float u, float v ) const
{
    vec3 n = normal( u, v );
    return vec3( radius_ * n.x, radius_ * n.y, v * (zmax_ - zmin_) );
}

math::vec3 Cylinder::normal( float u, float /*v*/ ) const
{    
    float theta = u * thetamax_;
    return vec3( cosf(theta), sinf(theta), 0.0f );
}