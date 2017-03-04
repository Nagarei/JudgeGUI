# JudgeGUI

# ソース説明

* babel
  * 文字コード変換ライブラリ「babel」のコンパイルおよびユーティリティ
* GUI
  * childモード（本体）専用コード
  * button
    * 上にカーソルを乗せると色が変わったり、クリックできたりするボタン
  * Data
    * 問題一覧、問題文、成績データ（大本のDataはシングルトン）
    * Script
      * 問題文スクリプト解析
  * Key
    * キー入力処理一般 シングルトン
  * Mouse
    * マウス入力処理一般 シングルトン
  * OpenFolder
    * 「フォルダを開く」ダイアログの処理
  * popup
    * ポップアップ（右上のACとか出てくるやつ）の処理 シングルトン
  * ScrollBar
    * スクロールバーの処理
    * ScrollBar
      * 一次元スクロール
    * ScrollBar2
      * 二次元スクロール　拡大/縮小処理 スクロールに関するキー処理
  * Sequence
    * Contest
      * 問題一覧
    * Option
      * 起動時のモード選択画面
    * Score_detail
      * 提出詳細
    * Show_Score
      * 提出一覧
    * common
      * 問題名、点数表示　問題切り替え処理
  * SetClipboardText
    * クリップボードに文字を乗せる
  * GUI_userdata.h
    * テストキューに提出ソースと一緒に乗せる付加データ（GUI用）
* ModeSwitch
  * chil,parentの切り替え用コード(主に初期化処理）
* NetCommon
  * ネット通信部の共通コード
* NetParent
  * parent(サーバー親機)専用コード
* TEST
  * ジャッジ系のコード
  * Submission
    * ジャッジを実際に起動するコードと、ジャッジの結果を読み込むコード
  * test
    * ジャッジ待ちキュー

(C) 2016-2017 Nagarei
