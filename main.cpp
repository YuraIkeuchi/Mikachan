#include "Dxlib.h"
#include <algorithm> // std::maxを使用するため
#include<malloc.h>

const char TITLE[] = "223_見返りみかちゃん";

const int WIN_WIDTH = 1920; //ウィンドウ横幅
const int WIN_HEIGHT = 1080;//ウィンドウ縦幅

//矩形当たり判定
int collision(float ax1, float ay1, float ax2, float ay2, float bx1, float by1, float bx2, float by2) {
	if (bx1 > ax2) { return false; }
	if (ax1 > bx2) { return false; }
	if (by1 > ay2) { return false; }
	if (ay1 > by2) { return false; }
	return true;
}

const int MAX_MAPCHIPY = 18;
const int MAX_MAPCHIPX = 300;

int** map;
//int** map2;
//ブロックサイズ
int blocksize = 64;

//シーン変数
int scene = 0;

//マップ番号
int mapNumber = 0;

//モンスターの総数(スライム)
const int SLIME_MAX = 6;

//モンスターの総数(コウモリ)
const int BAT_MAX = 6;

//モンスターの総数(ゴースト)
const int GORST_MAX = 6;

// マップチップの値を取得する関数
int GetChipParam(float pPosX, float pPosY);

// キャラクタをマップとの当たり判定を考慮しながら移動する関数
int CharMove(float* X, float* Y, float* DownSP,
	float MoveX, float MoveY, float Size, char* JumpFlag);

