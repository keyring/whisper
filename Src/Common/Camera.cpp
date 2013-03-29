/**************************************************************************
 *
 * 	Camera.cpp by z.keyring (c) 2013 All Right Reserved.
 *
 * 					--code with gVim :)
 *************************************************************************/

#include "Camera.h"

Camera::Camera(){

	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_right    = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_up       = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_look     = XMFLOAT3(0.0f, 0.0f, 1.0f);

	setProjParam(XM_PI * 0.25, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera(){

}

XMVECTOR Camera::getPositionVec(void) const {

	return XMLoadFloat3(&m_position);
}

XMFLOAT3 Camera::getPosition(void) const {

	return m_position;
}

void Camera::setPosition(float x, float y, float z){

	m_position = XMFLOAT3(x, y, z);
}

void Camera::setPosition(const XMFLOAT3 &v){

	m_position = v;
}

XMVECTOR Camera::getRightVec(void) const {

	return XMLoadFloat3(&m_right);
}

XMFLOAT3 Camera::getRight(void) const {

	return m_right;
}

XMVECTOR Camera::getUpVec(void) const {

	return XMLoadFloat3(&m_up);
}

XMFLOAT3 Camera::getUp(void) const {

	return m_up;
}

XMVECTOR Camera::getLookVec(void) const {

	return XMLoadFloat3(&m_look);
}

XMFLOAT3 Camera::getLook(void) const {

	return m_look;
}

XMMATRIX Camera::getViewMat(void) const {

	return XMLoadFloat4x4(&m_viewMat);
}

XMMATRIX Camera::getProjMat(void) const {

	return XMLoadFloat4x4(&m_projMat);
}

XMMATRIX Camera::getViewProjMat(void) const {

	return XMMatrixMultiply( getViewMat(), getProjMat());
}

float Camera::getFovX(void) const {

	float halfWidth = 0.5f * getNearPlaneWidth();
	return 2.0f * atan( halfWidth / m_nearZ);
}

float Camera::getFovY(void) const {

	return m_fovY;
}

float Camera::getAspect(void) const {

	return m_aspect;
}

float Camera::getNearZ(void) const {

	return m_nearZ;
}

float Camera::getFarZ(void) const {

	return m_farZ;
}

float Camera::getNearPlaneWidth(void) const {

	return m_aspect * m_nearPlaneHeight;
}

float Camera::getNearPlaneHeight(void) const {

	return m_nearPlaneHeight;
}

float Camera::getFarPlaneWidth(void) const {

	return m_aspect * m_farPlaneHeight;
}

float Camera::getFarPlaneHeight(void) const {

	return m_farPlaneHeight;
}

void Camera::setProjParam(float fovy, float aspect, float nearz, float farz){

	m_fovY   = fovy;
	m_aspect = aspect;
	m_nearZ  = nearz;
	m_farZ   = farz;

	m_nearPlaneHeight = 2.0f * m_nearZ * tanf( 0.5f * m_fovY );
	m_farPlaneHeight  = 2.0f * m_farZ * tanf( 0.5f * m_fovY );

	XMMATRIX p = XMMatrixPerspectiveFovLH( m_fovY, m_aspect, m_nearZ, m_farZ );
	XMStoreFloat4x4(&m_projMat, p);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up){

	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(up, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_position, pos);
	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_look, L);
}

void Camera::LookAt(const XMFLOAT3 &pos, const XMFLOAT3 &target, const XMFLOAT3 &up){

	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);
}

void Camera::MoveRightVec(float dist){

	XMVECTOR pos = XMLoadFloat3(&m_position);
	XMVECTOR right = XMLoadFloat3(&m_right);
	pos += right * XMVectorReplicate(dist);
	
	XMStoreFloat3(&m_position, pos);
}

void Camera::MoveUpVec(float dist){

	XMVECTOR pos = XMLoadFloat3(&m_position);
	XMVECTOR up = XMLoadFloat3(&m_up);
	pos += up * XMVectorReplicate(dist);
	
	XMStoreFloat3(&m_position, pos);
}

void Camera::MoveLookVec(float dist){

	XMVECTOR pos = XMLoadFloat3(&m_position);
	XMVECTOR look = XMLoadFloat3(&m_look);
	pos += look * XMVectorReplicate(dist);
	
	XMStoreFloat3(&m_position, pos);
}

void Camera::RotateRightVec(float angle){

	XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), rotation));
}

void Camera::RotateUpVec(float angle){

	XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&m_up), angle);
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotation));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), rotation));
}

void Camera::RotateLookVec(float angle){

	XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&m_look), angle);
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotation));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));	
}

void Camera::RotateX(float angle){

	XMMATRIX rotation = XMMatrixRotationX(angle);
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), rotation));
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotation));
}
void Camera::RotateY(float angle){

	XMMATRIX rotation = XMMatrixRotationY(angle);
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), rotation));
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotation));
}

void Camera::RotateZ(float angle){

	XMMATRIX rotation = XMMatrixRotationZ(angle);
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), rotation));
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotation));
}

void Camera::UpdateViewMat(){

	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR u = XMLoadFloat3(&m_up);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	
	l = XMVector3Normalize(l);
	u = XMVector3Normalize(XMVector3Cross(l, r));
	r = XMVector3Cross(u, l);	
	
	float x = -XMVectorGetX(XMVector3Dot(p, r));
	float y = -XMVectorGetX(XMVector3Dot(p, u));
	float z = -XMVectorGetX(XMVector3Dot(p, l));
	
	XMStoreFloat3(&m_right, r);
	XMStoreFloat3(&m_up, u);
	XMStoreFloat3(&m_look, l);
	XMStoreFloat3(&m_position, p);
	
	m_viewMat(0,0) = m_right.x;
	m_viewMat(0,1) = m_up.x; 
	m_viewMat(0,2) = m_look.x;
	m_viewMat(0,3) = 0;

	m_viewMat(1,0) = m_right.y;
	m_viewMat(1,1) = m_up.y;
	m_viewMat(1,2) = m_look.y;
	m_viewMat(1,3) = 0;

	m_viewMat(2,0) = m_right.z; 
	m_viewMat(2,1) = m_up.z;
	m_viewMat(2,2) = m_look.z;
	m_viewMat(2,3) = 0;

	m_viewMat(3,0) = x;   
	m_viewMat(3,1) = y;       
	m_viewMat(3,2) = z;         
	m_viewMat(3,3) = 1;
}

