/****************************************************************************
 *
 * 	Camera.h by z.keyring (c) 2013 All Right Reserved.
 *
 * 					--code with gVim :)
 ***************************************************************************/

#ifndef __CAMERA_H__
#define __CAMREA_H__ 1


#include "AppUtil.h"

class Camera{

	public:
		Camera();
		~Camera();

		XMVECTOR getPositionVec(void) const;
		void setPosition(const XMFLOAT3& v);
		XMFLOAT3 getPosition(void) const;
		void setPosition(float, float, float);

		XMVECTOR getRightVec(void) const;
		XMFLOAT3 getRight(void) const;
		XMVECTOR getUpVec(void) const;
		XMFLOAT3 getUp(void) const;
		XMVECTOR getLookVec(void) const;
		XMFLOAT3 getLook(void) const;

		XMMATRIX getViewMat(void) const;
		XMMATRIX getProjMat(void) const;
		XMMATRIX getViewProjMat(void) const;

		float getFovX(void) const;
		float getFovY(void) const;
		float getAspect(void) const;
		float getNearZ(void) const;
		float getFarZ(void) const;

		float getNearPlaneWidth(void) const;
		float getNearPlaneHeight(void) const;
		float getFarPlaneWidth(void) const;
		float getFarPlaneHeight(void) const;

		void setProjParam(float fovy, float aspect, float nearz, float farz);

		void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up);
		void LookAt(const XMFLOAT3 &pos, const XMFLOAT3 &target, const XMFLOAT3 &up);

		void MoveRightVec(float dist);
		void MoveUpVec(float dist);
		void MoveLookVec(float dist);

		void RotateRightVec(float angle);
		void RotateUpVec(float angle);
		void RotateLookVec(float angle);

		void RotateX(float angle);
		void RotateY(float angle);
		void RotateZ(float angle);

		void UpdateViewMat();

	private:
		XMFLOAT3 m_position;
		XMFLOAT3 m_right;
		XMFLOAT3 m_up;
		XMFLOAT3 m_look;

		XMFLOAT4X4 m_viewMat;
		XMFLOAT4X4 m_projMat;

		float m_fovY;
		float m_aspect;
		float m_nearZ;
		float m_farZ;
		float m_nearPlaneHeight;
		float m_farPlaneHeight;
};

#endif   //__CAMERA_H__
