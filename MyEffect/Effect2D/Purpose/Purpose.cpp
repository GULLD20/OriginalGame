#include "../../../pch.h"
#include "Purpose.h"
#include <WICTextureLoader.h>
#include "../../BinaryFile.h"
#include "d3d11.h"
#include <Effects.h>

#include "../../../Aspect.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

const std::vector<D3D11_INPUT_ELEMENT_DESC> Purpose::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vector3) + sizeof(Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
void Purpose::Create(DX::DeviceResources* &deviceResources, const wchar_t* name, int count)
{
	m_pDeviceResources = deviceResources;
	auto device = m_pDeviceResources->GetD3DDevice();

	//const wchar_t* name = L"Resources\\Textures\\image01.png";
	DirectX::CreateWICTextureFromFile(deviceResources->	GetD3DDevice(), name, nullptr, m_pTexture.GetAddressOf());
	DirectX::CreateWICTextureFromFile(deviceResources->GetD3DDevice(), L"Resources//Textures//hight.jpg", nullptr, m_pTexture2.GetAddressOf());

	// コンパイルされたシェーダファイルを読み込み
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleGS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticlePSPurpose.cso");

	device->CreateInputLayout(&INPUT_LAYOUT[0],
		INPUT_LAYOUT.size(),
		VSData.GetData(), VSData.GetSize(),
		m_pInputLayout.GetAddressOf());
	// 頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_pVertexShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}
	// ジオメトリシェーダ作成
	if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_pGeometryShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}
	// ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pPixelShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}
	// プリミティブバッチの作成
	m_pBatch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(m_pDeviceResources->GetD3DDeviceContext());

	m_pStates = std::make_unique<CommonStates>(device);



	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_pBuffer);
	m_pFxFactory = std::make_unique<EffectFactory>(device);
	m_pFxFactory->SetDirectory(L"Resources");

	//m_model = Model::CreateFromCMO(device, L"Resources/cup.cmo", *m_pFxFactory);

	for (int i = 0; i < count; i++) {
		MyEffect* effect = new MyEffect();
		m_pEffectList.push_back(effect);
	}

}
void Purpose::Lost() {
	for (std::list<MyEffect*>::iterator itr = m_pEffectList.begin(); itr != m_pEffectList.end(); itr++)
	{
		delete (*itr);
		*itr = nullptr;
	}

	m_pTexture.Reset();
	m_pTexture2.Reset();

	m_pBuffer.Reset();
	m_pStates.reset();

	// 頂点シェーダ
	m_pVertexShader.Reset();
	// ピクセルシェーダ
	m_pPixelShader.Reset();
	// ジオメトリシェーダ
	m_pGeometryShader.Reset();

	// プリミティブバッチ
	m_pBatch.reset();
	// 入力レイアウト
	m_pInputLayout.Reset();
	m_vertexList.clear();

	m_pFxFactory.reset();
	//std::unique_ptr<DirectX::Model> m_model;


	m_pCapture.Reset();
	m_pRtv.Reset();
	m_pSrv.Reset();

	m_keyStateTracker.Reset();
}
void Purpose::Initialize(float life, const DirectX::SimpleMath::Vector3 &pos)
{
	int range = 100;
	for (std::list<MyEffect*>::iterator itr = m_pEffectList.begin(); itr != m_pEffectList.end(); itr++)
	{
		Vector3 vel = Vector3::Zero;
		(*itr)->Initialize(life,pos,vel);
	}
	//中心位置の初期化
	m_centerPosition = pos;
	//プレイシーンの合計時間の初期化
	m_playTotalTime = 0;
}
void Purpose::InitializeNormal(float life, const DirectX::SimpleMath::Vector3 &pos)
{
	int range = 1000;
	for (std::list<MyEffect*>::iterator itr = m_pEffectList.begin(); itr != m_pEffectList.end(); itr++)
	{
		Vector3 vel = Vector3::Zero;
		(*itr)->Initialize(life, pos, vel);
	}
	//中心位置の初期化
	m_centerPosition = pos;
	//プレイシーンの合計時間の初期化
	m_playTotalTime = 0;
}
void Purpose::InitializeCorn(float life, const DirectX::SimpleMath::Vector3 &pos, const DirectX::SimpleMath::Vector3 &dir)
{
	int range = 10;
	Vector3 vel = Vector3::Zero;
	for (std::list<MyEffect*>::iterator itr = m_pEffectList.begin(); itr != m_pEffectList.end(); itr++)
	{
		Vector3 vel = Vector3(
			(((rand() % (range * 2)) - range)*0.1f / range) + dir.x,
			(((rand() % (range * 2)) - range)*0.1f / range) + dir.y,
			(((rand() % (range * 2)) - range)*0.1f / range) + dir.z);
		vel.Normalize();
		vel *= 0.1f;

		(*itr)->Initialize(life, pos, vel);
	}
	//中心位置の初期化
	m_centerPosition = pos;
	//プレイシーンの合計時間の初期化
	m_playTotalTime = 0;
}
void Purpose::Update(const DX::StepTimer &timer)
{
	m_timer = timer;
	for(std::list<MyEffect*>::iterator itr = m_pEffectList.begin(); itr != m_pEffectList.end();itr++)
	{
		(*itr)->Update(timer);
	}
	m_keyStateTracker.Update(DirectX::Keyboard::Get().GetState());
	if (m_keyStateTracker.IsKeyPressed(Keyboard::Up)) m_menuId = (m_menuId+3 - 1) % 3;
	if (m_keyStateTracker.IsKeyPressed(Keyboard::Down)) m_menuId = (m_menuId + 1) % 3;
	if (m_keyStateTracker.IsKeyPressed(Keyboard::Left)) m_colorMode = (m_colorMode+4 - 1) % 4;
	if (m_keyStateTracker.IsKeyPressed(Keyboard::Right)) m_colorMode = (m_colorMode + 1) % 4;
}

