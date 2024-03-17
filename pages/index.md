---
title: TunumCppについて
overview: TunumCpp は、C++20以降を対象とした、コンパイル時の数値関連の処理を扱うヘッダオンリーライブラリです。
---

## はじめに

TunumCpp は数値の変換や数学関数など、数値を扱うことを支援するライブラリです。  
ほとんどの機能は定数式上で使用可能です。

下記のような機能が含まれます。

- [{固定サイズ多倍長整数 - fmpint|ref/fmpint}]
- 数学関数
- 進数変換

## インストール

[GitHub のリポジトリ](https://github.com/TwilightUncle/tunum-cpp)より、ソースを取得の上、任意の場所に展開します。

本ライブラリを使用するプロジェクトのインクルードディレクトリに、下記パスを追加します。  
※ソースの展開先を`path/to/tunum-cpp`とする

- `path/to/tunum-cpp/include`

そのうえで、ソースコードに下記を記載することですべての機能を呼び出すことができます。

- `#include <tunum.hpp>`

本ライブラリの機能は名前空間`tump`に定義されています。

<!-- より詳細なインクルードパスの制御方法については、[{TUNUM_COMMON_INCLUDEの項|ref/macro}]を参照ください。 -->

## 動作確認環境

C++20を有効にした状態の、下記の環境/コンパイラにおいてコンパイルし、動作を確認しています。

- Windows11
    - Visual Studio 2022 付属の VC++
- ubuntu 22.04 (Docker)
    - GCC: 10, 11, 12
    - Clang: 12, 13, 14
