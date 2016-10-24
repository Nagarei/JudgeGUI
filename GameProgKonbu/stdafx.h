// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
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
#include <Lmcons.h>
#include "DxLib.h"
#include "FormatToString.h"
#include "Sequence.h"
#include "KeyInputData.h"

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。

namespace dxle
{
	using pointi32 = dxle::point_c<int32_t>;
	using sizei32 = dxle::size_c<int32_t>;
}

#endif