// 注意：MoveX と MoveY 、どっちか片方が０じゃないとまともに動作しません(爆)
int MapHitCheck(float pPosX, float pPosY, float* MoveX, float* MoveY);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(FALSE);						//ウィンドウモードに設定
	//ウィンドウサイズを手動では変更できず、かつウィンドウサイズに合わせて拡大できないようにする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);
	SetMainWindowText(TITLE);					// タイトルを変更
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);	//画面サイズの最大サイズ、カラービット数を設定（モニターの解像度に合わせる）
	SetWindowSizeExtendRate(1.0);				//画面サイズを設定（解像度との比率で設定）
	SetBackgroundColor(0x00, 0x00, 0x00);		// 画面の背景色を設定する

	//Dxライブラリの初期化
	if (DxLib_Init() == -1) { return -1; }

	//（ダブルバッファ）描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	//画像などのリソースデータの変数宣言と読み込み

	//ゲームループで使う変数の宣言
	char keys[256] = { 0 }; //最新のキーボード情報用
	char oldkeys[256] = { 0 };//1ループ（フレーム）前のキーボード情報
	map = (int**)malloc(sizeof(int*) * MAX_MAPCHIPY);
	map[0] = (int*)malloc(MAX_MAPCHIPY * MAX_MAPCHIPX * sizeof(int));
	for (int i = 0; i < MAX_MAPCHIPY; i++) {
		map[i] = map[0] + i * MAX_MAPCHIPX;
	}

	//マップ読み込み
	FILE* fp;
	FILE* fp2;
	FILE* fp3;
	FILE* fp4;
	FILE* fp5;

	fopen_s(&fp, "GameJamStage1 - シート1.csv", "r");
	fopen_s(&fp2, "GameJamStage2 - シート1.csv", "r");
	fopen_s(&fp3, "GameJamStage3 - シート1.csv", "r");
	fopen_s(&fp4, "GameJamStage4 - シート1.csv", "r");
	fopen_s(&fp5, "GameJamStage5 - シート1.csv", "r");

	if (mapNumber == 0) {
		for (int i = 0; i < MAX_MAPCHIPY; i++) {
			for (int j = 0; j < MAX_MAPCHIPX; j++) {
				fscanf_s(fp, "%d,", &map[i][j]);
			}
		}
		fclose(fp);
	}
	else if (mapNumber == 1) {
		fclose(fp);
		for (int i = 0; i < MAX_MAPCHIPY; i++) {
			for (int j = 0; j < MAX_MAPCHIPX; j++) {
				fscanf_s(fp2, "%d,", &map[i][j]);
			}
		}
		fclose(fp2);
	}
	else if (mapNumber == 2) {
		fclose(fp2);
		for (int i = 0; i < MAX_MAPCHIPY; i++) {
			for (int j = 0; j < MAX_MAPCHIPX; j++) {
				fscanf_s(fp3, "%d,", &map[i][j]);
			}
		}

		fclose(fp3);
	}
	else if (mapNumber == 3) {
		fclose(fp3);
		for (int i = 0; i < MAX_MAPCHIPY; i++) {
			for (int j = 0; j < MAX_MAPCHIPX; j++) {
				fscanf_s(fp4, "%d,", &map[i][j]);
			}
		}
		fclose(fp4);
	}
	else if (mapNumber == 4) {
		fclose(fp4);
		for (int i = 0; i < MAX_MAPCHIPY; i++) {
			for (int j = 0; j < MAX_MAPCHIPX; j++) {
				fscanf_s(fp5, "%d,", &map[i][j]);
			}
		}
		fclose(fp5);
	}

	//プレイヤーの変数
	float PlayerPosX = 300.0f; //プレイヤーの初期座標
	float PlayerPosY = 700.0f;//プレイヤーの初期座標
	int PlayerR = 50;//半径
	float PlayerspeedX = 8.0f;	//プレイヤーの移動力
	float PlayerG = 0.0f;// プレイヤーの落下速度
	char PlayerJumpFlag;// プレイヤーがジャンプ中か、のフラグ
	PlayerJumpFlag = FALSE;
	float PlayerMoveX;//プレイヤーの移動量
	float PlayerMoveY;//プレイヤーの移動量
	int playerDirection = 0;//プレイヤーの向いてる向き	
	int PlayerAlive = 1;//プレイヤーの生存フラグ
	int PlayerTimer = 0;//アニメーション関係
	int PlayerCount = 0;
	int DeathTimer = 0;//死んだ時のアニメーション
	int DeathCount = 0;
	int right = 0;//照らす強さ
	float LeftTimer = 10.0f;//左向ける時間
	int scrollX;//スクロール値
	scrollX = PlayerPosX - 960;

	//スライムの変数
	float SlimePosX[SLIME_MAX];//位置
	float SlimePosY[SLIME_MAX];
	int SlimeR[SLIME_MAX];//半径
	int SlimeAlive[SLIME_MAX];//生存フラグ
	int SlimeTimer[SLIME_MAX];//アニメーション関係
	int SlimeCount[SLIME_MAX];
	float SlimeG[SLIME_MAX];// スライムの落下速度
	float SlimeMoveX[SLIME_MAX];//スライムの移動量
	float SlimeMoveY[SLIME_MAX];
	char SlimeJumpFlag[SLIME_MAX];// スライムがジャンプ中か、のフラグ
	int SlimeMoveFlag[SLIME_MAX];//度の向きに動くかのフラグ

	for (int i = 0; i < SLIME_MAX; i++) {
		SlimePosX[i] = 0.0f;
		SlimePosY[i] = 0.0f;
		SlimeR[i] = 40;
		SlimeAlive[i] = 0;
		SlimeTimer[i] = 0;
		SlimeCount[i] = 0;
		SlimeG[i] = 0.0f;
		SlimeJumpFlag[i] = FALSE;
		SlimeMoveFlag[i] = 0;
	}

	//スライムの位置の初期化
	if (mapNumber == 0) {
		SlimeAlive[0] = 1;
		SlimeAlive[1] = 1;
		SlimePosX[0] = 1200.0f;
		SlimePosX[1] = 5300.0f;
		SlimePosY[0] = 600.0f;
		SlimePosY[1] = 700.0f;
	}

	else if (mapNumber == 1) {
		SlimeAlive[0] = 1;
		SlimeAlive[1] = 1;
		SlimePosX[0] = 1900.0f;
		SlimePosX[1] = 4700.0f;
		SlimePosY[0] = 900.0f;
		SlimePosY[1] = 600.0f;
	}

	else if (mapNumber == 2) {
		SlimeAlive[0] = 1;
		SlimeAlive[1] = 1;
		SlimePosX[0] = 2800.0f;
		SlimePosX[1] = 7800.0f;
		SlimePosY[0] = 900.0f;
		SlimePosY[1] = 550.0f;
	}

	else if (mapNumber == 3) {
		SlimeAlive[0] = 1;
		SlimeAlive[1] = 1;
		SlimeAlive[2] = 1;
		SlimeAlive[3] = 1;
		SlimeAlive[4] = 1;
		SlimeAlive[5] = 1;
		SlimePosX[0] = 1050.0f;
		SlimePosX[1] = 5000.0f;
		SlimePosX[2] = 8900.0f;
		SlimePosX[3] = 9300.0f;
		SlimePosX[4] = 10000.0f;
		SlimePosX[5] = 10500.0f;
		SlimePosY[0] = 850.0f;
		SlimePosY[1] = 700.0f;
		SlimePosY[2] = 750.0f;
		SlimePosY[3] = 750.0f;
		SlimePosY[4] = 750.0f;
		SlimePosY[5] = 750.0f;
	}

	else if (mapNumber == 4) {
		SlimeAlive[0] = 1;
		SlimeAlive[1] = 1;
		SlimeAlive[2] = 1;
		SlimeAlive[3] = 1;
		SlimeAlive[4] = 1;
		SlimeAlive[5] = 1;
		SlimePosX[0] = 650.0f;
		SlimePosX[1] = 6400.0f;
		SlimePosX[2] = 7400.0f;
		SlimePosX[3] = 11600.0f;
		SlimePosX[4] = 13278.0f;
		SlimePosX[5] = 12318.0f;
		SlimePosY[0] = 100.0f;
		SlimePosY[1] = 600.0f;
		SlimePosY[2] = 360.0f;
		SlimePosY[3] = 600.0f;
		SlimePosY[4] = 200.0f;
		SlimePosY[5] = 100.0f;
	}

	//コウモリの変数
	float BatPosX[BAT_MAX];//位置
	float BatPosY[BAT_MAX];
	int BatR[BAT_MAX];//半径
	int BatAlive[BAT_MAX];//生存フラグ
	int BatTimer[BAT_MAX];//アニメーション関係
	int BatCount[BAT_MAX];
	float BatG[BAT_MAX];// コウモリの落下速度
	float BatMoveX[BAT_MAX];//コウモリの移動量
	float BatMoveY[BAT_MAX];
	char BatJumpFlag[BAT_MAX];// コウモリがジャンプ中か、のフラグ
	int BatMoveFlag[BAT_MAX];//度の向きに動くかのフラグ

	for (int i = 0; i < BAT_MAX; i++) {
		BatPosX[i] = 0.0f;
		BatPosY[i] = 0.0f;
		BatR[i] = 40;
		BatAlive[i] = 0;
		BatTimer[i] = 0;
		BatCount[i] = 0;
		BatG[i] = 0.0f;
		BatJumpFlag[i] = FALSE;
		BatMoveFlag[i] = 0;
	}

	//コウモリの位置の初期化
	if (mapNumber == 0) {
		BatAlive[0] = 1;
		BatAlive[1] = 1;
		BatPosX[0] = 3800.0f;
		BatPosX[1] = 6000.0f;
		BatPosY[0] = 700.0f;
		BatPosY[1] = 600.0f;
	}

	else if (mapNumber == 1) {
		BatAlive[0] = 1;
		BatAlive[1] = 1;
		BatPosX[0] = 3100.0f;
		BatPosX[1] = 8600.0f;
		BatPosY[0] = 500.0f;
		BatPosY[1] = 500.0f;
	}

	else if (mapNumber == 2) {
		BatAlive[0] = 1;
		BatAlive[1] = 1;
		BatPosX[0] = 5800.0f;
		BatPosX[1] = 10900.0f;
		BatPosY[0] = 550.0f;
		BatPosY[1] = 850.0f;
	}

	else if (mapNumber == 3) {
		BatAlive[0] = 1;
		BatAlive[1] = 1;
		BatAlive[2] = 1;
		BatAlive[3] = 1;
		BatAlive[4] = 1;
		BatAlive[5] = 1;
		BatPosX[0] = 1500.0f;
		BatPosX[1] = 9100.0f;
		BatPosX[2] = 9700.0f;
		BatPosX[3] = 10200.0f;
		BatPosX[4] = 10700.0f;
		BatPosX[5] = 10900.0f;
		BatPosY[0] = 700.0f;
		BatPosY[1] = 850.0f;
		BatPosY[2] = 850.0f;
		BatPosY[3] = 850.0f;
		BatPosY[4] = 850.0f;
		BatPosY[5] = 850.0f;
	}

	else if (mapNumber == 4) {
		BatAlive[0] = 1;
		BatAlive[1] = 1;
		BatAlive[2] = 1;
		BatAlive[3] = 1;
		BatAlive[4] = 1;
		BatAlive[5] = 1;
		BatPosX[0] = 5000.0f;
		BatPosX[1] = 7400.0f;
		BatPosX[2] = 10900.0f;
		BatPosX[3] = 12830.0f;
		BatPosX[4] = 13470.0f;
		BatPosX[5] = 12634.0f;
		BatPosY[0] = 850.0f;
		BatPosY[1] = 560.0f;
		BatPosY[2] = 300.0f;
		BatPosY[3] = 920.0f;
		BatPosY[4] = 600.0f;
		BatPosY[5] = 500.0f;
	}

	//おばけのモンスター(小)の変数
	float GorstPosX[GORST_MAX];
	float GorstPosY[GORST_MAX];
	int GorstR[GORST_MAX];
	int GorstAlive[GORST_MAX];
	int GorstTimer[GORST_MAX];
	int GorstCount[GORST_MAX];
	int angleX[GORST_MAX];
	int	angleY[GORST_MAX];
	double	angleR[GORST_MAX];

	for (int i = 0; i < GORST_MAX; i++) {
		//おばけのモンスター(小)の変数
		GorstPosX[i] = 0;
		GorstPosY[i] = 0;
		GorstR[i] = 40;
		GorstAlive[i] = 0;
		GorstTimer[i] = 0;
		GorstCount[i] = 0;
		angleX[i] = 0;
		angleY[i] = 0;
		angleR[i] = 0.0f;
	}

	//コウモリの位置の初期化
	if (mapNumber == 0) {
		GorstAlive[0] = 1;
		GorstPosX[0] = 2500.0f;
		GorstPosY[0] = 600.0f;
	}

	else if (mapNumber == 1) {
		GorstAlive[0] = 1;
		GorstAlive[1] = 1;
		GorstAlive[2] = 1;
		GorstPosX[0] = 1050.0f;
		GorstPosX[1] = 6750.0f;
		GorstPosX[2] = 8600.0f;
		GorstPosY[0] = 550.0f;
		GorstPosY[1] = 700.0f;
		GorstPosY[2] = 500.0f;
	}

	else if (mapNumber == 2) {
		GorstAlive[0] = 1;
		GorstAlive[1] = 1;
		GorstPosX[0] = 4350.0f;
		GorstPosX[1] = 11300.0f;
		GorstPosY[0] = 600.0f;
		GorstPosY[1] = 740.0f;
	}

	else if (mapNumber == 3) {
		GorstAlive[0] = 1;
		GorstAlive[1] = 1;
		GorstAlive[2] = 1;
		GorstAlive[3] = 1;
		GorstPosX[0] = 2000.0f;
		GorstPosX[1] = 3300.0f;
		GorstPosX[2] = 6400.0f;
		GorstPosX[3] = 7300.0f;
		GorstPosX[4] = 15000.0f;
		GorstPosX[5] = 11100.0f;
		GorstPosY[0] = 700.0f;
		GorstPosY[1] = 700.0f;
		GorstPosY[2] = 800.0f;
		GorstPosY[3] = 700.0f;
		GorstPosY[4] = 750.0f;
		GorstPosY[5] = 600.0f;
	}

	else if (mapNumber == 4) {
		GorstAlive[0] = 1;
		GorstAlive[1] = 1;
		GorstAlive[2] = 1;
		GorstAlive[3] = 1;
		GorstAlive[4] = 1;
		GorstAlive[5] = 1;
		GorstPosX[0] = 870.0f;
		GorstPosX[1] = 5000.0f;
		GorstPosX[2] = 7500.0f;
		GorstPosX[3] = 13030.0f;
		GorstPosX[4] = 15000.0f;
		GorstPosX[5] = 12500.0f;
		GorstPosY[0] = 500.0f;
		GorstPosY[1] = 500.0f;
		GorstPosY[2] = 500.0f;
		GorstPosY[3] = 500.0f;
		GorstPosY[4] = 500.0f;
		GorstPosY[5] = 500.0f;
	}

	//おばけのモンスター(大)の変数
	float Gorst2PosX = -200.0f;
	if (mapNumber == 2) {
		Gorst2PosX = 5200.0f;
	}

	else if (mapNumber == 3) {
		Gorst2PosX = 2200.0f;
	}

	else if (mapNumber == 4) {
		Gorst2PosX = 100.0f;
	}
	float Gorst2PosY = 600.0f;
	int GorstR2 = 700;
	int Gorst2Alive = 0;
	int Gorst2Alpha = 0;
	float Gorst2Speed = 2.2f;
	int Gorst2Bright = 255;

	//音楽関係の変数
	int TopPositionFlag = 0;
	int TopPositionFlag2 = 0;
	int TopPositionFlag3 = 0;
	int Volume = 120;
	int Volume2 = 0;
	//タイトル画面の変数
	int TitleTimer = 0;
	int TitleCount = 0;
	//説明画面の変数
	int RuleTimer = 0;
	int RuleCount = 0;
	//ゲームクリア画面の変数
	int ClearTimer = 0;
	int ClearCount = 0;
	//リソースファイル
	//みかちゃん
	int LeftMika[6] = { 0 };
	LoadDivGraph("Resources/mikatyan_left.png", 6, 6, 1, 100, 100, LeftMika);
	int RightMika[6] = { 0 };
	LoadDivGraph("Resources/mikatyan_Right.png", 6, 6, 1, 100, 100, RightMika);
	//ジャンプ中
	int RightJump;
	RightJump = LoadGraph("Resources/mikatyan_jump.png");
	int LeftJump;
	LeftJump = LoadGraph("Resources/cam1.png");
	//死んだとき
	int DeathMika[9] = { 0 };
	LoadDivGraph("Resources/mikaDeth.png", 9, 9, 1, 160, 100, DeathMika);
	//敵
	int RightSlime[11] = { 0 };
	LoadDivGraph("Resources/slime-left.png", 11, 11, 1, 64, 64, RightSlime);
	int LeftSlime[11] = { 0 };
	LoadDivGraph("Resources/Slime.png", 11, 11, 1, 64, 64, LeftSlime);
	int RightBat[4] = { 0 };
	LoadDivGraph("Resources/bat-left.png", 4, 4, 1, 64, 64, RightBat);
	int LeftBat[4] = { 0 };
	LoadDivGraph("Resources/bat.png", 4, 4, 1, 64, 64, LeftBat);
	int obake[4] = { 0 };
	LoadDivGraph("Resources/obake.png", 4, 4, 1, 64, 64, obake);
	int obake2[4] = { 0 };
	LoadDivGraph("Resources/obake-sheet.png", 4, 4, 1, 64, 64, obake2);
	//背景
	int haikei;
	haikei = LoadGraph("Resources/backscreen.png");

	//マップチップ
	int mapchip[3];
	mapchip[0] = LoadGraph("Resources/blockss.png");
	mapchip[1] = LoadGraph("Resources/lightblock.png");
	mapchip[2] = LoadGraph("Resources/goal.png");
	//UI
	int UI = LoadGraph("Resources/UI.png");
	int UI2 = LoadGraph("Resources/UI2.png");

	//タイトル
	int Title[9] = { 0 };
	LoadDivGraph("Resources/title.png", 9, 9, 1, 1920, 1080, Title);
	//説明
	int Rule[8] = { 0 };
	LoadDivGraph("Resources/setumei.png", 8, 8, 1, 1920, 1080, Rule);
	//ゲームオーバー
	int GameOver;
	GameOver = LoadGraph("Resources/gameOver (1).png");
	//ステージクリア
	int Stageclear;
	Stageclear = LoadGraph("Resources/stageClear.png");
	//ゲームクリア
	int Clear[6] = { 0 };
	LoadDivGraph("Resources/clear.png", 6, 6, 1, 1920, 1080, Clear);

	//音楽
	int TitleBgm = LoadSoundMem("Sound/title.mp3");
	int StageBgm = LoadSoundMem("Sound/gamePlay.mp3");
	int StageBgm2 = LoadSoundMem("Sound/gamePlay2.mp3");
	int JumpSE = LoadSoundMem("Sound/se_jump.mp3");
	int ClearBGM = LoadSoundMem("Sound/sucess2.mp3");

	// ゲームループ
	while (1)
	{
		//最新のキーボード情報だったものは１フレーム前のキーボード情報として保存

		for (int i = 0; i < 256; i++) {
			oldkeys[i] = keys[i];
		}
		//最新のキーボード情報を取得
		GetHitKeyStateAll(keys);

		//画面クリア
		ClearDrawScreen();
		//---------  ここからプログラムを記述  ----------//

		//シーン0
		if (scene == 0) {
			TitleTimer++;
			if (TitleTimer % 15 == 0 && TitleCount <= 7) {
				TitleCount++;
			}
			if (CheckSoundMem(TitleBgm == 0)) {
				ChangeVolumeSoundMem(150 * 50 / 50, TitleBgm);
				PlaySoundMem(TitleBgm, DX_PLAYTYPE_LOOP, TopPositionFlag = 0);
			}
			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				scene = 1;
			}
		}

		else if (scene == 1) {
			RuleTimer++;
			if (RuleTimer % 15 == 0 && RuleCount <= 6) {
				RuleCount++;
			}

			if (RuleCount == 7) {
				RuleCount = 5;
			}
			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				scene = 2;
				StopSoundMem(TitleBgm);
			}
		}
		//シーン2
		else if (scene == 2) {
			ChangeVolumeSoundMem(Volume * 50 / 50, StageBgm);
			PlaySoundMem(StageBgm, DX_PLAYTYPE_LOOP, TopPositionFlag2 = 0);
			if (mapNumber == 0) {
				for (int i = 0; i < MAX_MAPCHIPY; i++) {
					for (int j = 0; j < MAX_MAPCHIPX; j++) {
						fscanf_s(fp, "%d,", &map[i][j]);
					}
				}
				fclose(fp);
			}
			else if (mapNumber == 1) {
				fclose(fp);
				for (int i = 0; i < MAX_MAPCHIPY; i++) {
					for (int j = 0; j < MAX_MAPCHIPX; j++) {
						fscanf_s(fp2, "%d,", &map[i][j]);
					}
				}
				fclose(fp2);
			}
			else if (mapNumber == 2) {
				fclose(fp2);
				for (int i = 0; i < MAX_MAPCHIPY; i++) {
					for (int j = 0; j < MAX_MAPCHIPX; j++) {
						fscanf_s(fp3, "%d,", &map[i][j]);
					}
				}

				fclose(fp3);
			}
			else if (mapNumber == 3) {
				fclose(fp3);
				for (int i = 0; i < MAX_MAPCHIPY; i++) {
					for (int j = 0; j < MAX_MAPCHIPX; j++) {
						fscanf_s(fp4, "%d,", &map[i][j]);
					}
				}
				fclose(fp4);
			}
			else if (mapNumber == 4) {
				fclose(fp4);
				for (int i = 0; i < MAX_MAPCHIPY; i++) {
					for (int j = 0; j < MAX_MAPCHIPX; j++) {
						fscanf_s(fp5, "%d,", &map[i][j]);
					}
				}
				fclose(fp5);
			}

			//更新処理
			//各移動量の初期化
			PlayerMoveX = 0.0f;
			PlayerMoveY = 0.0f;
			for (int i = 0; i < SLIME_MAX; i++) {
				SlimeMoveX[i] = 0.0f;
				SlimeMoveY[i] = 0.0f;
			}

			for (int i = 0; i < BAT_MAX; i++) {
				BatMoveX[i] = 0.0f;
				BatMoveY[i] = 0.0f;
			}

			//プレイヤーのアニメーション

			if (PlayerTimer >= 5) {
				PlayerCount++;
				PlayerTimer = 0;
			}

			if (PlayerCount == 6) {
				PlayerCount = 0;
			}

			//おばけのアニメーション
			for (int i = 0; i < GORST_MAX; i++) {
				GorstTimer[i]++;
				if (GorstTimer[i] >= 8) {
					GorstCount[i]++;
					GorstTimer[i] = 0;
				}

				if (GorstCount[i] == 4) {
					GorstCount[i] = 0;
				}
			}

			//スライムのアニメーション
			for (int i = 0; i < SLIME_MAX; i++) {
				SlimeTimer[i]++;
				if (SlimeTimer[i] >= 8) {
					SlimeCount[i]++;
					SlimeTimer[i] = 0;
				}

				if (SlimeCount[i] == 11) {
					SlimeCount[i] = 0;
				}
			}

			//コウモリのアニメーション
			for (int i = 0; i < BAT_MAX; i++) {
				BatTimer[i]++;
				if (BatTimer[i] >= 8) {
					BatCount[i]++;
					BatTimer[i] = 0;
				}

				if (BatCount[i] == 4) {
					BatCount[i] = 0;
				}
			}
			// プレイヤーの移動処理
			   // 左右の移動を見る
			if ((keys[KEY_INPUT_LEFT]) == 1 && PlayerAlive == 1) {
				PlayerTimer++;
				PlayerMoveX -= PlayerspeedX;
				playerDirection = 1;
				if (PlayerPosX - scrollX < 960) {
					scrollX = scrollX - PlayerspeedX;
				}
			}

			if ((keys[KEY_INPUT_RIGHT]) == 1 && PlayerAlive == 1) {
				PlayerTimer++;
				PlayerMoveX += PlayerspeedX;
				playerDirection = 0;
				if (PlayerPosX - scrollX > 960) {
					scrollX = scrollX + PlayerspeedX;
				}
			}

			//止まっている時の処理
			if (PlayerMoveX == 0.0f) {
				if (playerDirection == 1) {
					PlayerCount = 3;
				}
				else {
					PlayerCount = 2;
				}
			}

			//徐々に照らす
			if (playerDirection == 0 && right >= 0) {
				right -= 1;
			}

			//徐々に照らす
			else if (playerDirection == 1 && right <= 254 && LeftTimer >= 0.05) {
				right++;
				if (LeftTimer >= 0.003) {
					LeftTimer -= 0.003;
				}
			}

			//ジャンプ
			if (PlayerJumpFlag == FALSE && keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0 && PlayerAlive == 1)
			{
				PlaySoundMem(JumpSE, DX_PLAYTYPE_BACK, TRUE);
				ChangeVolumeSoundMem(180 * 30 / 60, JumpSE);
				PlayerG = -9.0;
				PlayerJumpFlag = TRUE;
			}

			// 落下処理
			PlayerG += 0.3;

			// 落下速度を移動量に加える
			PlayerMoveY = PlayerG;

			//スライムの挙動
			for (int i = 0; i < SLIME_MAX; i++) {
				if (SlimeAlive[i] == 1) {
					SlimeG[i] += 0.3;
				}
				// 落下速度を移動量に加える
				SlimeMoveY[i] = SlimeG[i];

				//スライムが折り返す時の動き
				if (SlimeMoveFlag[i] == 0) {
					SlimeMoveX[i] += 3.0f;
				}

				else if (SlimeMoveFlag[i] == 1) {
					SlimeMoveX[i] -= 3.0f;
				}
			}

			//コウモリの挙動
			for (int i = 0; i < BAT_MAX; i++) {
				if (BatAlive[i] == 1) {
					BatG[i] += 0.3;
				}
				// 落下速度を移動量に加える
				BatMoveY[i] = BatG[i];

				//スライムが折り返す時の動き
				if (BatMoveFlag[i] == 0) {
					BatMoveX[i] += 3.0f;
				}

				else if (BatMoveFlag[i] == 1) {
					BatMoveX[i] -= 3.0f;
				}
			}

			//スライム
			for (int i = 0; i < SLIME_MAX; i++) {
				//右の壁にあたったとき
				if (map[(static_cast<int>(SlimePosY[i] - SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] + SlimeR[i])) / 64] != 0 &&
					map[(static_cast<int>(SlimePosY[i] + SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] + SlimeR[i] - 1)) / 64] != 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 1;
				}

				//左の壁にあたったとき
				if (map[(static_cast<int>(SlimePosY[i] - SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] - SlimeR[i])) / 64] != 0 &&
					map[(static_cast<int>(SlimePosY[i] + SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] - SlimeR[i])) / 64] != 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 0;
				}

				//右下にブロックがなかったとき
				if (map[(static_cast<int>(SlimePosY[i] + SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] + SlimeR[i] - 1)) / 64] == 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 1;
				}

				//左下にブロックがなかったとき
				if (map[(static_cast<int>(SlimePosY[i] + SlimeR[i] - 1)) / 64][(static_cast<int>(SlimePosX[i] - SlimeR[i])) / 64] == 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 0;
				}
			}

			//コウモリ
			for (int i = 0; i < BAT_MAX; i++) {
				//右の壁にあたったとき
				if (map[(static_cast<int>(BatPosY[i] - BatR[i])) / 64][(static_cast<int>(BatPosX[i] + BatR[i])) / 64] != 0 &&
					map[(static_cast<int>(BatPosY[i] + BatR[i])) / 64][(static_cast<int>(BatPosX[i] + BatR[i] - 1)) / 64] != 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 1;
				}

				//左の壁にあたったとき
				if (map[(static_cast<int>(BatPosY[i] - BatR[i])) / 64][(static_cast<int>(BatPosX[i] - BatR[i])) / 64] != 0 &&
					map[(static_cast<int>(BatPosY[i] + BatR[i])) / 64][(static_cast<int>(BatPosX[i] - BatR[i])) / 64] != 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 0;
				}

				//右下にブロックがなかったとき
				if (map[(static_cast<int>(BatPosY[i] + BatR[i])) / 64][(static_cast<int>(BatPosX[i] + BatR[i] - 1)) / 64] == 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 1;
				}

				//左下にブロックがなかったときn
				if (map[(static_cast<int>(BatPosY[i] + BatR[i] - 1)) / 64][(static_cast<int>(BatPosX[i] - BatR[i])) / 64] == 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 0;
				}
			}
			////ゴースト
			for (int i = 0; i < GORST_MAX; i++) {
				if (GorstAlive[i] == 1 && PlayerPosX < GorstPosX[i] && GorstPosX[i] - PlayerPosX <= 960) {
					angleX[i] = (PlayerPosX - GorstPosX[i]);
					angleY[i] = (PlayerPosY - GorstPosY[i]);
					angleR[i] = sqrt((PlayerPosX - GorstPosX[i]) * (PlayerPosX - GorstPosX[i])
						+ (PlayerPosY - GorstPosY[i]) * (PlayerPosY - GorstPosY[i]));
					GorstPosX[i] += (angleX[i] / angleR[i]) * 2;
					GorstPosY[i] += (angleY[i] / angleR[i]) * 2;
				}

				else if (GorstAlive[i] == 1 && PlayerPosX > GorstPosX[i]) {
					GorstPosX[i] -= 3;
					GorstPosY[i] += (angleY[i] / angleR[i]) * 2;
				}

				if (PlayerPosX - GorstPosX[i] >= 960) {
					GorstAlive[i] = 0;
				}
			}

			//敵とプレイヤーの当たり判定
			//ゴースト(小)
			for (int i = 0; i < GORST_MAX; i++) {
				if (collision(PlayerPosX - PlayerR - scrollX, PlayerPosY - PlayerR - scrollX,
					PlayerPosX + PlayerR - scrollX, PlayerPosY + PlayerR - scrollX,
					GorstPosX[i] - (GorstR[i] - 40) - scrollX, GorstPosY[i] - (GorstR[i] - 40) - scrollX,
					GorstPosX[i] + (GorstR[i] - 40) - scrollX, GorstPosY[i] + (GorstR[i] - 40) - scrollX) == 1
					&& GorstAlive[i] == 1) {
					PlayerAlive = 0;
				}
			}

			//スライム
			for (int i = 0; i < SLIME_MAX; i++) {
				if (collision(PlayerPosX - PlayerR - scrollX, PlayerPosY - PlayerR - scrollX,
					PlayerPosX + PlayerR - scrollX, PlayerPosY + PlayerR - scrollX,
					SlimePosX[i] - (SlimeR[i] - 40) - scrollX, SlimePosY[i] - (SlimeR[i] - 40) - scrollX,
					SlimePosX[i] + (SlimeR[i] - 40) - scrollX, SlimePosY[i] + (SlimeR[i] - 40) - scrollX) == 1
					&& SlimeAlive[i] == 1) {
					PlayerAlive = 0;
				}
			}

			//コウモリ
			for (int i = 0; i < GORST_MAX; i++) {
				if (collision(PlayerPosX - PlayerR - scrollX, PlayerPosY - PlayerR - scrollX,
					PlayerPosX + PlayerR - scrollX, PlayerPosY + PlayerR - scrollX,
					BatPosX[i] - (BatR[i] - 40) - scrollX, BatPosY[i] - (BatR[i] - 40) - scrollX,
					BatPosX[i] + (BatR[i] - 40) - scrollX, BatPosY[i] + (BatR[i] - 40) - scrollX) == 1
					&& BatAlive[i] == 1) {
					PlayerAlive = 0;
				}
			}

			//敵とプレイヤーの当たり判定
			//ゴースト(大)
			if (mapNumber == 2 || mapNumber == 3 || mapNumber == 4) {
				if (collision(PlayerPosX - PlayerR - scrollX, PlayerPosY - PlayerR - scrollX,
					PlayerPosX + PlayerR - scrollX, PlayerPosY + PlayerR - scrollX,
					Gorst2PosX - (GorstR2 - 200) - scrollX, Gorst2PosY - (GorstR2 - 200) - scrollX,
					Gorst2PosX + (GorstR2 - 200) - scrollX, Gorst2PosY + (GorstR2 - 200) - scrollX) == 1
					&& Gorst2Alive == 1) {
					//scene = 2;
					PlayerAlive = 0;
				}
			}

			//ゴールにあたった時のシーン管理
			if (GetChipParam(PlayerPosX, PlayerPosY) == 3) {
				if (mapNumber != 4) {
					scene = 4;
				}

				else {
					scene = 5;
				}
			}

			// 移動量に基づいてキャラクタの座標を移動
			CharMove(&PlayerPosX, &PlayerPosY, &PlayerG, PlayerMoveX, PlayerMoveY, PlayerR, &PlayerJumpFlag);
			for (int i = 0; i < SLIME_MAX; i++) {
				if (SlimeAlive[i] == 1) {
					CharMove(&SlimePosX[i], &SlimePosY[i], &SlimeG[i], SlimeMoveX[i], SlimeMoveY[i], SlimeR[i], &SlimeJumpFlag[i]);
				}
			}

			for (int i = 0; i < BAT_MAX; i++) {
				if (BatAlive[i] == 1) {
					CharMove(&BatPosX[i], &BatPosY[i], &BatG[i], BatMoveX[i], BatMoveY[i], BatR[i], &BatJumpFlag[i]);
				}
			}

			//大きなおばけが出る条件
			if (PlayerPosX > 6000 && mapNumber == 2) {
				Gorst2Alive = 1;
			}

			//大きなおばけが出る条件
			if (PlayerPosX > 3000 && mapNumber == 3) {
				Gorst2Alive = 1;
			}

			if (mapNumber == 4) {

				if (PlayerPosX >= 1000) {
					Gorst2Alive = 1;
				}

				if (Gorst2PosX < 10000 && Gorst2PosX >= 5000) {
					Gorst2Speed = 2.4;
					Gorst2Bright = 128;
				}

				else if (Gorst2PosX >= 10000) {
					Gorst2Speed = 2.6;
					Gorst2Bright = 0;
				}
			}

			if (Gorst2Alive == 1) {
				Gorst2PosX += Gorst2Speed;
				Gorst2Alpha++;
				Volume--;
			}

			//音楽を変える
			if (Volume <= 0) {
				if (CheckSoundMem(StageBgm2 == 0)) {
					ChangeVolumeSoundMem(Volume2 * 50 / 50, StageBgm2);
					PlaySoundMem(StageBgm2, DX_PLAYTYPE_LOOP, TopPositionFlag3 = 0);
				}

				if (Volume2 <= 120) {
					Volume2++;
				}
			}

			if (PlayerPosY >= 1150) {
				PlayerAlive = 0;
			}

			if (PlayerAlive == 0) {
				StopSoundMem(StageBgm);
				StopSoundMem(StageBgm2);
				DeathTimer++;
			}
			if (DeathTimer >= 8) {
				DeathCount++;
				DeathTimer = 0;
			}

			if (DeathCount == 9) {
				scene = 3;
			}
		}

		//シーン3(ゲームオーバー）
		else if (scene == 3) {

			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				scene = 2;
				ChangeVolumeSoundMem(Volume * 50 / 50, StageBgm);
				PlaySoundMem(StageBgm, DX_PLAYTYPE_LOOP, TopPositionFlag2 = 1);
			}
			//プレイヤーの変数
			PlayerPosX = 300.0f; //プレイヤーの初期座標
			PlayerPosY = 700.0f; //プレイヤーの初期座標
			PlayerR = 50;//半径
			PlayerspeedX = 8.0f;	//プレイヤーの移動力
			PlayerG = 0.0f;// プレイヤーの落下速度
			PlayerJumpFlag = FALSE;
			playerDirection = 0;//プレイヤーの向いてる向き	
			PlayerAlive = 1;//プレイヤーの生存フラグ
			PlayerTimer = 0;//アニメーション関係
			PlayerCount = 0;
			DeathTimer = 0;//死んだ時のアニメーション
			DeathCount = 0;
			right = 0;//照らす強さ
			LeftTimer = 10.0f;//左向ける時間
			scrollX = PlayerPosX - 960;

			for (int i = 0; i < SLIME_MAX; i++) {
				SlimePosX[i] = 0.0f;
				SlimePosY[i] = 0.0f;
				SlimeR[i] = 40;
				SlimeAlive[i] = 0;
				SlimeTimer[i] = 0;
				SlimeCount[i] = 0;
				SlimeG[i] = 0.0f;
				SlimeJumpFlag[i] = FALSE;
				SlimeMoveFlag[i] = 0;
			}

			//スライムの位置の初期化
			if (mapNumber == 0) {
				SlimeAlive[0] = 1;
				SlimeAlive[1] = 1;
				SlimePosX[0] = 1200.0f;
				SlimePosX[1] = 5300.0f;
				SlimePosY[0] = 600.0f;
				SlimePosY[1] = 700.0f;
			}

			else if (mapNumber == 1) {
				SlimeAlive[0] = 1;
				SlimeAlive[1] = 1;
				SlimePosX[0] = 1900.0f;
				SlimePosX[1] = 4700.0f;
				SlimePosY[0] = 900.0f;
				SlimePosY[1] = 600.0f;
			}

			else if (mapNumber == 2) {
				SlimeAlive[0] = 1;
				SlimeAlive[1] = 1;
				SlimePosX[0] = 2800.0f;
				SlimePosX[1] = 7800.0f;
				SlimePosY[0] = 800.0f;
				SlimePosY[1] = 550.0f;
			}

			else if (mapNumber == 3) {
				SlimeAlive[0] = 1;
				SlimeAlive[1] = 1;
				SlimeAlive[2] = 1;
				SlimeAlive[3] = 1;
				SlimeAlive[4] = 1;
				SlimeAlive[5] = 1;
				SlimePosX[0] = 1050.0f;
				SlimePosX[1] = 5000.0f;
				SlimePosX[2] = 8900.0f;
				SlimePosX[3] = 9300.0f;
				SlimePosX[4] = 10000.0f;
				SlimePosX[5] = 10500.0f;
				SlimePosY[0] = 850.0f;
				SlimePosY[1] = 700.0f;
				SlimePosY[2] = 750.0f;
				SlimePosY[3] = 750.0f;
				SlimePosY[4] = 750.0f;
				SlimePosY[5] = 750.0f;
			}

			else if (mapNumber == 4) {
				SlimeAlive[0] = 1;
				SlimeAlive[1] = 1;
				SlimeAlive[2] = 1;
				SlimeAlive[3] = 1;
				SlimeAlive[4] = 1;
				SlimeAlive[5] = 1;
				SlimePosX[0] = 650.0f;
				SlimePosX[1] = 6400.0f;
				SlimePosX[2] = 7400.0f;
				SlimePosX[3] = 11600.0f;
				SlimePosX[4] = 13278.0f;
				SlimePosX[5] = 12318.0f;
				SlimePosY[0] = 100.0f;
				SlimePosY[1] = 600.0f;
				SlimePosY[2] = 360.0f;
				SlimePosY[3] = 600.0f;
				SlimePosY[4] = 200.0f;
				SlimePosY[5] = 100.0f;
			}

			for (int i = 0; i < BAT_MAX; i++) {
				BatPosX[i] = 0.0f;
				BatPosY[i] = 0.0f;
				BatR[i] = 40;
				BatAlive[i] = 0;
				BatTimer[i] = 0;
				BatCount[i] = 0;
				BatG[i] = 0.0f;
				BatJumpFlag[i] = FALSE;
				BatMoveFlag[i] = 0;
			}

			//コウモリの位置の初期化
			if (mapNumber == 0) {
				BatAlive[0] = 1;
				BatAlive[1] = 1;
				BatPosX[0] = 3800.0f;
				BatPosX[1] = 6000.0f;
				BatPosY[0] = 700.0f;
				BatPosY[1] = 600.0f;
			}

			else if (mapNumber == 1) {
				BatAlive[0] = 1;
				BatAlive[1] = 1;
				BatPosX[0] = 3100.0f;
				BatPosX[1] = 8600.0f;
				BatPosY[0] = 500.0f;
				BatPosY[1] = 500.0f;
			}

			else if (mapNumber == 2) {
				BatAlive[0] = 1;
				BatAlive[1] = 1;
				BatPosX[0] = 5800.0f;
				BatPosX[1] = 10900.0f;
				BatPosY[0] = 550.0f;
				BatPosY[1] = 850.0f;
			}

			else if (mapNumber == 3) {
				BatAlive[0] = 1;
				BatAlive[1] = 1;
				BatAlive[2] = 1;
				BatAlive[3] = 1;
				BatAlive[4] = 1;
				BatAlive[5] = 1;
				BatPosX[0] = 1500.0f;
				BatPosX[1] = 9100.0f;
				BatPosX[2] = 9700.0f;
				BatPosX[3] = 10200.0f;
				BatPosX[4] = 10700.0f;
				BatPosX[5] = 10900.0f;
				BatPosY[0] = 700.0f;
				BatPosY[1] = 850.0f;
				BatPosY[2] = 850.0f;
				BatPosY[3] = 850.0f;
				BatPosY[4] = 850.0f;
				BatPosY[5] = 850.0f;
			}

			else if (mapNumber == 4) {
				BatAlive[0] = 1;
				BatAlive[1] = 1;
				BatAlive[2] = 1;
				BatAlive[3] = 1;
				BatAlive[4] = 1;
				BatAlive[5] = 1;
				BatPosX[0] = 5000.0f;
				BatPosX[1] = 7400.0f;
				BatPosX[2] = 10900.0f;
				BatPosX[3] = 12830.0f;
				BatPosX[4] = 13470.0f;
				BatPosX[5] = 12634.0f;
				BatPosY[0] = 850.0f;
				BatPosY[1] = 560.0f;
				BatPosY[2] = 300.0f;
				BatPosY[3] = 920.0f;
				BatPosY[4] = 600.0f;
				BatPosY[5] = 500.0f;
			}
			for (int i = 0; i < GORST_MAX; i++) {
				//おばけのモンスター(小)の変数
				GorstPosX[i] = 0;
				GorstPosY[i] = 0;
				GorstR[i] = 40;
				GorstAlive[i] = 0;
				GorstTimer[i] = 0;
				GorstCount[i] = 0;
				angleX[i] = 0;
				angleY[i] = 0;
				angleR[i] = 0.0f;
			}

			//コウモリの位置の初期化
			if (mapNumber == 0) {
				GorstAlive[0] = 1;
				GorstPosX[0] = 2500.0f;
				GorstPosY[0] = 600.0f;
			}

			else if (mapNumber == 1) {
				GorstAlive[0] = 1;
				GorstAlive[1] = 1;
				GorstAlive[2] = 1;
				GorstPosX[0] = 1050.0f;
				GorstPosX[1] = 6750.0f;
				GorstPosX[2] = 8600.0f;
				GorstPosY[0] = 550.0f;
				GorstPosY[1] = 700.0f;
				GorstPosY[2] = 500.0f;
			}

			else if (mapNumber == 2) {
				GorstAlive[0] = 1;
				GorstAlive[1] = 1;
				GorstPosX[0] = 4350.0f;
				GorstPosX[1] = 11300.0f;
				GorstPosY[0] = 600.0f;
				GorstPosY[1] = 740.0f;
			}

			else if (mapNumber == 3) {
				GorstAlive[0] = 1;
				GorstAlive[1] = 1;
				GorstAlive[2] = 1;
				GorstAlive[3] = 1;
				GorstPosX[0] = 2000.0f;
				GorstPosX[1] = 3300.0f;
				GorstPosX[2] = 6400.0f;
				GorstPosX[3] = 7300.0f;
				GorstPosX[4] = 15000.0f;
				GorstPosX[5] = 11100.0f;
				GorstPosY[0] = 700.0f;
				GorstPosY[1] = 700.0f;
				GorstPosY[2] = 800.0f;
				GorstPosY[3] = 700.0f;
				GorstPosY[4] = 750.0f;
				GorstPosY[5] = 600.0f;
			}

			else if (mapNumber == 4) {
				GorstAlive[0] = 1;
				GorstAlive[1] = 1;
				GorstAlive[2] = 1;
				GorstAlive[3] = 1;
				GorstAlive[4] = 1;
				GorstAlive[5] = 1;
				GorstPosX[0] = 870.0f;
				GorstPosX[1] = 5000.0f;
				GorstPosX[2] = 7500.0f;
				GorstPosX[3] = 13030.0f;
				GorstPosX[4] = 15000.0f;
				GorstPosX[5] = 12500.0f;
				GorstPosY[0] = 500.0f;
				GorstPosY[1] = 500.0f;
				GorstPosY[2] = 500.0f;
				GorstPosY[3] = 500.0f;
				GorstPosY[4] = 500.0f;
				GorstPosY[5] = 500.0f;
			}

			//おばけのモンスター(大)の変数
			Gorst2PosX = -200.0f;
			if (mapNumber == 2) {
				Gorst2PosX = 5200.0f;
			}

			else if (mapNumber == 3) {
				Gorst2PosX = 2200.0f;
			}

			else if (mapNumber == 4) {
				Gorst2PosX = 100.0f;
			}
			Gorst2PosY = 600.0f;
			GorstR2 = 700;
			Gorst2Alive = 0;
			Gorst2Alpha = 0;
			Gorst2Speed = 2.2f;
			Gorst2Bright = 255;

			Volume = 120;
			Volume2 = 0;
			//タイトル画面の変数
			TitleTimer = 0;
			TitleCount = 0;
			//説明画面の変数
			RuleTimer = 0;
			RuleCount = 0;
		}

		//シーン4（ステージクリア)
		else if (scene == 4) {
			StopSoundMem(StageBgm);
			StopSoundMem(StageBgm2);
			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				ChangeVolumeSoundMem(Volume * 50 / 50, StageBgm);
				PlaySoundMem(StageBgm, DX_PLAYTYPE_LOOP, TopPositionFlag2 = 1);
				mapNumber++;
				scene = 2;
				//変数の再度初期化
				PlayerAlive = 1;
				PlayerPosX = 100.0f;
				PlayerPosY = 700.0f;
				LeftTimer = 10.0f;
				DeathTimer = 0;
				DeathCount = 0;
				right = 0;
				PlayerG = 0.0f;
				scrollX = PlayerPosX - 960;

				for (int i = 0; i < SLIME_MAX; i++) {
					SlimePosX[i] = 0.0f;
					SlimePosY[i] = 0.0f;
					SlimeR[i] = 40;
					SlimeAlive[i] = 0;
					SlimeTimer[i] = 0;
					SlimeCount[i] = 0;
					SlimeG[i] = 0.0f;
					SlimeJumpFlag[i] = FALSE;
					SlimeMoveFlag[i] = 0;
				}


				//スライムの位置の初期化
				if (mapNumber == 0) {
					SlimeAlive[0] = 1;
					SlimeAlive[1] = 1;
					SlimePosX[0] = 1200.0f;
					SlimePosX[1] = 5300.0f;
					SlimePosY[0] = 600.0f;
					SlimePosY[1] = 700.0f;
				}

				else if (mapNumber == 1) {
					SlimeAlive[0] = 1;
					SlimeAlive[1] = 1;
					SlimePosX[0] = 1900.0f;
					SlimePosX[1] = 4700.0f;
					SlimePosY[0] = 850.0f;
					SlimePosY[1] = 600.0f;
				}

				else if (mapNumber == 2) {
					SlimeAlive[0] = 1;
					SlimeAlive[1] = 1;
					SlimePosX[0] = 2800.0f;
					SlimePosX[1] = 7800.0f;
					SlimePosY[0] = 800.0f;
					SlimePosY[1] = 550.0f;
				}

				else if (mapNumber == 3) {
					SlimeAlive[0] = 1;
					SlimeAlive[1] = 1;
					SlimeAlive[2] = 1;
					SlimeAlive[3] = 1;
					SlimeAlive[4] = 1;
					SlimeAlive[5] = 1;
					SlimePosX[0] = 1050.0f;
					SlimePosX[1] = 5000.0f;
					SlimePosX[2] = 8900.0f;
					SlimePosX[3] = 9300.0f;
					SlimePosX[4] = 10000.0f;
					SlimePosX[5] = 10500.0f;
					SlimePosY[0] = 850.0f;
					SlimePosY[1] = 700.0f;
					SlimePosY[2] = 750.0f;
					SlimePosY[3] = 750.0f;
					SlimePosY[4] = 750.0f;
					SlimePosY[5] = 750.0f;
				}

				else if (mapNumber == 4) {
					SlimeAlive[0] = 1;
					SlimeAlive[1] = 1;
					SlimeAlive[2] = 1;
					SlimeAlive[3] = 1;
					SlimeAlive[4] = 1;
					SlimeAlive[5] = 1;
					SlimePosX[0] = 650.0f;
					SlimePosX[1] = 6400.0f;
					SlimePosX[2] = 7400.0f;
					SlimePosX[3] = 11600.0f;
					SlimePosX[4] = 13278.0f;
					SlimePosX[5] = 12318.0f;
					SlimePosY[0] = 100.0f;
					SlimePosY[1] = 600.0f;
					SlimePosY[2] = 360.0f;
					SlimePosY[3] = 600.0f;
					SlimePosY[4] = 200.0f;
					SlimePosY[5] = 100.0f;
				}

				for (int i = 0; i < BAT_MAX; i++) {
					BatPosX[i] = 0.0f;
					BatPosY[i] = 0.0f;
					BatR[i] = 40;
					BatAlive[i] = 0;
					BatTimer[i] = 0;
					BatCount[i] = 0;
					BatG[i] = 0.0f;
					BatJumpFlag[i] = FALSE;
					BatMoveFlag[i] = 0;
				}

				//コウモリの位置の初期化
				if (mapNumber == 0) {
					BatAlive[0] = 1;
					BatAlive[1] = 1;
					BatPosX[0] = 3800.0f;
					BatPosX[1] = 6000.0f;
					BatPosY[0] = 700.0f;
					BatPosY[1] = 600.0f;
				}

				else if (mapNumber == 1) {
					BatAlive[0] = 1;
					BatAlive[1] = 1;
					BatPosX[0] = 3100.0f;
					BatPosX[1] = 8600.0f;
					BatPosY[0] = 500.0f;
					BatPosY[1] = 500.0f;
				}

				else if (mapNumber == 2) {
					BatAlive[0] = 1;
					BatAlive[1] = 1;
					BatPosX[0] = 5800.0f;
					BatPosX[1] = 10900.0f;
					BatPosY[0] = 550.0f;
					BatPosY[1] = 850.0f;
				}

				else if (mapNumber == 3) {
					BatAlive[0] = 1;
					BatAlive[1] = 1;
					BatAlive[2] = 1;
					BatAlive[3] = 1;
					BatAlive[4] = 1;
					BatAlive[5] = 1;
					BatPosX[0] = 1500.0f;
					BatPosX[1] = 9100.0f;
					BatPosX[2] = 9500.0f;
					BatPosX[3] = 9900.0f;
					BatPosX[4] = 10300.0f;
					BatPosX[5] = 10800.0f;
					BatPosY[0] = 700.0f;
					BatPosY[1] = 850.0f;
					BatPosY[2] = 850.0f;
					BatPosY[3] = 850.0f;
					BatPosY[4] = 850.0f;
					BatPosY[5] = 850.0f;
				}

				else if (mapNumber == 4) {
					BatAlive[0] = 1;
					BatAlive[1] = 1;
					BatAlive[2] = 1;
					BatAlive[3] = 1;
					BatAlive[4] = 1;
					BatAlive[5] = 1;
					BatPosX[0] = 5000.0f;
					BatPosX[1] = 7400.0f;
					BatPosX[2] = 10900.0f;
					BatPosX[3] = 12830.0f;
					BatPosX[4] = 13470.0f;
					BatPosX[5] = 12634.0f;
					BatPosY[0] = 850.0f;
					BatPosY[1] = 560.0f;
					BatPosY[2] = 300.0f;
					BatPosY[3] = 850.0f;
					BatPosY[4] = 600.0f;
					BatPosY[5] = 500.0f;
				}

				for (int i = 0; i < GORST_MAX; i++) {
					//おばけのモンスター(小)の変数
					GorstPosX[i] = 0;
					GorstPosY[i] = 0;
					GorstR[i] = 40;
					GorstAlive[i] = 0;
					GorstTimer[i] = 0;
					GorstCount[i] = 0;
					angleX[i] = 0;
					angleY[i] = 0;
					angleR[i] = 0.0f;
				}

				//おばけの位置の初期化
				if (mapNumber == 0) {
					GorstAlive[0] = 1;
					GorstPosX[0] = 2500.0f;
					GorstPosY[0] = 600.0f;
				}

				else if (mapNumber == 1) {
					GorstAlive[0] = 1;
					GorstAlive[1] = 1;
					GorstAlive[2] = 1;
					GorstPosX[0] = 1050.0f;
					GorstPosX[1] = 6750.0f;
					GorstPosX[2] = 8600.0f;
					GorstPosY[0] = 550.0f;
					GorstPosY[1] = 700.0f;
					GorstPosY[2] = 500.0f;
				}

				else if (mapNumber == 2) {
					GorstAlive[0] = 1;
					GorstAlive[1] = 1;
					GorstPosX[0] = 4350.0f;
					GorstPosX[1] = 11300.0f;
					GorstPosY[0] = 600.0f;
					GorstPosY[1] = 740.0f;
				}

				else if (mapNumber == 3) {
					GorstAlive[0] = 1;
					GorstAlive[1] = 1;
					GorstAlive[2] = 1;
					GorstAlive[3] = 1;
					GorstAlive[4] = 1;
					GorstAlive[5] = 1;
					GorstPosX[0] = 2000.0f;
					GorstPosX[1] = 3300.0f;
					GorstPosX[2] = 6400.0f;
					GorstPosX[3] = 7300.0f;
					GorstPosX[4] = 10500.0f;
					GorstPosX[5] = 10800.0f;
					GorstPosY[0] = 700.0f;
					GorstPosY[1] = 700.0f;
					GorstPosY[2] = 800.0f;
					GorstPosY[3] = 700.0f;
					GorstPosY[4] = 750.0f;
					GorstPosY[5] = 600.0f;
				}

				else if (mapNumber == 4) {
					GorstAlive[0] = 1;
					GorstAlive[1] = 1;
					GorstAlive[2] = 1;
					GorstAlive[3] = 1;
					GorstAlive[4] = 1;
					GorstAlive[5] = 1;
					GorstPosX[0] = 870.0f;
					GorstPosX[1] = 5000.0f;
					GorstPosX[2] = 7500.0f;
					GorstPosX[3] = 13030.0f;
					GorstPosX[4] = 15000.0f;
					GorstPosX[5] = 12500.0f;
					GorstPosY[0] = 500.0f;
					GorstPosY[1] = 500.0f;
					GorstPosY[2] = 500.0f;
					GorstPosY[3] = 500.0f;
					GorstPosY[4] = 500.0f;
					GorstPosY[5] = 500.0f;
				}

				if (mapNumber == 2) {
					Gorst2PosX = 5200;
				}

				else if (mapNumber == 3) {
					Gorst2PosX = 2200;
				}

				else if (mapNumber == 4) {
					Gorst2PosX = 200;
				}
				Gorst2PosY = 600;
				GorstR2 = 700;
				Gorst2Alive = 0;
				Gorst2Alpha = 0;
				Volume = 120;
				Volume2 = 0;

				TopPositionFlag = 0;

				TopPositionFlag3 = 1;
			}
		}

		//シーン4（ゲームクリア)
		else if (scene == 5) {
			StopSoundMem(StageBgm);
			StopSoundMem(StageBgm2);
			ClearTimer++;
			if (ClearTimer % 10 == 0 && ClearCount <= 5) {
				ClearCount++;
			}

			if (ClearCount == 6) {
				ClearCount = 0;
			}
			PlaySoundMem(ClearBGM, DX_PLAYTYPE_BACK, FALSE);
			ChangeVolumeSoundMem(180 * 30 / 60, ClearBGM);
		}
		//描画処理
		//シーン0
		if (scene == 0) {
			DrawGraph(0, 0, Title[TitleCount], TRUE);
		}
		//シーン1
		else if (scene == 1) {
			DrawGraph(0, 0, Rule[RuleCount], TRUE);
		}
		//シーン2
		else if (scene == 2) {
			//背景の描画
			SetDrawBright(right + 80, right + 80, right + 80);
			DrawGraph(-1920 - scrollX, 0, haikei, TRUE);
			for (int i = 0; i < 10; i++) {
				DrawGraph(1920 * i - scrollX, 0, haikei, TRUE);
			}
			SetDrawBright(255, 255, 255);
			//マップ配置
			for (int i = 0; i < MAX_MAPCHIPY; i++)
			{
				for (int j = 0; j < MAX_MAPCHIPX; j++)
				{
					if (map[i][j] == 1)
					{
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, right);
						DrawGraph(j * blocksize - scrollX, i * blocksize, mapchip[1], TRUE);
					}

					else if (map[i][j] == 2)
					{
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						DrawGraph(j * blocksize - scrollX, i * blocksize, mapchip[0], TRUE);
					}

					else if (map[i][j] == 3)
					{
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, right);
						DrawGraph(j * blocksize - scrollX, i * blocksize, mapchip[2], TRUE);
					}
				}
			}

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			//明かりのゲージ
			DrawLineAA((PlayerPosX - PlayerR) + (LeftTimer / 10) * 60 - scrollX, PlayerPosY - 30, PlayerPosX - PlayerR - scrollX, PlayerPosY - 30, GetColor(50, 200, 200), 10);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, right);
			//スライム
			for (int i = 0; i < SLIME_MAX; i++) {
				if (SlimeAlive[i] == 1 && SlimeMoveFlag[i] == 0) {
					DrawExtendGraph(SlimePosX[i] - SlimeR[i] - scrollX, (SlimePosY[i] - 20) - SlimeR[i],
						SlimePosX[i] + SlimeR[i] - scrollX, (SlimePosY[i] - 20) + SlimeR[i],
						RightSlime[SlimeCount[i]], TRUE);
				}

				else if (SlimeAlive[i] == 1 && SlimeMoveFlag[i] == 1) {
					DrawExtendGraph(SlimePosX[i] - SlimeR[i] - scrollX, (SlimePosY[i] - 20) - SlimeR[i],
						SlimePosX[i] + SlimeR[i] - scrollX, (SlimePosY[i] - 20) + SlimeR[i],
						LeftSlime[SlimeCount[i]], TRUE);
				}
			}

			//コウモリ
			for (int i = 0; i < BAT_MAX; i++) {
				if (BatAlive[i] == 1 && BatMoveFlag[i] == 0) {
					DrawExtendGraph(BatPosX[i] - BatR[i] - scrollX, (BatPosY[i] - 20) - BatR[i],
						BatPosX[i] + BatR[i] - scrollX, (BatPosY[i] - 20) + BatR[i],
						RightBat[BatCount[i]], TRUE);
				}

				else if (BatAlive[i] == 1 && BatMoveFlag[i] == 1) {
					DrawExtendGraph(BatPosX[i] - BatR[i] - scrollX, (BatPosY[i] - 20) - BatR[i],
						BatPosX[i] + BatR[i] - scrollX, (BatPosY[i] - 20) + BatR[i],
						LeftBat[BatCount[i]], TRUE);
				}
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			//おばけ(小)
			for (int i = 0; i < GORST_MAX; i++) {
				if (GorstAlive[i] == 1) {
					DrawExtendGraph(GorstPosX[i] - GorstR[i] - scrollX, GorstPosY[i] - GorstR[i],
						GorstPosX[i] + GorstR[i] - scrollX, GorstPosY[i] + GorstR[i],
						obake[GorstCount[i]], TRUE);
				}
			}

			//おばけ(大)
			if (Gorst2Alive == 1) {
				SetDrawBright(255, Gorst2Bright, Gorst2Bright);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, Gorst2Alpha);
				DrawExtendGraph(Gorst2PosX - GorstR2 - scrollX, Gorst2PosY - GorstR2,
					Gorst2PosX + GorstR2 - scrollX, Gorst2PosY + GorstR2,
					obake2[GorstCount[0]], TRUE);
			}

			SetDrawBright(255, 255, 255);

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			//プレイヤー
			if (playerDirection == 0 && PlayerAlive == 1) {
				if (PlayerJumpFlag == FALSE) {
					DrawExtendGraph((int)(PlayerPosX - PlayerR * 0.5F) - scrollX, (int)(PlayerPosY - PlayerR * 0.5F),
						(int)(PlayerPosX + PlayerR * 0.5F) + 1 - scrollX, (int)(PlayerPosY + PlayerR * 0.5F) + 1,
						RightMika[PlayerCount], TRUE);
				}

				else if (PlayerJumpFlag == TRUE) {
					DrawExtendGraph((int)(PlayerPosX - PlayerR * 0.5F) - scrollX, (int)(PlayerPosY - PlayerR * 0.5F),
						(int)(PlayerPosX + PlayerR * 0.5F) + 1 - scrollX, (int)(PlayerPosY + PlayerR * 0.5F) + 1,
						RightJump, TRUE);
				}
			}

			else if (playerDirection == 1 && PlayerAlive == 1) {
				if (PlayerJumpFlag == FALSE) {
					DrawExtendGraph((int)(PlayerPosX - PlayerR * 0.5F) - scrollX, (int)(PlayerPosY - PlayerR * 0.5F),
						(int)(PlayerPosX + PlayerR * 0.5F) + 1 - scrollX, (int)(PlayerPosY + PlayerR * 0.5F) + 1,
						LeftMika[PlayerCount], TRUE);
				}

				else if (PlayerJumpFlag == TRUE) {
					DrawExtendGraph((int)(PlayerPosX - PlayerR * 0.5F) - scrollX, (int)(PlayerPosY - PlayerR * 0.5F),
						(int)(PlayerPosX + PlayerR * 0.5F) + 1 - scrollX, (int)(PlayerPosY + PlayerR * 0.5F) + 1,
						LeftJump, TRUE);
				}
			}

			else if (PlayerAlive == 0) {
				DrawExtendGraph((int)(PlayerPosX - 80 * 0.5F) - scrollX, (int)(PlayerPosY - PlayerR * 0.5F),
					(int)(PlayerPosX + 80 * 0.5F) + 1 - scrollX, (int)(PlayerPosY + PlayerR * 0.5F) + 1,
					DeathMika[DeathCount], TRUE);
			}

			DrawGraph(1320, 1005, UI, TRUE);
			DrawGraph(1620, 1005, UI2, TRUE);
		}

		//シーン3
		else if (scene == 3) {
			DrawGraph(0, 0, GameOver, TRUE);
		}

		//シーン4
		else if (scene == 4) {
			DrawGraph(0, 0, Stageclear, TRUE);
		}

		//シーン5
		else if (scene == 5) {
			DrawGraph(0, 0, Clear[ClearCount], TRUE);
		}

		//---------  ここまでにプログラムを記述  ---------//
		ScreenFlip();//（ダブルバッファ）裏面
		// 20ミリ秒待機（疑似60FPS）
		WaitTimer(20);
		// Windows システムからくる情報を処理する
		if (ProcessMessage() == -1)
		{
			break;
		}
		// ＥＳＣキーが押されたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)
		{
			break;
		}
	}
	//Dxライブラリ終了処理
	DxLib_End();

	return 0;
}

