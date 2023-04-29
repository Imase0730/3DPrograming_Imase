//--------------------------------------------------------------------------------------
// File: ResourceManager.h
//
// リソースを管理するクラス
//
// Date: 2023.4.27
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

#include <unordered_map>
#include <vector>
#include <string>

namespace Imase
{
	class ResourceManager
	{
	private:

		// リソースリスト
		std::unordered_map<std::wstring, std::vector<uint8_t>> m_list;

	public:

		// データのロード
		bool LoadData(const wchar_t* name);

		// データの解放
		bool ReleaseData(const wchar_t* name);

		// データの取得
		std::vector<uint8_t> GetData(const wchar_t* name);

	};
}
