#pragma once
#include "TesterPch.h"
#include "assert.h"
#include "TesterUtils.h"
#include "MatrixCompare.h"

//#include "myVec3.h"
//#include "myMatrix3x3.h"
//#include "myVec4.h"
//#include "myMatrix4x4.h"
//#include "myEulerAngles.h"
//#include "myQuaternion.h"
//#include "TransformBase.h"

using namespace std;



// 부동소수 비교용
static constexpr float EPS = 1e-5f;
static inline bool feq(float a, float b, float eps = EPS) { return std::fabs(a - b) <= eps; }

static void SpeakVec(const char* tag, const myVec3& v)
{
    cout << tag << " : " << v.X << " / " << v.Y << " / " << v.Z << endl;
}

// 기대값과 비교 + 출력
static void ExpectVec(const char* tag, const myVec3& v, float x, float y, float z)
{
    SpeakVec(tag, v);
    assert(feq(v.X, x) && feq(v.Y, y) && feq(v.Z, z) && "Vector value mismatch");
}

void TestVec3()
{
    // 1) 생성자/멤버 접근
    myVec3 a(1, 2, 3);
    ExpectVec("a", a, 1, 2, 3);


    // 2) +, -
    myVec3 b(4, 5, 6);
    ExpectVec("b", b, 4, 5, 6);
    myVec3 c = a + b;            ExpectVec("a + b", c, 5, 7, 9);
    myVec3 d = a - b;            ExpectVec("a - b", d, -3, -3, -3);

    // 3) 스칼라 곱/나눗셈 및 복합대입
    myVec3 e = a * 2.f;          ExpectVec("a * 2", e, 2, 4, 6);
    myVec3 f = e / 2.f;          ExpectVec("(a*2)/2", f, 1, 2, 3);

    myVec3 g = a;  g += b;       ExpectVec("g += b", g, 5, 7, 9);
    g -= b;          ExpectVec("g -= b", g, 1, 2, 3);
    g *= 3.f;        ExpectVec("g *= 3", g, 3, 6, 9);
    g /= 3.f;        ExpectVec("g /= 3", g, 1, 2, 3);

    // 4) 인덱서 (읽기/쓰기)
    assert(feq(a[0], 1) && feq(a[1], 2) && feq(a[2], 3));
    a[1] = 10.f;                 ExpectVec("a[1] = 10", a, 1, 10, 3);

    // 5) Size / SizeSquared
    myVec3 h(3, 4, 12);
    assert(feq(h.SizeSquared(), 169.f));
    assert(feq(h.Size(), 13.f));
    SpeakVec("h size check (13)", h);

    // 6) Normalize / GetNormalize
    myVec3 n(3, 0, 4);
    myVec3 n_hat = n.GetNormalize();
    assert(feq(n_hat.Size(), 1.f));
    SpeakVec("GetNormalize(3,0,4)", n_hat);

    n.Normalize();
    assert(feq(n.Size(), 1.f));
    ExpectVec("Normalize(3,0,4)", n, 0.6f, 0.0f, 0.8f);

    myVec3 z(0, 0, 0);
    z.Normalize();               ExpectVec("Normalize(0,0,0)", z, 0, 0, 0);

    // 7) Dot / Cross
    myVec3 u(1, 2, 3), v(4, -5, 6);
    float dot = u.Dot(v);        cout << "dot(u,v) = " << dot << endl;
    assert(feq(dot, 12.f));

    myVec3 cr = u.Cross(v);      ExpectVec("u x v", cr, 27.f, 6.f, -13.f);

    
    cout << "=== 모든 Vec3 테스트 통과 ===" << endl;
}

