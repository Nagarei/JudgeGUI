﻿// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
// VC++の設定によりファイルの先頭で自動的に読み込まれます。

#ifndef STDAFX_H_201505121652_1_6_5_8_
#define STDAFX_H_201505121652_1_6_5_8_

#include "dxlibex/graph2d.hpp"
#include "dxlibex/color.hpp"
#include <Windows.h>
#include <boost/utility.hpp>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <cstdint>
#include <numeric>
#include <Lmcons.h>
#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib")
#include "DxLib.h"
#include "FormatToString.h"
#include "Sequence.h"
#include "KeyInputData.h"

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。

namespace dxle
{
	using pointi32 = dxle::point_c<int32_t>;
	using pointui32 = dxle::point_c<uint32_t>;
	using sizei32 = dxle::size_c<int32_t>;
	using sizeui32 = dxle::size_c<uint32_t>;
	inline int DrawBox(dxle::pointi pos1, dxle::pointi pos2, dxle::dx_color_param color, bool fill_flag) {
		return DxLib::DrawBox(pos1.x, pos1.y, pos2.x, pos2.y, color.get(), fill_flag);
	}
}

#endif