void Purpose::Render()
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();



	m_vertexList.clear();
	//マネージャで管理しているエフェクト分イテレータを回す
	for (auto itr = m_pEffectList.begin(); itr != m_pEffectList.end(); itr++)
	{
		//エフェクトの頂点の座標と速度を取得する
		Vector3 pos = (*itr)->GetPosition();
		Vector3 vel = (*itr)->GetVelocity();

		{
			//取得した座標を登録する
			VertexPositionColorTexture vertex;
			//vertex = VertexPositionColorTexture(pos, Vector4(vel.x,vel.y,vel.z,1), Vector2(0.0f,3.0f));
			vertex = VertexPositionColorTexture(pos, Vector4::Zero, Vector2::Zero);
			m_vertexList.push_back(vertex);
		}
	}

	//回転するモデルを表示
	static float rot = 0.0f;
	rot += 0.1f;
	//m_model->Draw(m_pDeviceResources->GetD3DDeviceContext(), *m_pStates, Matrix::CreateRotationZ(rot), m_view, m_proj);

	//全画面エフェクト
	Matrix  mat = Matrix::Identity;
	Draw(mat, mat, mat);

	//板ポリゴンエフェクト

	//Draw(m_billboardTranslation,m_view, m_proj);

}

void Purpose::SetRenderState(const DirectX::SimpleMath::Vector3 &camera, const DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj)
{
	m_view = view;
	m_proj = proj;

	m_billboardTranslation = Matrix::CreateBillboard(m_centerPosition, camera, Vector3::UnitY);

	//ビルボードの計算で裏返るので補正
	//Y軸で180度回転する行列
	Matrix rev = Matrix::Identity;
	rev._11 = -1;
	rev._33 = -1;

	//補正行列を先にかけて他に影響がないようにする
	m_billboardTranslation =rev * m_billboardTranslation;

}

