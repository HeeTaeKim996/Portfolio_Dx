/*
	○ Shaders fx 파일 만드는 과정 정리
	 - fx 파일들이 Client Project 내에 위치하지 않고, $(SolutionDir)Shaders\ 에 위치하기 때문에, 추가적 절차 필요
	 - 1) Client든, Engine이든, hlsl 파일을 만든 후, 삭제 -> 제거
	 - 2) 만들어진 hlsl 파일을, Ctrl + X 후, $(SolutionDir)Shaders\ 로 이동
	 - 3) 이동한 hlsl 파일을 드래그 하여, Client 의 Shaders 파일로 이동
     - 4) 확장자를 hlsl 이 아닌, fx로 수정

	○ fx 파일 초기화 작업
	 - 해당 fx 파일은, 꼭짓점 3D hlsl 파일로 생성함
	 - 속성 ->		진입점 이름 : "" (없음),		셰이더 형식 : 효과(/fx),		셰이더 모델 : Shader Model 5.0(/5_0)
	  ※ 효과(/fx) 는 Engine 의 Shader_externLibrary 의 Pass, Shader, Technique.. 등의 코드와 연계되어 사용됨
	 
*/


struct VertexInput
{
	float4 position : POSITION;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.position = input.position;

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
	return float4(1, 0, 0, 1);
}

float4 PS2(VertexOutput input) : SV_TARGET
{
	return float4(0, 1, 0, 1);
}

float4 PS3(VertexOutput input) : SV_TARGET
{
	return float4(0, 0, 1, 1);
}

technique11 T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS2()));
	}
};

technique11 T1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS3()));
	}
};