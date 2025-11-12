#include "pch.h"
#include "ETCUtils.h"

#if 1
void ETCUtils_AddMyMatrixToCls(FILE* file, myMatrix4x4 matrix)
{
	for (int i = 0; i < 4; i++)
	{
		::fprintf(file, "%f, %f, %f, %f \n",
			matrix[i][0], matrix[i][1], matrix[i][2], matrix[i][3]);
	}
}
#endif


#if 1
void ETCUtils_AddSimpleMatrixToCls(FILE* file, SimpleMath::Matrix matrix)
{
	::fprintf(file, "%f, %f, %f, %f \n",
		matrix._11, matrix._12, matrix._13, matrix._14);
	::fprintf(file, "%f, %f, %f, %f \n",
		matrix._21, matrix._22, matrix._23, matrix._24);
	::fprintf(file, "%f, %f, %f, %f \n",
		matrix._31, matrix._32, matrix._33, matrix._34);
	::fprintf(file, "%f, %f, %f, %f \n",
		matrix._41, matrix._42, matrix._43, matrix._44);
}
#endif