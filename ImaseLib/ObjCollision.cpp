//--------------------------------------------------------------------------------------
// File: CollisionMesh.cpp
//
// Obj形式のメッシュをコリジョンにするクラス
//
// Date: 2018.7.11
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"
#include "ObjCollision.h"
#include <fstream>

using namespace DirectX;
using namespace Imase;

// コンストラクタ
ObjCollision::ObjCollision(const wchar_t* fname)
{
	// obj形式のファイル読み込み
	std::ifstream ifs(fname);

	std::string str;
	while (getline(ifs, str))
	{
		// 頂点
		if (str[0] == 'v')
		{
			VertexPosition val = {};
			sscanf_s(str.data(), "v  %f %f %f", &val.position.x, &val.position.y, &val.position.z);
			m_vertexes.push_back(val);
		}
		// インデックス
		if (str[0] == 'f')
		{
			int a, b, c;
			sscanf_s(str.data(), "f %d %d %d", &a, &b, &c);
			// 三角形の頂点インデックス番号が同じ物を含む場合は無視する
			if (a != b && a != c && b != c)
			{
				m_indexes.push_back(static_cast<uint16_t>(a - 1));
				m_indexes.push_back(static_cast<uint16_t>(c - 1));
				m_indexes.push_back(static_cast<uint16_t>(b - 1));
			}
		}
	}
	ifs.close();

	// 三角形リストに登録
	for (size_t i = 0; i < m_indexes.size() / 3; i++)
	{
		AddTriangle(m_vertexes[m_indexes[i * 3]].position, m_vertexes[m_indexes[i * 3 + 1]].position, m_vertexes[m_indexes[i * 3 + 2]].position);
	}
}

// コリジョン用三角形データの追加関数
void ObjCollision::AddTriangle(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c)
{
	Imase::Collision::Triangle t(a, b, c);
	m_triangles.push_back(t);
}

// コリジョン情報の更新
void ObjCollision::UpdateBoundingInfo(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotate)
{
	m_position = position;
	m_rotation = rotate;
}

// 線分との交差判定関数
bool ObjCollision::IntersectLineSegment(
	DirectX::SimpleMath::Vector3 a,
	DirectX::SimpleMath::Vector3 b,
	DirectX::SimpleMath::Vector3* hitPosition,
	DirectX::SimpleMath::Vector3* normal
)
{
	// 線分に逆行列を掛ける
	SimpleMath::Matrix rotate = SimpleMath::Matrix::CreateFromQuaternion(m_rotation);
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_position);
	SimpleMath::Matrix world = rotate * trans;
	SimpleMath::Matrix matInvert = world.Invert();
	a = SimpleMath::Vector3::Transform(a, matInvert);
	b = SimpleMath::Vector3::Transform(b, matInvert);

	for (int i = 0; i < m_triangles.size(); i++)
	{
		if (Imase::Collision::IntersectSegmentTriangle(a, b, m_triangles[i], hitPosition) == true)
		{
			// 衝突位置をワールド座標系へ変換
			*hitPosition = SimpleMath::Vector3::Transform(*hitPosition, world);
			// 衝突した三角形の法線情報を取得
			if (normal) *normal = SimpleMath::Vector3::Transform(m_triangles[i].p.Normal(), rotate);
			return true;
		}
	}
	return false;
}

void ObjCollision::AddDisplayCollision(Imase::DisplayCollision* displayCollision, DirectX::FXMVECTOR lineColor) const
{
	displayCollision->AddBoundingVolume(m_vertexes, m_indexes, m_position, m_rotation, lineColor);
}