// キャラクタをマップとの当たり判定を考慮しながら移動する
int CharMove(float* X, float* Y, float* DownSP,
	float MoveX, float MoveY, float Size, char* JumpFlag)
{
	float Dummy = 0.0F;
	float hsize;

	// キャラクタの左上、右上、左下、右下部分が当たり判定のある
	// マップに衝突しているか調べ、衝突していたら補正する

	// 半分のサイズを算出
	hsize = Size * 0.5F;

	// 先ず上下移動成分だけでチェック
	{
		// 左下のチェック、もしブロックの上辺に着いていたら落下を止める
		if (MapHitCheck(*X - hsize, *Y + hsize, &Dummy, &MoveY) == 3) {
			*DownSP = 0.0F;
		}

		// 右下のチェック、もしブロックの上辺に着いていたら落下を止める
		if (MapHitCheck(*X + hsize, *Y + hsize, &Dummy, &MoveY) == 3) {
			*DownSP = 0.0F;
		}

		// 左上のチェック、もしブロックの下辺に当たっていたら落下させる
		if (MapHitCheck(*X - hsize, *Y - hsize, &Dummy, &MoveY) == 4) {
			*DownSP *= -1.0F;
		}

		// 右上のチェック、もしブロックの下辺に当たっていたら落下させる
		if (MapHitCheck(*X + hsize, *Y - hsize, &Dummy, &MoveY) == 4) {
			*DownSP *= -1.0F;
		}

		// 上下移動成分を加算
		*Y += MoveY;
	}

	// 後に左右移動成分だけでチェック
	{
		// 左下のチェック
		MapHitCheck(*X - hsize, *Y + hsize, &MoveX, &Dummy);

		// 右下のチェック
		MapHitCheck(*X + hsize, *Y + hsize, &MoveX, &Dummy);

		// 左上のチェック
		MapHitCheck(*X - hsize, *Y - hsize, &MoveX, &Dummy);

		// 右上のチェック
		MapHitCheck(*X + hsize, *Y - hsize, &MoveX, &Dummy);

		// 左右移動成分を加算
		*X += MoveX;
	}

	// 接地判定
	{
		// キャラクタの左下と右下の下に地面があるか調べる
		if (GetChipParam(*X - Size * 0.5F, *Y + Size * 0.5F + 1.0F) == 0 &&
			GetChipParam(*X + Size * 0.5F, *Y + Size * 0.5F + 1.0F) == 0)
		{
			// 足場が無かったらジャンプ中にする
			*JumpFlag = TRUE;
		}
		else
		{
			// 足場が在ったら接地中にする
			*JumpFlag = FALSE;
		}
	}

	// 終了
	return 0;
}

