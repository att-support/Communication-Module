// ServerApp.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include "ServerMain.h"

int main(int argc,      // Number of strings in array argv  
	char *argv[],   // Array of command-line argument strings  
	char *envp[])  // Array of environment variable strings  
{
	printf("ServerApp Start!\n");
	//if (argc != 2) return 1;

	std::vector<ServerMain*> servers;

	for (int i = 0; i < 5; i++) {
		servers.push_back(new ServerMain(0));
		servers.push_back(new ServerMain(1));
		servers.push_back(new ServerMain(2));
		servers.push_back(new ServerMain(3));

		for (std::vector<ServerMain*>::iterator itr = servers.begin(); itr != servers.end(); itr++) {
			(*itr)->TestStart();
		}
		std::string inputChar;
		do
		{
			std::cout << "Input 'q' if you terminate app." << std::endl;
			std::cin >> inputChar;
			if (inputChar == "q") {
				break;
			}
		} while (1);
		for (std::vector<ServerMain*>::iterator itr = servers.begin(); itr != servers.end(); itr++) {
			//(*itr)->WaitThreadEnd();
			(*itr)->TestStop();
		}
		for (std::vector<ServerMain*>::iterator itr = servers.begin(); itr != servers.end(); itr++) {
			delete (*itr);
		}
		servers.clear();
	}
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
