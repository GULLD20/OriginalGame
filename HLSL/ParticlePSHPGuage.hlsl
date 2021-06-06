#include "Particle.hlsli"


Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

//本来は円のフェード処理
//なぜか四角になってる
float4 RoundFade(float4 fade1, float inputTex, float aspect)
{
	float2 uv = inputTex;
	uv.x *= aspect;

	if (distance(uv, float2(0.5f, aspect*0.5f)) > sin(Time.z))
	{
		fade1.a = 0.0f;
	}

	return fade1;
}

//二つの画像を合わせるフェード処理
float4 CrossFadeNormal(float4 fade1, float4 fade2)
{
	return lerp(fade1, fade2, sin(Time.x));
}
//別の画像の色情報を元に透明化するタイミングをずらす
float4 CrossFadeShift(float4 fade1, float alpha)
{
	fade1.a = alpha;
	return fade1;
}
//別の画像の色情報を元に透明化するタイミングをずらす（白色に変える）
float4 CrossFadeWhite(float4 fade1,float alpha)
{
	return lerp(float4(1, 1, 1, 1), fade1, alpha);
}

//別の画像の色情報を元に透明化するタイミングをずらす（黒色に変える）
float4 CrossFadeBlack(float4 fade1, float alpha)
{
	return lerp(float4(0, 0, 0, 0.5f), fade1, alpha);
}

//別の画像の色情報を元に透明化するタイミングをずらす（はっきりに変える）
float4 CrossFadeClearly(float4 fade1,float alpha)
{
	fade1.a = step(alpha, 0);
	return fade1;
}
//別の画像の色情報を元に透明化するタイミングをずらす（透明化した直後に白色にする）
float4 CrossFadeTipWhite(float4 fade1, float alpha)
{
	fade1 = lerp(float4(1, 1, 1, 1), fade1, step(alpha, 0));
	fade1 = lerp(float4(0, 0, 0, 0), fade1, step(alpha, 0.1f));
	return fade1;
}

//二つの画像を合わせるフェード処理
float4 CrossFade(float4 fade1, float4 fade2)
{
	//二つの画像を合わせるフェード
	float alpha = fade2.r - sin(Time.x);

	return CrossFadeBlack(fade1,alpha);
}

//ノイズフェード処理
//乱数
float random(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898f, 78.233f)))* 43758.5453f);
}
//メイン
float4 noise(float4 fade1, float2 uv)
{
	float2 p = floor(uv);
	float rand = random(p);
	float4 blockNoise = float4(rand, rand, rand, 1);

	return blockNoise;
}

//グレースケール
float4 gray(float4 fade1)
{
	float g = fade1.r * 0.3f + fade1.g * 0.6f + fade1.b * 0.1f;
	fade1 = float4(g, g, g, fade1.a);

	return fade1;
}

//パーリーノイズ
float perlinNoise(float2 uv)
{
	float2 p = floor(uv);
	float2 f = frac(uv);
	float2 u = f * f * (3.0f - 2.0f * f);
	float v00 = -1 + 2 * random(p + float2(0, 0));
	float v10 = -1 + 2 * random(p + float2(1, 0));
	float v01 = -1 + 2 * random(p + float2(0, 1));
	float v11 = -1 + 2 * random(p + float2(1, 1));

	return lerp(
		lerp(dot(v00, f - float2(0, 0)), dot(v10, f - float2(1, 0)), u.x),
		lerp(dot(v01, f - float2(0, 1)), dot(v11, f - float2(1, 1)), u.x),
		u.y); //+ 0.5f;
}

//FBM処理
float FBM(float2 uv)
{
	float f = 0;
	float2 q = uv;
	f += 0.5000f*perlinNoise(q); q = q * 2.01f;
	f += 0.2500f*perlinNoise(q); q = q * 2.02f;
	f += 0.2500f*perlinNoise(q);
	return f;
}

//ブラー
float4 Blur(float2 uv)
{
	float power = 0.1f;
	float center = 1.0f - power * 4.0f;

	float4 output = 0;
	for (int i = 0; i < 4; i++)
	{
		output += tex.Sample(samLinear, uv
			+ float2(((i % 3) - 1)*0.01f, ((i / 3) - 1)*0.01f)) * power;
	}
	output += tex.Sample(samLinear, uv) * center;

	return output;
}

//モーションブラー
float4 MotionBlur(float2 uv)
{
	float power = 0.1f;

	float4 output = 0;
	for (int i = 0; i < 9; i++)
	{
		output += tex.Sample(samLinear, uv
			+ float2(sin(Time.x - i*0.1f)*0.05f, 0.0f)) * power*(10.0f - i) / 5.0f;
	}

	return output;
}

//モザイク
float4 Mosaic(float2 baseUV, float mag)
{
	return tex.Sample(samLinear, floor(baseUV * mag) / mag);
}

/*
float4 oldTV(float2 inUV)
{
	float2 uv = inUV;
	//中心をずらす
	uv = uv - 0.5f; // uv - float2(0.5f,0.5f)
	float vignette = length(uv);
	uv /= 1 - vignette * 0.2f;
	//中心を戻す
	float2 texUV = uv + 0.5f;
	texUV.x += floor(texUV.y * 5) * 0.1f;
	float4 base = tex.Sample(samLinear, texUV);
	return base;
}
*/


float4 main(PS_INPUT input) : SV_TARGET
{
	float2 uv = input.Tex;
	uv.x *= (1280.0f / 720.0f);

	//表示サイズ
	float sizeX = 0.25f;
	float sizeY = 0.1f;

	//Hp x:最大HP  y:現在のHP  z:ダメージを受けたときの赤い部分  z:未使用(1)
	//sizeXをかけるのは表示サイズに合わせて小さくするため
	//現在のHP
	float nowHp = (Hp.y / Hp.x) * sizeX;
	//ダメージを受けたときの赤い部分
	float redZone = (Hp.z / Hp.x)* sizeX;
	//step(step(0.025f,input.Tex.x), step(0.975f, input.Tex.x))
	//step(step(0.025f,input.Tex.y), step(0.975f, input.Tex.y))
	//緑
	float4 green = float4(0.0f, 1.0f, 0.0f, 1.0f);
	//赤
	float4 red = float4(1.0f, 0.0f, 0.0f, 1.0f);
	//黒
	float4 black = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//returnで返す値
	//HPゲージ
	float4 HpGuage = lerp(green, lerp(red,black,step(nowHp+redZone, input.Tex.x)), step(nowHp,input.Tex.x));
	//HPゲージの表示位置に大きさを調整
	HpGuage.a = lerp(1, 0, step(step(input.Tex.x, sizeX), step(sizeY, input.Tex.y)));;

	return HpGuage;
}