// マップとの当たり判定( 戻り値 0:当たらなかった  1:左辺に当たった  2:右辺に当たった
//                                                3:上辺に当たった  4:下辺に当たった
// 注意：MoveX と MoveY 、どっちか片方が０じゃないとまともに動作しません(爆)
int MapHitCheck(float pPosX, float pPosY,
	float* MoveX, float* MoveY)
{
	float playerPosX, playerPosY;

	// 移動量を足す
	playerPosX = pPosX + *MoveX;
	playerPosY = pPosY + *MoveY;

	// 当たり判定のあるブロックに当たっているかチェック
	if (GetChipParam(playerPosX, playerPosY) == 1 || GetChipParam(playerPosX, playerPosY) == 2)
	{
		float playerLeftPosX, playerTopPosY, playerRightPosX, playerButtumPosY;

		// 当たっていたら壁から離す処理を行う

		// プレイヤーの各辺の座標を算出
		playerLeftPosX = (float)((int)playerPosX / blocksize) * blocksize;        // 左辺の X 座標
		playerRightPosX = (float)((int)playerPosX / blocksize + 1) * blocksize;    // 右辺の X 座標

		playerTopPosY = (float)((int)playerPosY / blocksize) * blocksize;        // 上辺の Y 座標
		playerButtumPosY = (float)((int)playerPosY / blocksize + 1) * blocksize;    // 下辺の Y 座標

		//DrawFormatString(100, 450, GetColor(255, 255, 255), "playerPosX:%.0f", playerPosX);

		// 上辺に当たっていた場合
		if (*MoveY > 0.0F)
		{
			// 移動量を補正する
			*MoveY = playerTopPosY - pPosY - 1.0f;

			// 上辺に当たったと返す
			return 3;
		}

		// 下辺に当たっていた場合
		if (*MoveY < 0.0F)
		{
			// 移動量を補正する
			*MoveY = playerButtumPosY - pPosY + 1.0F;

			// 下辺に当たったと返す
			return 4;
		}

		// 左辺に当たっていた場合
		if (*MoveX > 0.0F)
		{
			// 移動量を補正する
			*MoveX = playerLeftPosX - pPosX - 1.0f;
			return 1;
		}

		// 右辺に当たっていた場合
		if (*MoveX < 0.0F)
		{
			// 移動量を補正する
			*MoveX = playerRightPosX - pPosX + 1.0F;

			// 右辺に当たったと返す
			return 2;
		}

		// ここに来たら適当な値を返す
		return 4;
	}

	// どこにも当たらなかったと返す
	return 0;
}

// マップチップの値を取得する関数
int GetChipParam(float pPosX, float pPosY)
{
	int x, y;

	// 整数値へ変換
	x = (int)pPosX / blocksize;
	y = (int)pPosY / blocksize;

	// マップからはみ出ていたら 0 を返す
	if (x >= MAX_MAPCHIPX || y >= MAX_MAPCHIPY || x < 0 || y < 0) {
		return 0;
	}

	// 指定の座標に該当するマップの情報を返す
	return map[y][x];
}