void Purpose::Draw(const DirectX::SimpleMath::Matrix &world, const DirectX::SimpleMath::Matrix &view, const DirectX::SimpleMath::Matrix &proj)
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	//定数バッファで渡す値の設定
	ConstBuffer cbuff;
	cbuff.matView = view.Transpose();
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = world.Transpose();

	//追加処理-------
	float aspectX = static_cast<float>(Aspect::ASPECT_X);
	float aspectY = static_cast<float>(Aspect::ASPECT_Y);
	//---------------

	//Time		x:経過時間(トータル秒)	y:1Fの経過時間(秒）	z:反復（サインカーブ） w:アスペクト比
	cbuff.Time = Vector4(m_playTotalTime, float(m_timer.GetElapsedSeconds()), sinf(float(m_timer.GetTotalSeconds())), (aspectX/ aspectY));
	DirectX::Mouse::State ms = DirectX::Mouse::Get().GetState();
	cbuff.Mouse = Vector4(ms.x/800.0f, ms.y/600.0f, ms.leftButton, ms.rightButton);
	cbuff.Mode = Vector4(static_cast<float>(m_menuId), static_cast<float>(m_colorMode), static_cast<float>(1), static_cast<float>(1));


	//定数バッファの内容更新
	context->UpdateSubresource(m_pBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	ID3D11BlendState* blendstate = m_pStates->NonPremultiplied();
	// 透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
	// 深度バッファは参照のみ
	context->OMSetDepthStencilState(m_pStates->DepthRead(), 0);
	// カリングは反時計回り
	context->RSSetState(m_pStates->CullCounterClockwise());

	//定数バッファをシェーダに渡す（とりあえずPSは要らないのでコメントアウト）
	ID3D11Buffer* cb[1] = { m_pBuffer.Get() };
	//context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);


	//サンプラー、シェーダ、画像をそれぞれ登録
	ID3D11SamplerState* sampler[1] = { m_pStates->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);
	context->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	context->GSSetShader(m_pGeometryShader.Get(), nullptr, 0);
	context->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());
	context->PSSetShaderResources(1, 1, m_pTexture2.GetAddressOf());
	context->PSSetShaderResources(2, 1, m_pTexture2.GetAddressOf());

	//入力レイアウトを反映
	context->IASetInputLayout(m_pInputLayout.Get());

	//バッチに頂点情報を渡す
	m_pBatch->Begin();
	//m_pBatch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_pBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &m_vertexList[0], m_vertexList.size());
	m_pBatch->End();


	//他のモデルに影響が出る可能性があるので使い終わったらシェーダを外す
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}

//モデル等をテクスチャ(m_pSrv)に描画する処理
void Purpose::RenderModel()
{
	auto context = m_pDeviceResources->GetD3DDeviceContext();

	D3D11_TEXTURE2D_DESC texDesc;
	m_pDeviceResources->GetRenderTarget()->GetDesc(&texDesc);
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_pDeviceResources->GetD3DDevice()->CreateTexture2D(&texDesc, NULL, m_pCapture.ReleaseAndGetAddressOf());

	// レンダーターゲットビューの設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	// レンダーターゲットビューの生成
	m_pDeviceResources->GetD3DDevice()->CreateRenderTargetView(m_pCapture.Get(), &rtvDesc, &m_pRtv);

	//シェーダリソースビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	//レンダーターゲットビュー,深度ビューを取得（後で元に戻すため）
	ID3D11RenderTargetView* defaultRTV = m_pDeviceResources->GetRenderTargetView();
	ID3D11DepthStencilView* pDSV = m_pDeviceResources->GetDepthStencilView();

	//背景色の設定（アルファを０にするとオブジェクトのみ表示）
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };

	//レンダーターゲットビューをセットし、初期化する
	context->OMSetRenderTargets(1, m_pRtv.GetAddressOf(), pDSV);
	context->ClearRenderTargetView(m_pRtv.Get(), clearColor);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//----------------------------------------------------------------------------
	//とりあえずの動きのために回転
	static float rot = 0.0f;
	rot += 0.1f;

	//モデルを描画
	//m_model->Draw(m_pDeviceResources->GetD3DDeviceContext(), *m_pStates, Matrix::CreateRotationZ(rot), m_view, m_proj);

	//描画した画面をm_pSrvに保存
	auto hr = m_pDeviceResources->GetD3DDevice()->CreateShaderResourceView(
		m_pCapture.Get(), &srvDesc, m_pSrv.ReleaseAndGetAddressOf());

	//------------------------------------------------------------------
	//設定をもとに戻す
	clearColor[0] = 0.3f;
	clearColor[1] = 0.3f;
	context->OMSetRenderTargets(1, &defaultRTV, pDSV);
	context->ClearRenderTargetView(defaultRTV, clearColor);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);


}