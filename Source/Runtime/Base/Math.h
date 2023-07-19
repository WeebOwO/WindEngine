#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/simd/matrix.h>
#include <utility>

namespace wind {
using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;

using Matrix2x2 = glm::mat2;
using Matrix3x3 = glm::mat3;
using Matrix4x4 = glm::mat4;
using Matrix3x4 = glm::mat3x4;

using VectorInt2 = glm::vec<2, int32_t>;
using VectorInt3 = glm::vec<3, int32_t>;
using VectorInt4 = glm::vec<4, int32_t>;

constexpr float HalfPi = glm::half_pi<float>();
constexpr float Pi     = glm::pi<float>();
constexpr float TwoPi  = glm::two_pi<float>();

template <typename T> T Normalize(const T& v) { return glm::normalize(v); }

template <typename T> float Length(const T& v) { return glm::length(v); }

template <typename T> float Dot(const T& v1, const T& v2) { return glm::dot(v1, v2); }

inline float ToRadians(float degrees) { return glm::radians(degrees); }

inline float ToDegrees(float radians) { return glm::degrees(radians); }

inline Matrix4x4 MakeRotationMatrix(const Vector3& rotations) {
    return glm::yawPitchRoll(rotations.y, rotations.x, rotations.z);
}

inline Vector3 MakeRotationAngles(const Matrix4x4& matrix) {
    Vector3 result{0.0f, 0.0f, 0.0f};
    glm::extractEulerAngleXYZ(matrix, result.x, result.y, result.z);
    return result;
}

inline Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
    Matrix4x4 result{1.0f};
    result[0][0] = scale.x;
    result[1][1] = scale.y;
    result[2][2] = scale.z;
    return result;
}

inline Matrix4x4 MakePerspectiveMatrix(float fov, float aspect, float znear, float zfar) {
    return glm::perspective(fov, aspect, znear, zfar);
}

inline Matrix4x4 MakeLookAtMatrix(const Vector3& position, const Vector3& direction,
                                  const Vector3& up) {
    return glm::lookAt(position, position + direction, up);
}

inline Matrix4x4 MakeOrthographicMatrix(float xLow, float xHigh, float yLow, float yHigh,
                                        float zLow, float zHigh) {
    return glm::ortho(xLow, xHigh, yLow, yHigh, zLow, zHigh);
}
} // namespace wind