void TestMatrix3x3()
{
    // 단위행렬 I
    myMatrix3x3 I;
    cout << "I = Identity Matrix" << endl;

    // 벡터 v
    myVec3 v(1, 2, 3);
    cout << "v = (1,2,3)" << endl;

    // v * I
    myVec3 r1 = v * I;
    cout << "v * I : " << r1.X << " / " << r1.Y << " / " << r1.Z << endl;

    // I * 2
    myMatrix3x3 M = I * 2.f;
    cout << "M = I * 2" << endl;
    cout << "I * 2 : "
        << M[0].X << "," << M[0].Y << "," << M[0].Z << " | "
        << M[1].X << "," << M[1].Y << "," << M[1].Z << " | "
        << M[2].X << "," << M[2].Y << "," << M[2].Z << endl;

    // I * I
    myMatrix3x3 M2 = I * I;
    cout << "I * I : "
        << M2[0].X << "," << M2[0].Y << "," << M2[0].Z << " | "
        << M2[1].X << "," << M2[1].Y << "," << M2[1].Z << " | "
        << M2[2].X << "," << M2[2].Y << "," << M2[2].Z << endl;

    // Transpose(M)
    myMatrix3x3 T = M.Transpose();
    cout << "Transpose(M) : "
        << T[0].X << "," << T[0].Y << "," << T[0].Z << " | "
        << T[1].X << "," << T[1].Y << "," << T[1].Z << " | "
        << T[2].X << "," << T[2].Y << "," << T[2].Z << endl;

    // 비대칭 행렬 A
    myMatrix3x3 A(
        myVec3(1, 2, 3),
        myVec3(0, 4, 5),
        myVec3(0, 0, 6)
    );
    cout << "A = (1,2,3 | 0,4,5 | 0,0,6)" << endl;

    myMatrix3x3 AT = A.Transpose();
    cout << "Transpose(A) : "
        << AT[0].X << "," << AT[0].Y << "," << AT[0].Z << " | "
        << AT[1].X << "," << AT[1].Y << "," << AT[1].Z << " | "
        << AT[2].X << "," << AT[2].Y << "," << AT[2].Z << endl;

    // v * N (대각행렬)
    myMatrix3x3 N(
        myVec3(1, 0, 0),
        myVec3(0, 2, 0),
        myVec3(0, 0, 3)
    );
    cout << "N = diag(1,2,3)" << endl;

    myVec3 r2 = v * N;
    cout << "v * N : " << r2.X << " / " << r2.Y << " / " << r2.Z << endl;

    cout << "V's Size : " << v.Size() << endl;
}

