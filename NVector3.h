#pragma once

const float PI = 3.141592654f;

class Vector3
{
public:
	float x;
	float y;
	float z;

public:
	Vector3();							//��x�N�g���Ƃ���
	Vector3(float x, float y, float z);	//x����,y����,z�������w�肵�Ă̐���

	//�����o�ϐ�
	float length() const;					//�m����(����)�����߂�
	Vector3& normalize();					//���K��������
	float dot(const Vector3& v) const;		//���Ђ����߂�
	Vector3 cross(const Vector3& v) const;	//�O�ς����߂�

	//�P�����Z�q�I�[�o�[���[�h
	Vector3 operator+() const;
	Vector3 operator-() const;

	//������Z�q�I�[�o�[���[�h
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

//�x���@�����W�A��
float Degree2Radian(float degree);
//�x���@�����W�A��
float Radian2Degree(float radian);