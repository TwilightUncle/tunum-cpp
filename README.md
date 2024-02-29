# tunum-cpp

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


