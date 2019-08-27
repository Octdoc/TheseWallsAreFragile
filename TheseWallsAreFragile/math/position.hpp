#pragma once

#include "linalg.hpp"

namespace twaf
{
	template <typename T>
	class Position
	{
	public:
		vec3<T> position;
		vec3<T> rotation;
		vec3<T> scale;

	public:
		Position() { Reset(); }

		void MoveForward(T d)
		{
			position.x += sin(rotation.y) * d;
			position.z += cos(rotation.y) * d;
		}
		void MoveBackward(T d)
		{
			position.x -= sin(rotation.y) * d;
			position.z -= cos(rotation.y) * d;
		}
		void MoveRight(T d)
		{
			position.x += cos(rotation.y) * d;
			position.z -= sin(rotation.y) * d;
		}
		void MoveLeft(T d)
		{
			position.x -= cos(rotation.y) * d;
			position.z += sin(rotation.y) * d;
		}
		void MoveUp(T d) { position.y += d; }
		void MoveDown(T d) { position.y -= d; }
		void Move(vec3<T> delta) { position += delta; }
		void MoveInLookDirection(T d) { MoveInLookDirection(vec3<T>(0, 0, d)); }
		void MoveInLookDirection(vec3<T> delta) { position += GetRotationMatrix3x3() * delta; }

		void LookDown(T r) { rotation.x += r; }
		void LookUp(T r) { rotation.x -= r; }
		void TurnRight(T r) { rotation.y += r; }
		void TurnLeft(T r) { rotation.y -= r; }
		void RollRight(T r) { rotation.z -= r; }
		void RollLeft(T r) { rotation.z += r; }

		void ScaleX(T s) { scale.x *= s; }
		void ScaleY(T s) { scale.y *= s; }
		void ScaleZ(T s) { scale.z *= s; }

		mat4x4<T> GetPositionMatrix() { return mat4x4<T>::Translation(position); }
		mat4x4<T> GetRotationMatrix() { return mat4x4<T>::Rotation(rotation); }
		mat4x4<T> GetScaleMatrix() { return mat4x4<T>::Scaling(scale); }
		mat4x4<T> GetWorldMatrix() { return mat4x4<T>::ScalingRotationTranslation(scale, rotation, position); }
		mat4x4<T> GetPositionMatrixInv() { return mat4x4<T>::Translation(-position); }
		mat4x4<T> GetRotationMatrixInv() { return mat4x4<T>::Rotation(rotation).Transposed(); }
		mat4x4<T> GetScaleMatrixInv() { return mat4x4<T>::Scaling(vec3<T>(1) / scale); }
		mat4x4<T> GetWorldMatrixInv() { return GetScaleMatrixInv() * GetRotationMatrixInv() * GetPositionMatrixInv(); }
		vec3<T> GetLookDirection() { return GetRotationMatrix3x3() * vec3<T>(0, 0, 1); }
		mat3x3<T> GetRotationMatrix3x3() { return mat3x3<T>::Rotation(rotation); }
		mat3x3<T> GetScaleMatrix3x3() { return mat3x3<T>::Scaling(scale); }
		mat3x3<T> GetRotationMatrixInv3x3() { return mat3x3<T>::Rotation(rotation).Transposed(); }
		mat3x3<T> GetScaleMatrixInv3x3() { return mat3x3<T>::Scaling(vec3<T>(1) / scale); }
		void Reset()
		{
			position = 0;
			rotation = 0;
			scale = 1;
		}
		T DistanceSquare(Position<T>& other) { return (position - other.position).LengthSquare(); }
		T Distance(Position<T>& other) { return (position - other.position).Length(); }

		template <typename S>
		operator Position<S>()
		{
			Position<S> pos;
			pos.position = (vec3<S>)position;
			pos.rotation = (vec3<S>)rotation;
			pos.scale = (vec3<S>)scale;
			return pos;
		}
		template <typename S>
		Position<S> WithType() { return (Position<S>) * this; }
	};

	using Positionf = Position<float>;
	using Positiond = Position<double>;
}