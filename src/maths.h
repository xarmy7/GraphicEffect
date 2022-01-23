#pragma once

#include "types.h"

#include <cmath>

namespace Math
{
    inline float Pi() { return 3.14159265359f; }
    inline float HalfPi() { return 0.5f * Math::Pi(); }
    inline float TwoPi() { return 2.f * Math::Pi(); }
    inline float ToRadians(float Degrees) { return Degrees * Math::Pi() / 180.f; }
    inline float ToDegrees(float Radians) { return Radians * 180.f / Math::Pi(); }
    inline float Cos(float V) { return std::cos(V); }
    inline float Sin(float V) { return std::sin(V); }
    inline float Tan(float V) { return std::tan(V); }
    inline float Atan(float V) { return std::atan(V); }
    inline float Atan2(float Y, float X) { return std::atan2(Y, X); }
    
    inline float Sqrt(float Value) { return std::sqrt(Value); }
    
    template<typename T>
    inline T Min(T A, T B) { return A < B ? A : B; }

    template<typename T>
    inline T Max(T A, T B) { return A > B ? A : B; }

    template<typename T>
    inline T Clamp(T X, T MinValue, T MaxValue) { return Min(Max(X, MinValue), MaxValue); }

    template<typename T>
    inline T Lerp(T X, T Y, float A) { return (1.f - A) * X + A * Y; }

    inline float Mod(float Value, float Base) { return fmod(Value, Base); }
    inline int Mod(int Value, int Base) { return Value % Base; }

    inline int TrueMod(int Value, int Base) { return (Value % Base + Base) % Base; }
    inline float TrueMod(float Value, float Base) { return Mod(Mod(Value, Base) + Base, Base); }
}

// ========================================================================
// VEC2 FUNCTIONS
// ========================================================================
inline v2 operator-(v2 A) { return { -A.x, -A.y }; }
inline v2 operator-(v2 A, v2 B) { return { A.x - B.x, A.y - B.y }; }
inline v2 operator/(v2 A, v2 B) { return { A.x / B.x, A.y / B.y }; }

// ========================================================================
// VEC3 FUNCTIONS
// ========================================================================
inline v3 operator+(v3 A) { return { A.x, A.y, A.z }; }
inline v3 operator+(v3 A, v3 B) { return { A.x + B.x, A.y + B.y, A.z + B.z }; }
inline v3& operator+=(v3& A, v3 B) { A = A + B; return A; }

inline v3 operator-(v3 A) { return { -A.x, -A.y, -A.z }; }
inline v3 operator-(v3 A, v3 B) { return { A.x - B.x, A.y - B.y, A.z - B.z }; }
inline v3& operator-=(v3& A, v3 B) { A = A - B; return A; }

inline v3 operator*(v3 V, float S) { return { V.x * S, V.y * S, V.z * S }; }
inline v3 operator*(float S, v3 V) { return V * S; }
inline v3& operator*=(v3& V, float A) { V = V * A; return V; }


inline v3 operator/(v3 V, float A)
{
    float Inverse = 1.f / A;
    return V * Inverse;
}

inline v3& operator/=(v3& V, float A) { V = V / A; return V; }

namespace Vec3
{
    inline v3 vec3(v2 xy, float z)
    {
        return v3 { xy.x, xy.y, z };
    }

    inline float Length(v3 V)
    {
        return Math::Sqrt(V.x * V.x + V.y * V.y + V.z * V.z);
    }

    inline v3 Normalize(v3 V)
    {
        float InvLen = 1.f / Vec3::Length(V);
        return V * InvLen;
    }

    inline float Dot(v3 A, v3 B)
    {
        return { A.x * B.x + A.y * B.y + A.z * B.z };
    }

    inline v3 Cross(v3 A, v3 B)
    {
        v3 R;
        R.x = A.y * B.z - A.z * B.y;
        R.y = A.z * B.x - A.x * B.z;
        R.z = A.x * B.y - A.y * B.x;
        return R;
    }
}

