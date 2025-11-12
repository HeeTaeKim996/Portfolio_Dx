#include "TesterPch.h"
#include "MatrixCompare.h"
#include "TesterUtils.h"

void MatrixCompare::Compare(myVec3 startScale, myQuaternion startRotation, 
	myVec3 startPos, SimpleMatrix startParentMatrix, SimpleMatrix rootMat)
{
	cout << " -------------------------SRT ----------------------------" << endl;


	SimpleMatrix S = SimpleMatrix::CreateScale(startScale.X, startScale.Y, startScale.Z);

	SimpleQuaternion simpleQuaternion(startRotation.X, startRotation.Y, startRotation.Z, startRotation.W);
	SimpleMatrix R = SimpleMatrix::CreateFromQuaternion(simpleQuaternion);
	
	SimpleMatrix T = SimpleMatrix::CreateTranslation(startPos.X, startPos.Y, startPos.Z);

	SimpleMatrix SRT = S * R * T;



	myMatrix4x4 mySRT(startScale,
		myQuaternion(startRotation.X, startRotation.Y, startRotation.Z, startRotation.W),
		myVec3(startPos.X, startPos.Y, startPos.Z));

	TransformBase mySRTTrnasofmr(myVec3(startPos.X, startPos.Y, startPos.Z),
		myQuaternion(startRotation.X, startRotation.Y, startRotation.Z, startRotation.W),
		startScale);


	cout << "MyMat2" << endl;
	myMatrix4x4 myS = myMatrix4x4(
		myVec4(startScale.X, 0, 0, 0),
		myVec4(0, startScale.Y, 0, 0),
		myVec4(0, 0, startScale.Z, 0),
		myVec4(0, 0, 0, 1)
	);
	myMatrix3x3 myR3x3 = startRotation.ToRotateMatrix();
	myMatrix4x4 myR = myR3x3.ToMatrix4x4();
	myMatrix4x4 myT = myMatrix4x4(
		myVec4(1, 0, 0, 0),
		myVec4(0, 1, 0, 0),
		myVec4(0, 0, 1, 0),
		myVec4(startPos, 1)
	);
	myMatrix4x4 mySRT2 = myS * myR * myT;

	
	cout << "S" << endl;
	cout << "Simple" << endl;
	PrintSimpleMatrix(S);
	cout << "MyMat" << endl;
	PrintMatrix4x4(myS);

	cout << "R" << endl;
	cout << "Simple" << endl;
	PrintSimpleMatrix(R);
	cout << "MyMat" << endl;
	PrintMatrix4x4(myR);
	
	cout << "T" << endl;
	cout << "Simple" << endl;
	PrintSimpleMatrix(T);
	cout << "My" << endl;
	PrintMatrix4x4(myT);



	cout << "SRT" << endl;
	cout << "Simple Math" << endl;
	PrintSimpleMatrix(SRT);
	cout << "MyMat" << endl;
	PrintMatrix4x4(mySRT);
	cout << "MyMat2" << endl;
	PrintMatrix4x4(mySRT2);
	cout << "MyMat3" << endl;
	PrintMatrix4x4(mySRTTrnasofmr.GetSRT());





	SimpleMatrix SRT_Parent = SRT * startParentMatrix;

	myMatrix4x4 myParentMat(&startParentMatrix._11);
	myMatrix4x4 mySRT_Parent = mySRT * myParentMat;
	
	cout << "SRT * Parent" << endl;
	cout << "Simple" << endl;
	PrintSimpleMatrix(SRT_Parent);
	cout << "My" << endl;
	PrintMatrix4x4(mySRT_Parent);




	

	SimpleMatrix invRootMat = rootMat.Invert();

	myMatrix4x4 myRootMat(&rootMat._11);
	TransformBase myRootTransform(myRootMat);
	TransformBase myInvRootTransform = myRootTransform.Inverse();
	myMatrix4x4 myInvRootMat = myInvRootTransform.GetSRT();

	cout << "Root Mat" << endl;
	cout << "Simple" << endl;
	PrintSimpleMatrix(rootMat);
	cout << "My" << endl;
	PrintMatrix4x4(myRootMat);



	cout << " Inverted Root Matrix " << endl;
	cout << "Simple" << endl;
	PrintSimpleMatrix(invRootMat);
	cout << "My" << endl;
	PrintMatrix4x4(myInvRootMat);



	SimpleMatrix finalMat = invRootMat * SRT_Parent;
	myMatrix4x4 myFinalMat = myInvRootMat * mySRT_Parent;
	
	TransformBase mySRT_Parent_Trnasform(mySRT_Parent);
	TransformBase finalTrnasform = mySRT_Parent_Trnasform.LocalToWorld(myInvRootTransform);
	myMatrix4x4 finalMat2 = finalTrnasform.GetSRT();


	cout << "Final Mal" << endl;
	cout << "Simple" << endl;
	PrintSimpleMatrix(finalMat);
	cout << "My" << endl;
	PrintMatrix4x4(myFinalMat);
	cout << "My2(UsingTrnasform)" << endl;
	PrintMatrix4x4(finalMat2);


}
