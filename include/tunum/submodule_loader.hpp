#ifndef TUNUM_INCLUDE_GUARD_TUNUM_SUBMODULE_LOADER_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_SUBMODULE_LOADER_HPP

// ----------------------------------------
// サブモジュールで管理する依存ライブラリ管理用
// 本ファイル内で本ライブラリのファイルを読み込んではならない
// ----------------------------------------

// 各依存ライブラリ内のインクルードパス設定
#ifndef TUMP_V_0_1_1_COMMON_INCLUDE
#define TUMP_V_0_1_1_COMMON_INCLUDE(path) <tump-cpp/include/tump/path>
#endif

// 依存ライブラリインクルード
#include <tump-cpp/include/tump.hpp>

// tunum内部用、依存ライブラリのエイリアス
namespace tunum
{
    namespace tump = tump_0_1_1;
}

#endif