void TestVec4Matrix4x4()
{
    // 벡터/행렬 준비
    myVec4 v(1, 2, 3, 1);                         cout << "v~~ : " << v.X << "," << v.Y << "," << v.Z << "," << v.W << endl;
    myVec4 w(4, -5, 6, 0.5f);                     cout << "w~~ : " << w.X << "," << w.Y << "," << w.Z << "," << w.W << endl;

    myMatrix4x4 I;                                cout << "I~~ : "
        << I[0].X << "," << I[0].Y << "," << I[0].Z << "," << I[0].W << " | "
        << I[1].X << "," << I[1].Y << "," << I[1].Z << "," << I[1].W << " | "
        << I[2].X << "," << I[2].Y << "," << I[2].Z << "," << I[2].W << " | "
        << I[3].X << "," << I[3].Y << "," << I[3].Z << "," << I[3].W << endl;

    myMatrix4x4 M(
        myVec4(1, 2, 3, 4),
        myVec4(0, 5, 6, 7),
        myVec4(0, 0, 8, 9),
        myVec4(0, 0, 0, 10)
    );
    cout << "M~~ : "
        << M[0].X << "," << M[0].Y << "," << M[0].Z << "," << M[0].W << " | "
        << M[1].X << "," << M[1].Y << "," << M[1].Z << "," << M[1].W << " | "
        << M[2].X << "," << M[2].Y << "," << M[2].Z << "," << M[2].W << " | "
        << M[3].X << "," << M[3].Y << "," << M[3].Z << "," << M[3].W << endl;

    // myVec4 연산
    myVec4 add = v + w;                           cout << "v + w~~ : " << add.X << "," << add.Y << "," << add.Z << "," << add.W << endl;
    myVec4 sub = v - w;                           cout << "v - w~~ : " << sub.X << "," << sub.Y << "," << sub.Z << "," << sub.W << endl;
    myVec4 sm = v * 2.f;                         cout << "v * 2~~ : " << sm.X << "," << sm.Y << "," << sm.Z << "," << sm.W << endl;
    myVec4 sd = v / 2.f;                         cout << "v / 2~~ : " << sd.X << "," << sd.Y << "," << sd.Z << "," << sd.W << endl;

    myVec4 vt = v; vt += w;                       cout << "v += w~~ : " << vt.X << "," << vt.Y << "," << vt.Z << "," << vt.W << endl;
    vt -= w;                                      cout << "v -= w~~ : " << vt.X << "," << vt.Y << "," << vt.Z << "," << vt.W << endl;
    vt *= 3.f;                                    cout << "v *= 3~~ : " << vt.X << "," << vt.Y << "," << vt.Z << "," << vt.W << endl;
    vt /= 3.f;                                    cout << "v /= 3~~ : " << vt.X << "," << vt.Y << "," << vt.Z << "," << vt.W << endl;

    float vdot = v.Dot(w);                        cout << "dot(v,w)~~ : " << vdot << endl;
    cout << "v.Size()~~ : " << v.Size() << endl;

    myVec4 vhat = v.GetNormalize();               cout << "Normalize(v)~~ : " << vhat.X << "," << vhat.Y << "," << vhat.Z << "," << vhat.W << endl;
    myVec4 vn = v; vn.Normalize();                cout << "v.Normalize()~~ : " << vn.X << "," << vn.Y << "," << vn.Z << "," << vn.W << endl;

    myVec3 v3 = v.ToVec3();                       cout << "v.ToVec3()~~ : " << v3.X << "," << v3.Y << "," << v3.Z << endl;

    // 벡터 * 행렬
    myVec4 vI = v * I;                            cout << "v * I~~ : " << vI.X << "," << vI.Y << "," << vI.Z << "," << vI.W << endl;
    myVec4 vM = v * M;                            cout << "v * M~~ : " << vM.X << "," << vM.Y << "," << vM.Z << "," << vM.W << endl;
    myVec4 vMeq = v; vMeq *= M;                   cout << "v *= M~~ : " << vMeq.X << "," << vMeq.Y << "," << vMeq.Z << "," << vMeq.W << endl;

    // myMatrix4x4 연산
    myMatrix4x4 I2 = I * 2.f;                     cout << "I * 2~~ : "
        << I2[0].X << "," << I2[0].Y << "," << I2[0].Z << "," << I2[0].W << " | "
        << I2[1].X << "," << I2[1].Y << "," << I2[1].Z << "," << I2[1].W << " | "
        << I2[2].X << "," << I2[2].Y << "," << I2[2].Z << "," << I2[2].W << " | "
        << I2[3].X << "," << I2[3].Y << "," << I2[3].Z << "," << I2[3].W << endl;

    myMatrix4x4 MT = M.Transpose();               cout << "Transpose(M)~~ : "
        << MT[0].X << "," << MT[0].Y << "," << MT[0].Z << "," << MT[0].W << " | "
        << MT[1].X << "," << MT[1].Y << "," << MT[1].Z << "," << MT[1].W << " | "
        << MT[2].X << "," << MT[2].Y << "," << MT[2].Z << "," << MT[2].W << " | "
        << MT[3].X << "," << MT[3].Y << "," << MT[3].Z << "," << MT[3].W << endl;

    myMatrix4x4 MM = M * MT;                      cout << "M * Transpose(M)~~ : "
        << MM[0].X << "," << MM[0].Y << "," << MM[0].Z << "," << MM[0].W << " | "
        << MM[1].X << "," << MM[1].Y << "," << MM[1].Z << "," << MM[1].W << " | "
        << MM[2].X << "," << MM[2].Y << "," << MM[2].Z << "," << MM[2].W << " | "
        << MM[3].X << "," << MM[3].Y << "," << MM[3].Z << "," << MM[3].W << endl;

    myMatrix4x4 K = M; K.SetIdentity();           cout << "K.SetIdentity()~~ : "
        << K[0].X << "," << K[0].Y << "," << K[0].Z << "," << K[0].W << " | "
        << K[1].X << "," << K[1].Y << "," << K[1].Z << "," << K[1].W << " | "
        << K[2].X << "," << K[2].Y << "," << K[2].Z << "," << K[2].W << " | "
        << K[3].X << "," << K[3].Y << "," << K[3].Z << "," << K[3].W << endl;
}