// ========================================================================
// VEC4 FUNCTIONS
// ========================================================================

inline v4 operator+(v4 A, v4 B) { return { A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w }; }
inline v4 operator-(v4 A, v4 B) { return { A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w }; }

inline v4 operator*(v4 V, float S) { return { V.x * S, V.y * S, V.z * S, V.w * S }; }
inline v4 operator*(float S, v4 V) { return V * S; }

inline v4 operator/(v4 V, float A)
{
    float Inverse = 1.f / A;
    return V * Inverse;
}

inline v4& operator/=(v4& V, float A) { V = V / A; return V; }

namespace Vec4
{
    inline v4 vec4(v3 xyz, float w)
    {
        return v4 { xyz.x, xyz.y, xyz.z, w };
    }
}

// ========================================================================
// MAT4 FUNCTIONS
// ========================================================================
inline v4 operator*(const mat4& M, v4 V)
{
    v4 R;
    R.x = V.x*M.c[0].e[0] + V.y*M.c[1].e[0] + V.z*M.c[2].e[0] + V.w*M.c[3].e[0];
    R.y = V.x*M.c[0].e[1] + V.y*M.c[1].e[1] + V.z*M.c[2].e[1] + V.w*M.c[3].e[1];
    R.z = V.x*M.c[0].e[2] + V.y*M.c[1].e[2] + V.z*M.c[2].e[2] + V.w*M.c[3].e[2];
    R.w = V.x*M.c[0].e[3] + V.y*M.c[1].e[3] + V.z*M.c[2].e[3] + V.w*M.c[3].e[3];
    return R;
}

inline mat4 operator*(const mat4& A, const mat4& B)
{
    mat4 Res = {};
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            for (int i = 0; i < 4; ++i)
                Res.c[c].e[r] += A.c[i].e[r] * B.c[c].e[i];
    return Res;
}

inline mat4& operator*=(mat4& A, const mat4& B)
{
    A = A * B;
    return A;
}

namespace Mat3
{
    inline mat3 Mat3(const mat4& Matrix4)
    {
        return {
            Matrix4.c[0].e[0], Matrix4.c[0].e[1], Matrix4.c[0].e[2],
            Matrix4.c[1].e[0], Matrix4.c[1].e[1], Matrix4.c[1].e[2],
            Matrix4.c[2].e[0], Matrix4.c[2].e[1], Matrix4.c[2].e[2],
        };
    }

    inline mat3 Transpose(mat3 M)
    {
        return {
            M.c[0].e[0], M.c[1].e[0], M.c[2].e[0],
            M.c[0].e[1], M.c[1].e[1], M.c[2].e[1],
            M.c[0].e[2], M.c[1].e[2], M.c[2].e[2],
        };
    }
}

