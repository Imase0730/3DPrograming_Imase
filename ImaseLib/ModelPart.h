//--------------------------------------------------------------------------------------
// File: ModelPart.h
//
// モデルのパーツ表示クラス
//
// Date: 2023.7.9
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

namespace Imase
{
	// モデルパーツクラス
	class ModelPart
	{
	protected:

		// モデルハンドル
		DirectX::Model* m_model;

		// 子へのポインタ
		ModelPart* m_child;

		// 兄弟へのポインタ
		ModelPart* m_sibling;

		// 初期位置行列
		DirectX::SimpleMath::Matrix m_initial;

		// 変換行列
		DirectX::SimpleMath::Matrix m_transform;

		// ワールド行列
		DirectX::SimpleMath::Matrix m_world;

		// 描画関数
		void Draw(
			ModelPart* parts,
			ID3D11DeviceContext* context,
			const DirectX::CommonStates& states,
			DirectX::SimpleMath::Matrix view,
			DirectX::SimpleMath::Matrix proj,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr
		)
		{
			if (parts->m_child) Draw(parts->m_child, context, states, view, proj, setCustomState);
			if (parts->m_sibling) Draw(parts->m_sibling, context, states, view, proj, setCustomState);
			if (parts->m_model) parts->m_model->Draw(context, states, parts->m_world, view, proj, false, setCustomState);
		}

		// 各パーツの初期位置行列を親の座標系に変換する関数
		void SetupMatrix(ModelPart* parts, const DirectX::SimpleMath::Matrix& parent)
		{
			if (parts->m_child) SetupMatrix(parts->m_child, parts->m_initial.Invert());
			if (parts->m_sibling) SetupMatrix(parts->m_sibling, parent);
			parts->m_initial *= parent;
		}

		// 各パーツのワールド行列を更新する関数
		void UpdateMatrix(ModelPart* parts, const DirectX::SimpleMath::Matrix& parent)
		{
			parts->m_world = parts->m_transform * parts->m_initial;
			parts->m_world *= parent;
			if (parts->m_child) UpdateMatrix(parts->m_child, parts->m_world);
			if (parts->m_sibling) UpdateMatrix(parts->m_sibling, parent);
		}

	public:

		// コンストラクタ
		ModelPart(DirectX::Model* model = nullptr)
			: m_model(model), m_child(nullptr), m_sibling(nullptr)
		{
		}

		// デストラクタ
		virtual ~ModelPart() {}

		// 子パーツの設定関数
		void SetChild(ModelPart* part) { m_child = part; }

		// 兄弟パーツの設定関数
		void SetSibling(ModelPart* part) { m_sibling = part; }

		// 初期位置行列の設定関数
		void SetInitialMatrix(DirectX::SimpleMath::Matrix inital) { m_initial = inital; }

		// 変換行列の設定関数
		void SetTransformMatrix(DirectX::SimpleMath::Matrix transform) { m_transform = transform; }

		// 連結されているパーツの初期位置行列を親の座標系に全て変換する関数
		void SetupMatrix() { SetupMatrix(this, DirectX::SimpleMath::Matrix::Identity); }

		// 連結されているパーツのワールド行列を全て更新する関数
		void UpdateMatrix(DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity)
		{
			UpdateMatrix(this, world);
		}

		// 描画関数
		void Draw(
			ID3D11DeviceContext* context,
			const DirectX::CommonStates& states,
			DirectX::SimpleMath::Matrix view,
			DirectX::SimpleMath::Matrix proj,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr
		)
		{
			Draw(this, context, states, view, proj, setCustomState);
		}

		// ワールド行列を取得する関数
		const DirectX::SimpleMath::Matrix& GetWorldMatrix() { return m_world; }

		// モデルへのポインタを取得する関数
		DirectX::Model* GetModel() { return m_model; }

	};
}