void TestQuaternion()
{
    if(0)
    {
        //myEulerAngles startEuler(90.f, 0.f, 0.f);
        //myEulerAngles startEuler(45.f, 45.f, 0.f);
        myEulerAngles startEuler(45.f, 0, 0);

        myQuaternion mainQuaternion(startEuler);

        myVec3 testVec = mainQuaternion.RotateVector(myVec3::UnitX);

        cout << "StartPos" << endl;
        cout << testVec.X << "  /  " << testVec.Y << "  /  " << testVec.Z << endl;
        printf("\n");


        myQuaternion rotQuaternion(myVec3(1, 0, 0), 90.f);

        mainQuaternion *= rotQuaternion;

        myVec3 testVec2 = mainQuaternion.RotateVector(myVec3::UnitX);

        cout << " Frist Rot Pos" << endl;
        cout << testVec2.X << "  /  " << testVec2.Y << "  /  " << testVec2.Z << endl;
        printf("\n");


        myMatrix3x3 rotMatrix = mainQuaternion.ToRotateMatrix();

        cout << "Rot Matrix IS " << endl;
        PrintMatrix3x3(rotMatrix);
        

        mainQuaternion.Normalize();

        cout << "Again After Normalized" << endl;
        PrintMatrix3x3(mainQuaternion.ToRotateMatrix());
    }


    if(0)
    {
        cout << "Regression Test" << endl;
        myEulerAngles startEuler(26.f, 87.f, 47.f);
        //myEulerAngles startEuler(0.f, 0.001f, 80.999f);
        //myEulerAngles startEuler(0.1, 0.2f, 0.3);

        PrintEulerAngles(startEuler);

        myQuaternion startQuat(startEuler);
        PrintQuaternion(startQuat);

        myMatrix3x3 vertexMat = startQuat.ToRotateMatrix();
        PrintMatrix3x3(vertexMat);

        myQuaternion regQuat(vertexMat);
        PrintQuaternion(regQuat);

        myEulerAngles regEuler = regQuat.ToEulerAngles();
        PrintEulerAngles(regEuler);

    }



    if (1) // 회전 보간 테스트
    {
        myQuaternion startQuaternion(myVec3(0.0f, 0.0f, 0.0f));
        myQuaternion endQuaternion(myVec3(90.f, 90.f, 0.f));
        myQuaternion lepredQuaternion;
        
        PrintQuaternion(startQuaternion);
        PrintQuaternion(endQuaternion);


        cout << "Lerp" << endl;
        for (int i = 0; i <= 10; i++)
        {
            lepredQuaternion.Slerp(startQuaternion, endQuaternion, i / 10.f);

            cout << i/ 10.f << endl;
            PrintQuaternion(lepredQuaternion);
        }


    }

}



void TestTransformBase()
{
    if (0) // Reg Test
    {
        TransformBase localTransform(myVec3(2, 1, 3), myQuaternion(myVec3(12.f, 24.f, 36.f)), myVec3(5, 6, 7));
        cout << "StartLocalTransform" << endl;
        PrintTransformBase(localTransform);

        TransformBase parentTransform(myVec3(5, 2, 7), myQuaternion(myVec3(40.f, 52.f, 20.f)), myVec3(1, 4, 3));

        TransformBase worldTransform = localTransform.LocalToWorld(parentTransform);
        cout << "WorldTransform" << endl;
        PrintTransformBase(worldTransform);

        TransformBase regLocalTransform = worldTransform.WorldToLocal(parentTransform);
        cout << "RegLocalTransform" << endl;
        PrintTransformBase(regLocalTransform);
    }


    if (0) // 월드 변환 잘됐는지 확인 테스트
    {
        cout << "월드 변환 잘됐는지 확인하는 코드" << endl;
        TransformBase localTransform(myVec3(1, 0, 0));

        TransformBase parentTransform(myVec3(0, 0, 0), myQuaternion(myVec3(90.f, 0, 0)), myVec3(1, 1, 1));

        TransformBase worldTrnasform = localTransform.LocalToWorld(parentTransform);
        PrintTransformBase(worldTrnasform);
    }


    if (0) // Add 회전축 테스트
    {
        cout << "AddRot 으로 잘 회전되는지 확인하는 코드" << endl;
        TransformBase transform(myVec3(1, 0, 0));

        transform.AddYawRotation(90.f);

        PrintTransformBase(transform);
    }
}