namespace Mat4
{
    inline mat4 Identity()
    {
        return
        {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

    inline mat4 Translate(v3 T)
    {
        return
        {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            T.x, T.y, T.z, 1.f,
        };
    }

    inline mat4 Scale(v3 S)
    {
        return
        {
            S.x, 0.f, 0.f, 0.f,
            0.f, S.y, 0.f, 0.f,
            0.f, 0.f, S.z, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

    inline mat4 RotateX(float C, float S)
    {
        return
        {
            1.f, 0.f, 0.f, 0.f,
            0.f,   C,   S, 0.f,
            0.f,  -S,   C, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

    inline mat4 RotateX(float AngleRadians)
    {
        float C = Math::Cos(AngleRadians);
        float S = Math::Sin(AngleRadians);
        return Mat4::RotateX(C, S);
    }

    inline mat4 RotateY(float C, float S)
    {
        return
        {
              C, 0.f,   S, 0.f,
            0.f, 1.f, 0.f, 0.f,
             -S, 0.f,   C, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

    inline mat4 RotateY(float AngleRadians)
    {
        float C = Math::Cos(AngleRadians);
        float S = Math::Sin(AngleRadians);
        return Mat4::RotateY(C, S);
    }

    inline mat4 RotateZ(float C, float S)
    {
        return
        {
              C,   S, 0.f, 0.f,
             -S,   C, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

    inline mat4 RotateZ(float AngleRadians)
    {
        float C = Math::Cos(AngleRadians);
        float S = Math::Sin(AngleRadians);
        return Mat4::RotateZ(C, S);
    }
    
    inline mat4 Transpose(mat4 M)
    {
        return {
            M.c[0].e[0], M.c[1].e[0], M.c[2].e[0], M.c[3].e[0],
            M.c[0].e[1], M.c[1].e[1], M.c[2].e[1], M.c[3].e[1],
            M.c[0].e[2], M.c[1].e[2], M.c[2].e[2], M.c[3].e[2],
            M.c[0].e[3], M.c[1].e[3], M.c[2].e[3], M.c[3].e[3]
        };
    }
    
    inline mat4 Inverse(const mat4& M)
    {
        mat4 R;

        float S[6];
        S[0] = M.c[0].e[0] * M.c[1].e[1] - M.c[1].e[0] * M.c[0].e[1];
        S[1] = M.c[0].e[0] * M.c[1].e[2] - M.c[1].e[0] * M.c[0].e[2];
        S[2] = M.c[0].e[0] * M.c[1].e[3] - M.c[1].e[0] * M.c[0].e[3];
        S[3] = M.c[0].e[1] * M.c[1].e[2] - M.c[1].e[1] * M.c[0].e[2];
        S[4] = M.c[0].e[1] * M.c[1].e[3] - M.c[1].e[1] * M.c[0].e[3];
        S[5] = M.c[0].e[2] * M.c[1].e[3] - M.c[1].e[2] * M.c[0].e[3];

        float C[6];
        C[0] = M.c[2].e[0] * M.c[3].e[1] - M.c[3].e[0] * M.c[2].e[1];
        C[1] = M.c[2].e[0] * M.c[3].e[2] - M.c[3].e[0] * M.c[2].e[2];
        C[2] = M.c[2].e[0] * M.c[3].e[3] - M.c[3].e[0] * M.c[2].e[3];
        C[3] = M.c[2].e[1] * M.c[3].e[2] - M.c[3].e[1] * M.c[2].e[2];
        C[4] = M.c[2].e[1] * M.c[3].e[3] - M.c[3].e[1] * M.c[2].e[3];
        C[5] = M.c[2].e[2] * M.c[3].e[3] - M.c[3].e[2] * M.c[2].e[3];

        // Assuming it is invertible
        float InvDet = 1.0f / (S[0] * C[5] - S[1] * C[4] + S[2] * C[3] + S[3] * C[2] - S[4] * C[1] + S[5] * C[0]);

        R.c[0].e[0] = +(M.c[1].e[1] * C[5] - M.c[1].e[2] * C[4] + M.c[1].e[3] * C[3]) * InvDet;
        R.c[0].e[1] = -(M.c[0].e[1] * C[5] - M.c[0].e[2] * C[4] + M.c[0].e[3] * C[3]) * InvDet;
        R.c[0].e[2] = +(M.c[3].e[1] * S[5] - M.c[3].e[2] * S[4] + M.c[3].e[3] * S[3]) * InvDet;
        R.c[0].e[3] = -(M.c[2].e[1] * S[5] - M.c[2].e[2] * S[4] + M.c[2].e[3] * S[3]) * InvDet;

        R.c[1].e[0] = -(M.c[1].e[0] * C[5] - M.c[1].e[2] * C[2] + M.c[1].e[3] * C[1]) * InvDet;
        R.c[1].e[1] = +(M.c[0].e[0] * C[5] - M.c[0].e[2] * C[2] + M.c[0].e[3] * C[1]) * InvDet;
        R.c[1].e[2] = -(M.c[3].e[0] * S[5] - M.c[3].e[2] * S[2] + M.c[3].e[3] * S[1]) * InvDet;
        R.c[1].e[3] = +(M.c[2].e[0] * S[5] - M.c[2].e[2] * S[2] + M.c[2].e[3] * S[1]) * InvDet;

        R.c[2].e[0] = +(M.c[1].e[0] * C[4] - M.c[1].e[1] * C[2] + M.c[1].e[3] * C[0]) * InvDet;
        R.c[2].e[1] = -(M.c[0].e[0] * C[4] - M.c[0].e[1] * C[2] + M.c[0].e[3] * C[0]) * InvDet;
        R.c[2].e[2] = +(M.c[3].e[0] * S[4] - M.c[3].e[1] * S[2] + M.c[3].e[3] * S[0]) * InvDet;
        R.c[2].e[3] = -(M.c[2].e[0] * S[4] - M.c[2].e[1] * S[2] + M.c[2].e[3] * S[0]) * InvDet;

        R.c[3].e[0] = -(M.c[1].e[0] * C[3] - M.c[1].e[1] * C[1] + M.c[1].e[2] * C[0]) * InvDet;
        R.c[3].e[1] = +(M.c[0].e[0] * C[3] - M.c[0].e[1] * C[1] + M.c[0].e[2] * C[0]) * InvDet;
        R.c[3].e[2] = -(M.c[3].e[0] * S[3] - M.c[3].e[1] * S[1] + M.c[3].e[2] * S[0]) * InvDet;
        R.c[3].e[3] = +(M.c[2].e[0] * S[3] - M.c[2].e[1] * S[1] + M.c[2].e[2] * S[0]) * InvDet;

        return R;
    }

    inline mat4 Frustum(float Left, float Right, float Bottom, float Top, float Near, float Far)
    {
        return
        {
            (Near * 2.f) / (Right - Left),   0.f,                              0.f,                               0.f,
            0.f,                             (Near * 2.f)   / (Top - Bottom),  0.f,                               0.f,
            (Right + Left) / (Right - Left), (Top + Bottom) / (Top - Bottom), -(Far + Near) / (Far - Near),      -1.f, 
            0.f,                             0.f,                             -(Far * Near * 2.f) / (Far - Near), 0.f
        };
    }

    inline mat4 Orthographic(float left, float right, float bottom, float top, float near, float far)
    {
        return {
            2.f / (right - left),               0.f,                                0.f,                            0.f,
            0.f,                                2.f / (top - bottom),               0.f,                            0.f,
            0.f,                                0.f,                                -2.f / (far - near),            0.f,
            -(right + left) / (right - left),   -(top + bottom) / (top-bottom),     -(far + near) / (far - near),   1.f
        };
    }
    
    inline mat4 Perspective(float FovY, float Aspect, float Near, float Far)
    {
        float Top = Near * tanf(FovY / 2.f);
        float Right = Top * Aspect;
        return Mat4::Frustum(-Right, Right, -Top, Top, Near, Far);
    }

    inline mat4 LookAt(v3 Eye, v3 At, v3 Up)
    {
        v3 ZAxis = Vec3::Normalize(At - Eye);
        v3 XAxis = Vec3::Normalize(Vec3::Cross(ZAxis, Up));
        v3 YAxis = Vec3::Cross(XAxis, ZAxis);

        return {
            XAxis.x, YAxis.x, -ZAxis.x, 0.f,
            XAxis.y, YAxis.y, -ZAxis.y, 0.f,
            XAxis.z, YAxis.z, -ZAxis.z, 0.f,
            -Vec3::Dot(XAxis, Eye), -Vec3::Dot(YAxis, Eye), Vec3::Dot(ZAxis, Eye), 1.f
        };
    }
};

#include "maths_extension.h"