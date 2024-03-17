# TunumCpp

C++20 以降向けの数値関連処理支援ヘッダーオンリーライブラリです。  
ほとんどの機能を定数式上で利用可能です。

主に、下記の機能が含まれます。

- 固定サイズ多倍長整数
- 数学関数
- 進数変換

## ドキュメント

TunumCpp の機能リファレンスのリンクです。  
https://twilightuncle.github.io/tunum-cpp/

## 使い方

任意の場所に本ライブラリを展開し、プロジェクトのインクルードディレクトリに下記パスを追加してください。

- `path/to/tunum-cpp/include`

そのうえで、ソースコードに下記を記載することで全ての機能を使用できます。

- `#include <tunum.hpp>`

全ての機能は、名前空間`tunum`において定義されています。

## 機能概要

### 固定サイズ多倍長整数 - fmpint

C++ 組み込みの整数型では扱うことのできないような、大きな数値を利用できるクラスです。  
基本的に、組み込みの整数型と同様に、四則演算、シフト演算、比較、ビット演算などを行うことができます。

ただし、現状は浮動小数点との変換などの実装は行っていません。

```cpp
#include <tunum.hpp>
using namespace tunum::literals;

// 符号なし64ビット整数で表現可能な最大値 + 1
constexpr auto unsigned_big_number = 18'446'744'073'709'551'616_ufmp;

// 大きな数値も組み込み整数と同じように使用可能
static_assert((unsigned_big_number * 2) == 36'893'488'147'419'103'232_ufmp);
static_assert((36'893'488'147'419'103'232_ufmp - unsigned_big_number) == unsigned_big_number);

constexpr auto signed_big_number = -18'446'744'073'709'551'616_fmp;
static_assert(unsigned_big_number + signed_big_number == 0);

int main() {}
```

## 動作確認環境

C++20を有効にした状態の、下記の環境/コンパイラにおいてコンパイルし、動作を確認しています。

-   Windows11
    -   Visual Studio 2022 付属の VC++
-   ubuntu 22.04 (Docker)
    -   GCC: 10, 11, 12  
    -   Clang: 12, 13, 14

## テストのビルドと実行
下記の通りコマンドを実行します。
以下で出現する`path/to/tunum-cpp`は、本ライブラリを展開したディレクトリのパスです。

```powershell
# 本ライブラリのソースを展開したディレクトリへ移動
cd path/to/tunum-cpp

# ビルド用ディレクトリ作成、移動
# ※mkdir は 初回のみ
mkdir build
cd build

# プロジェクトの作成
# ※初回のみ
cmake ..

# テストソースのビルド
cmake --build .

# テストの実行
ctest -C Debug
```

詳細なテスト結果は`path\to\tunum-cpp\build\Testing\Temporary\LastTest.log`に出力されます。  
テストが失敗した際の詳細を確認する場合は上記ファイルを参照ください。


