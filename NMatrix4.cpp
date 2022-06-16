#include "NMatrix4.h"

//Matrix4& Matrix4::operator*=(const Matrix4& m2)
//{
//	Matrix4 result;
//	for (size_t i = 0; i < 4; i++)
//	{
//		for (size_t j = 0; j < 4; j++)
//		{
//			for (size_t k = 0; k < 4; k++)
//			{
//				result.m[i][j] += m[i][k] * m2.m[k][j];
//			}
//		}
//	}
//	return result;
//}

Matrix4& Matrix4::operator*=(const Vector3 scalar)
{
	m[0][0] *= scalar.x;
	m[1][0] *= scalar.x;
	m[2][0] *= scalar.x;
	m[3][0] *= scalar.x;

	m[0][1] *= scalar.y;
	m[1][1] *= scalar.y;
	m[2][1] *= scalar.y;
	m[3][1] *= scalar.y;

	m[0][2] *= scalar.z;
	m[1][2] *= scalar.z;
	m[2][2] *= scalar.z;
	m[3][2] *= scalar.z;
	return *this;
}

Matrix4 IdentityMatrix(Matrix4 matrix)
{
	for (int i = 0; i < 4; i++)
	{
		matrix.m[i][i] = 1;
	}
	return matrix;
}