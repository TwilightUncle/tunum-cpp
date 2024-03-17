---
title: 固定サイズ多倍長整数リテラル - TunumCpp リファレンス
---

### リテラルの接頭詞

| prefix |　概要 |
| --- | --- |
| なし | 10 進数リテラル |
| `0b`, `0B` | 2 進数リテラル |
| `0` | 8 進数リテラル |
| `0x`, `0X` | 16 進数リテラル |

### リテラルの接尾語

| suffix | 概要 |
| --- | --- |
| `_fmp` | 符号ありの固定サイズ多倍長整数リテラル |
| `_ufmp` | 符号なしの固定サイズ多倍長整数リテラル |

## 概要

TunumCpp では[{`tunum::fmpint`|ref/fmpint}]を構築するユーザー定義リテラルを定義しています。  
符号ありの[{`tunum::fmpint`|ref/fmpint}]を構築したい場合は数値リテラルの末尾に`_fmp`を、符号なしの[{`tunum::fmpint`|ref/fmpint}]を構築したい場合は`_ufmp`を指定します。

リテラルの接頭詞については、組み込みの整数リテラルと同様に、`0` または、`0`と特定のアルファベットを指定することで、任意の進数表記で[{`tunum::fmpint`|ref/fmpint}]を構築できます。

また、組み込みの整数リテラルと同様に、シングルクオテーションマークによる区切り表記が可能です。  
ただし、下記のいずれかに該当するようにシングルクオテーションマークを記述すると、コンパイルエラーとなります。

- リテラルの先頭にシングルクオテーションマークが存在する
- prefix の直後にシングルクオテーションマークが存在する
- シングルクオテーションマークが 2 回以上連続で出現する

## 固定サイズ多倍長整数リテラルのサイズ

suffix へ`_fmp`または、`_ufmp`を付与した整数リテラルは、リテラルの桁数から表現可能なサイズを算出し、自動的に適切な`Bytes`の[{`tunum::fmpint`|ref/fmpint}]を構築します。

## 固定サイズ多倍長整数リテラルと符合

リテラルの前方に`+`や`-`を記載しても、整数リテラルに符号は含まれません。  
いったん正の整数として[{`tunum::fmpint`|ref/fmpint}]を構築したのち、単項演算子として適用が行われます。

## 例

```cpp
#include <tunum.hpp>
using namespace tunum::literals;

// v1 と v2 は全く同じ符号ありの fmpint インスタンスを構築する
constexpr auto v1 = 12345_fmp;
constexpr auto v2 = 12'345_fmp;

// v1 と v2 は全く同じ符号なしの fmpint インスタンスを構築する
constexpr auto v3 = 12345_ufmp;
constexpr auto v4 = 12'345_ufmp;

// 2 進数リテラル。0b と 0B で結果に違いはない
constexpr auto v5 = 0b1111'0000'1100_ufmp;
constexpr auto v6 = 0B1111'0000'1100_ufmp;

// 8 進数リテラル
constexpr auto v7 = 037'7777'7777_fmp;

// 16進数リテラル。prefix の x や a ~ f は大文字と小文字で結果に違いはない
constexpr auto v8 = 0xFFFF'FFFF'FFFF'FFFF_ufmp;
constexpr auto v9 = 0Xffff'ffff'ffff'ffff_ufmp;

// 符号なし64ビット整数で表現できないサイズの整数リテラルも可能
constexpr auto v10 = 340'282'366'920'938'463'463'374'607'431'768'211'455_ufmp;

// 以下は全てエラーのため、コメントアウトを外すとコンパイルエラーが発生する
// constexpr auto v11 = 088888_fmp;
// constexpr auto v12 = 0b22222_fmp;
// constexpr auto v13 = 0x'FFFFFF_fmp;
// constexpr auto v14 = 1234''5_fmp;
// constexpr auto v15 = 1234uio9_fmp;

int main() {}
```

## 関連リンク

- [{`tunum::fmpint`|ref/fmpint}]
- [{`tunum::fmpint`のコンストラクタ(文字列による構築)|ref/fmpint/constructor}]
