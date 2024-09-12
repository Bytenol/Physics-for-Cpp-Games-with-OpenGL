#ifndef __BYTENOL_PCGA_VECTOR_H__
#define __BYTENOL_PCGA_VECTOR_H__

#include <cmath>


namespace phy {

    struct Vector2
    {
        float x = 0.0f;
        float y = 0.0f;

        Vector2 operator+(const Vector2& v) const;
        Vector2& operator+=(const Vector2& v);

        Vector2 operator-(const Vector2& v) const;
        Vector2& operator-=(const Vector2& v);

        Vector2 operator*(const float& s) const;
        Vector2& operator*=(const float& s);

        Vector2 negate() const;
        Vector2& normalize();

        float getLength() const;

        float dotProduct(const Vector2& v) const;

        float angleBetween(const Vector2& v) const;
    };

    inline Vector2 Vector2::operator+(const Vector2& v) const 
    {
        return { x + v.x, y + v.y };
    }

    inline Vector2 &Vector2::operator+=(const Vector2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    inline Vector2 Vector2::operator-(const Vector2& v) const 
    {
        return { x - v.x, y - v.y };
    }

    inline Vector2 &Vector2::operator-=(const Vector2 &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline Vector2 Vector2::operator*(const float &s) const
    {
        return { x * s, y * s };
    }

    inline Vector2 &Vector2::operator*=(const float &s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    
    inline Vector2 Vector2::negate() const
    {
        return { -x, -y };
    }

    inline Vector2& Vector2::normalize()
    {
        auto l = getLength();
        if(l == 0.0f) {
            x = 0.0f;
            y = 0.0f;
            return *this;
        }
        x /= l;
        y /= l;
        return *this;
    }

    inline float Vector2::getLength() const
    {
        return std::hypot(x, y);
    }

    inline float Vector2::dotProduct(const Vector2 &v) const
    {
        return x * v.x + y * v.y;
    }

    inline float Vector2::angleBetween(const Vector2 &v) const
    {
        auto ab = dotProduct(v);
        return std::acos(ab / (getLength() * v.getLength()));
    }
}

#endif 