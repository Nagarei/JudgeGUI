#pragma once
#include "Data.h"
//メインスレッドでカレントディレクトリが変更される為
//相対パス使用禁止！！！

//結果の解析
//@param log_directory 出力するフォルダの「絶対パス」
//@param input_directory 入力があるフォルダの「絶対パス」
//@param cppfile_full_name 入力cppファイルの「絶対パス」
void RunTest(dxle::tstring log_directory, dxle::tstring input_directory, const dxle::tstring& cppfile_full_name);
//結果の解析
//@param log_directory 出力があるフォルダの「絶対パス」
Submission BuildScores(dxle::tstring log_directory, dxle::tstring user_name);