int main()
{
    //TestVec3();

    //TestMatrix3x3();

    //TestVec4Matrix4x4();

    //TestQuaternion();

    //TestTransformBase();


    {
        //myMatrix4x4 firstMatrix =
        //    myMatrix4x4(myVec4(1, 2, 3, 4), myVec4(5, 6, 7, 8), myVec4(8, 7, 6, 5), myVec4(4, 3, 2, 1));

        //myMatrix4x4 secondMatrix(&firstMatrix[0][0]);

        //for (int i = 0; i < 4; i++)
        //{
        //    for (int j = 0; j < 4; j++)
        //    {
        //        printf("%f\t", secondMatrix[i][j]);
        //    }
        //    printf("\n");
        //}
    }



    {
        //myMatrix4x4 scaleMatrix(
        //    myVec4(2, 0, 0, 0), 
        //    myVec4(0, 3, 0, 0), 
        //    myVec4(0, 0, 4, 0), 
        //    myVec4(0, 0, 0, 1));

        //myQuaternion quatRot(myEulerAngles(24, 62, 18));
        //myMatrix3x3 rotMatrix33 = quatRot.ToRotateMatrix();
        //myMatrix4x4 rotMatrix = rotMatrix33.ToMatrix4x4();


        //myMatrix4x4 translationMatrix(
        //    myVec4(1, 0, 0, 0),
        //    myVec4(0, 1, 0, 0),
        //    myVec4(0, 0, 1, 0),
        //    myVec4(52, 11, 27, 1));

        //
        //myMatrix4x4 srt = scaleMatrix * rotMatrix * translationMatrix;

        //PrintMatrix4x4(scaleMatrix);
        //PrintMatrix4x4(rotMatrix);
        //PrintMatrix4x4(translationMatrix);
        //PrintMatrix4x4(srt);

        //myVec3 pos, scale;
        //myQuaternion quat;

        //srt.Decompose(pos, quat, scale);

        //PrintVec3(pos);
        //PrintVec3(scale);
        //PrintVec3(quat.ToEulerAngles().ToVec3());
    }


#if 0
    {
        myMatrix4x4 srt(myVec3(2, 7, 4), myQuaternion(myEulerAngles(18.2f, 43.7f, 52.1f)),
            myVec3(-18.2, 20.3, 40.2));

        PrintMatrix4x4(srt);

        myVec3 regScale;
        myQuaternion regRotation;
        myVec3 regTranslation;

        srt.Decompose(regScale, regRotation, regTranslation);

        PrintVec3(regScale);
        PrintVec3(regRotation.ToEulerAngles().ToVec3());
        PrintVec3(regTranslation);
    }
#endif

    {



#if 0
        SimpleMatrix parent = SimpleMatrix(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );

        SimpleMatrix rootTransform = SimpleMatrix(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0.042101, 114.053238, -0.311199, 1
        );

        MatrixCompare::Compare(myVec3(1, 1, 1), myQuaternion(-0.075489f, 0.479205f, 0.037999f, 0.873624f),
            myVec3(0.577255, 95.37883, 1.211316), parent, rootTransform);
#else

        TransformBase parentBase(
            myVec3(22.f, 75.f, 24.f), 
            myQuaternion(myEulerAngles(22.f, 52.f, 30.f)),
            myVec3(2, 6, 3.2f));
        myMatrix4x4 parentMat = parentBase.GetSRT();
        SimpleMatrix parent(&parentMat[0][0]);

        TransformBase rootBase(
            myVec3(42.f, 36.f, 18.f),
            myQuaternion(myEulerAngles(18.2f, 37.4f, 15.2f)),
            myVec3(3.2f, 6.7f, 1.8f));
        myMatrix4x4 rootMat = rootBase.GetSRT();
        SimpleMatrix root(&rootMat[0][0]);


        MatrixCompare::Compare(myVec3(1, 22, 67), myQuaternion(-0.075489f, 0.479205f, 0.037999f, 0.873624f),
            myVec3(0.577255, 95.37883, 1.211316), parent, root);
#endif
    }


}
