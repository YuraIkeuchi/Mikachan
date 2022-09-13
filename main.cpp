#include "Dxlib.h"
#include <algorithm> // std::max���g�p���邽��
#include<malloc.h>

const char TITLE[] = "223_���Ԃ�݂������";

const int WIN_WIDTH = 1920; //�E�B���h�E����
const int WIN_HEIGHT = 1080;//�E�B���h�E�c��

//��`�����蔻��
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
//�u���b�N�T�C�Y
int blocksize = 64;

//�V�[���ϐ�
int scene = 0;

//�}�b�v�ԍ�
int mapNumber = 0;

//�����X�^�[�̑���(�X���C��)
const int SLIME_MAX = 6;

//�����X�^�[�̑���(�R�E����)
const int BAT_MAX = 6;

//�����X�^�[�̑���(�S�[�X�g)
const int GORST_MAX = 6;

// �}�b�v�`�b�v�̒l���擾����֐�
int GetChipParam(float pPosX, float pPosY);

// �L�����N�^���}�b�v�Ƃ̓����蔻����l�����Ȃ���ړ�����֐�
int CharMove(float* X, float* Y, float* DownSP,
	float MoveX, float MoveY, float Size, char* JumpFlag);

// ���ӁFMoveX �� MoveY �A�ǂ������Е����O����Ȃ��Ƃ܂Ƃ��ɓ��삵�܂���(��)
int MapHitCheck(float pPosX, float pPosY, float* MoveX, float* MoveY);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(FALSE);						//�E�B���h�E���[�h�ɐݒ�
	//�E�B���h�E�T�C�Y���蓮�ł͕ύX�ł����A���E�B���h�E�T�C�Y�ɍ��킹�Ċg��ł��Ȃ��悤�ɂ���
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);
	SetMainWindowText(TITLE);					// �^�C�g����ύX
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);	//��ʃT�C�Y�̍ő�T�C�Y�A�J���[�r�b�g����ݒ�i���j�^�[�̉𑜓x�ɍ��킹��j
	SetWindowSizeExtendRate(1.0);				//��ʃT�C�Y��ݒ�i�𑜓x�Ƃ̔䗦�Őݒ�j
	SetBackgroundColor(0x00, 0x00, 0x00);		// ��ʂ̔w�i�F��ݒ肷��

	//Dx���C�u�����̏�����
	if (DxLib_Init() == -1) { return -1; }

	//�i�_�u���o�b�t�@�j�`���O���t�B�b�N�̈�͗��ʂ��w��
	SetDrawScreen(DX_SCREEN_BACK);

	//�摜�Ȃǂ̃��\�[�X�f�[�^�̕ϐ��錾�Ɠǂݍ���

	//�Q�[�����[�v�Ŏg���ϐ��̐錾
	char keys[256] = { 0 }; //�ŐV�̃L�[�{�[�h���p
	char oldkeys[256] = { 0 };//1���[�v�i�t���[���j�O�̃L�[�{�[�h���
	map = (int**)malloc(sizeof(int*) * MAX_MAPCHIPY);
	map[0] = (int*)malloc(MAX_MAPCHIPY * MAX_MAPCHIPX * sizeof(int));
	for (int i = 0; i < MAX_MAPCHIPY; i++) {
		map[i] = map[0] + i * MAX_MAPCHIPX;
	}

	//�}�b�v�ǂݍ���
	FILE* fp;
	FILE* fp2;
	FILE* fp3;
	FILE* fp4;
	FILE* fp5;

	fopen_s(&fp, "GameJamStage1 - �V�[�g1.csv", "r");
	fopen_s(&fp2, "GameJamStage2 - �V�[�g1.csv", "r");
	fopen_s(&fp3, "GameJamStage3 - �V�[�g1.csv", "r");
	fopen_s(&fp4, "GameJamStage4 - �V�[�g1.csv", "r");
	fopen_s(&fp5, "GameJamStage5 - �V�[�g1.csv", "r");

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

	//�v���C���[�̕ϐ�
	float PlayerPosX = 300.0f; //�v���C���[�̏������W
	float PlayerPosY = 700.0f;//�v���C���[�̏������W
	int PlayerR = 50;//���a
	float PlayerspeedX = 8.0f;	//�v���C���[�̈ړ���
	float PlayerG = 0.0f;// �v���C���[�̗������x
	char PlayerJumpFlag;// �v���C���[���W�����v�����A�̃t���O
	PlayerJumpFlag = FALSE;
	float PlayerMoveX;//�v���C���[�̈ړ���
	float PlayerMoveY;//�v���C���[�̈ړ���
	int playerDirection = 0;//�v���C���[�̌����Ă����	
	int PlayerAlive = 1;//�v���C���[�̐����t���O
	int PlayerTimer = 0;//�A�j���[�V�����֌W
	int PlayerCount = 0;
	int DeathTimer = 0;//���񂾎��̃A�j���[�V����
	int DeathCount = 0;
	int right = 0;//�Ƃ炷����
	float LeftTimer = 10.0f;//�������鎞��
	int scrollX;//�X�N���[���l
	scrollX = PlayerPosX - 960;

	//�X���C���̕ϐ�
	float SlimePosX[SLIME_MAX];//�ʒu
	float SlimePosY[SLIME_MAX];
	int SlimeR[SLIME_MAX];//���a
	int SlimeAlive[SLIME_MAX];//�����t���O
	int SlimeTimer[SLIME_MAX];//�A�j���[�V�����֌W
	int SlimeCount[SLIME_MAX];
	float SlimeG[SLIME_MAX];// �X���C���̗������x
	float SlimeMoveX[SLIME_MAX];//�X���C���̈ړ���
	float SlimeMoveY[SLIME_MAX];
	char SlimeJumpFlag[SLIME_MAX];// �X���C�����W�����v�����A�̃t���O
	int SlimeMoveFlag[SLIME_MAX];//�x�̌����ɓ������̃t���O

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

	//�X���C���̈ʒu�̏�����
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

	//�R�E�����̕ϐ�
	float BatPosX[BAT_MAX];//�ʒu
	float BatPosY[BAT_MAX];
	int BatR[BAT_MAX];//���a
	int BatAlive[BAT_MAX];//�����t���O
	int BatTimer[BAT_MAX];//�A�j���[�V�����֌W
	int BatCount[BAT_MAX];
	float BatG[BAT_MAX];// �R�E�����̗������x
	float BatMoveX[BAT_MAX];//�R�E�����̈ړ���
	float BatMoveY[BAT_MAX];
	char BatJumpFlag[BAT_MAX];// �R�E�������W�����v�����A�̃t���O
	int BatMoveFlag[BAT_MAX];//�x�̌����ɓ������̃t���O

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

	//�R�E�����̈ʒu�̏�����
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

	//���΂��̃����X�^�[(��)�̕ϐ�
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
		//���΂��̃����X�^�[(��)�̕ϐ�
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

	//�R�E�����̈ʒu�̏�����
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

	//���΂��̃����X�^�[(��)�̕ϐ�
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

	//���y�֌W�̕ϐ�
	int TopPositionFlag = 0;
	int TopPositionFlag2 = 0;
	int TopPositionFlag3 = 0;
	int Volume = 120;
	int Volume2 = 0;
	//�^�C�g����ʂ̕ϐ�
	int TitleTimer = 0;
	int TitleCount = 0;
	//������ʂ̕ϐ�
	int RuleTimer = 0;
	int RuleCount = 0;
	//�Q�[���N���A��ʂ̕ϐ�
	int ClearTimer = 0;
	int ClearCount = 0;
	//���\�[�X�t�@�C��
	//�݂������
	int LeftMika[6] = { 0 };
	LoadDivGraph("Resources/mikatyan_left.png", 6, 6, 1, 100, 100, LeftMika);
	int RightMika[6] = { 0 };
	LoadDivGraph("Resources/mikatyan_Right.png", 6, 6, 1, 100, 100, RightMika);
	//�W�����v��
	int RightJump;
	RightJump = LoadGraph("Resources/mikatyan_jump.png");
	int LeftJump;
	LeftJump = LoadGraph("Resources/cam1.png");
	//���񂾂Ƃ�
	int DeathMika[9] = { 0 };
	LoadDivGraph("Resources/mikaDeth.png", 9, 9, 1, 160, 100, DeathMika);
	//�G
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
	//�w�i
	int haikei;
	haikei = LoadGraph("Resources/backscreen.png");

	//�}�b�v�`�b�v
	int mapchip[3];
	mapchip[0] = LoadGraph("Resources/blockss.png");
	mapchip[1] = LoadGraph("Resources/lightblock.png");
	mapchip[2] = LoadGraph("Resources/goal.png");
	//UI
	int UI = LoadGraph("Resources/UI.png");
	int UI2 = LoadGraph("Resources/UI2.png");

	//�^�C�g��
	int Title[9] = { 0 };
	LoadDivGraph("Resources/title.png", 9, 9, 1, 1920, 1080, Title);
	//����
	int Rule[8] = { 0 };
	LoadDivGraph("Resources/setumei.png", 8, 8, 1, 1920, 1080, Rule);
	//�Q�[���I�[�o�[
	int GameOver;
	GameOver = LoadGraph("Resources/gameOver (1).png");
	//�X�e�[�W�N���A
	int Stageclear;
	Stageclear = LoadGraph("Resources/stageClear.png");
	//�Q�[���N���A
	int Clear[6] = { 0 };
	LoadDivGraph("Resources/clear.png", 6, 6, 1, 1920, 1080, Clear);

	//���y
	int TitleBgm = LoadSoundMem("Sound/title.mp3");
	int StageBgm = LoadSoundMem("Sound/gamePlay.mp3");
	int StageBgm2 = LoadSoundMem("Sound/gamePlay2.mp3");
	int JumpSE = LoadSoundMem("Sound/se_jump.mp3");
	int ClearBGM = LoadSoundMem("Sound/sucess2.mp3");

	// �Q�[�����[�v
	while (1)
	{
		//�ŐV�̃L�[�{�[�h��񂾂������̂͂P�t���[���O�̃L�[�{�[�h���Ƃ��ĕۑ�

		for (int i = 0; i < 256; i++) {
			oldkeys[i] = keys[i];
		}
		//�ŐV�̃L�[�{�[�h�����擾
		GetHitKeyStateAll(keys);

		//��ʃN���A
		ClearDrawScreen();
		//---------  ��������v���O�������L�q  ----------//

		//�V�[��0
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
		//�V�[��2
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

			//�X�V����
			//�e�ړ��ʂ̏�����
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

			//�v���C���[�̃A�j���[�V����

			if (PlayerTimer >= 5) {
				PlayerCount++;
				PlayerTimer = 0;
			}

			if (PlayerCount == 6) {
				PlayerCount = 0;
			}

			//���΂��̃A�j���[�V����
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

			//�X���C���̃A�j���[�V����
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

			//�R�E�����̃A�j���[�V����
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
			// �v���C���[�̈ړ�����
			   // ���E�̈ړ�������
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

			//�~�܂��Ă��鎞�̏���
			if (PlayerMoveX == 0.0f) {
				if (playerDirection == 1) {
					PlayerCount = 3;
				}
				else {
					PlayerCount = 2;
				}
			}

			//���X�ɏƂ炷
			if (playerDirection == 0 && right >= 0) {
				right -= 1;
			}

			//���X�ɏƂ炷
			else if (playerDirection == 1 && right <= 254 && LeftTimer >= 0.05) {
				right++;
				if (LeftTimer >= 0.003) {
					LeftTimer -= 0.003;
				}
			}

			//�W�����v
			if (PlayerJumpFlag == FALSE && keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0 && PlayerAlive == 1)
			{
				PlaySoundMem(JumpSE, DX_PLAYTYPE_BACK, TRUE);
				ChangeVolumeSoundMem(180 * 30 / 60, JumpSE);
				PlayerG = -9.0;
				PlayerJumpFlag = TRUE;
			}

			// ��������
			PlayerG += 0.3;

			// �������x���ړ��ʂɉ�����
			PlayerMoveY = PlayerG;

			//�X���C���̋���
			for (int i = 0; i < SLIME_MAX; i++) {
				if (SlimeAlive[i] == 1) {
					SlimeG[i] += 0.3;
				}
				// �������x���ړ��ʂɉ�����
				SlimeMoveY[i] = SlimeG[i];

				//�X���C�����܂�Ԃ����̓���
				if (SlimeMoveFlag[i] == 0) {
					SlimeMoveX[i] += 3.0f;
				}

				else if (SlimeMoveFlag[i] == 1) {
					SlimeMoveX[i] -= 3.0f;
				}
			}

			//�R�E�����̋���
			for (int i = 0; i < BAT_MAX; i++) {
				if (BatAlive[i] == 1) {
					BatG[i] += 0.3;
				}
				// �������x���ړ��ʂɉ�����
				BatMoveY[i] = BatG[i];

				//�X���C�����܂�Ԃ����̓���
				if (BatMoveFlag[i] == 0) {
					BatMoveX[i] += 3.0f;
				}

				else if (BatMoveFlag[i] == 1) {
					BatMoveX[i] -= 3.0f;
				}
			}

			//�X���C��
			for (int i = 0; i < SLIME_MAX; i++) {
				//�E�̕ǂɂ��������Ƃ�
				if (map[(static_cast<int>(SlimePosY[i] - SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] + SlimeR[i])) / 64] != 0 &&
					map[(static_cast<int>(SlimePosY[i] + SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] + SlimeR[i] - 1)) / 64] != 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 1;
				}

				//���̕ǂɂ��������Ƃ�
				if (map[(static_cast<int>(SlimePosY[i] - SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] - SlimeR[i])) / 64] != 0 &&
					map[(static_cast<int>(SlimePosY[i] + SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] - SlimeR[i])) / 64] != 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 0;
				}

				//�E���Ƀu���b�N���Ȃ������Ƃ�
				if (map[(static_cast<int>(SlimePosY[i] + SlimeR[i])) / 64][(static_cast<int>(SlimePosX[i] + SlimeR[i] - 1)) / 64] == 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 1;
				}

				//�����Ƀu���b�N���Ȃ������Ƃ�
				if (map[(static_cast<int>(SlimePosY[i] + SlimeR[i] - 1)) / 64][(static_cast<int>(SlimePosX[i] - SlimeR[i])) / 64] == 0
					&& SlimeAlive[i] == 1) {
					SlimeMoveFlag[i] = 0;
				}
			}

			//�R�E����
			for (int i = 0; i < BAT_MAX; i++) {
				//�E�̕ǂɂ��������Ƃ�
				if (map[(static_cast<int>(BatPosY[i] - BatR[i])) / 64][(static_cast<int>(BatPosX[i] + BatR[i])) / 64] != 0 &&
					map[(static_cast<int>(BatPosY[i] + BatR[i])) / 64][(static_cast<int>(BatPosX[i] + BatR[i] - 1)) / 64] != 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 1;
				}

				//���̕ǂɂ��������Ƃ�
				if (map[(static_cast<int>(BatPosY[i] - BatR[i])) / 64][(static_cast<int>(BatPosX[i] - BatR[i])) / 64] != 0 &&
					map[(static_cast<int>(BatPosY[i] + BatR[i])) / 64][(static_cast<int>(BatPosX[i] - BatR[i])) / 64] != 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 0;
				}

				//�E���Ƀu���b�N���Ȃ������Ƃ�
				if (map[(static_cast<int>(BatPosY[i] + BatR[i])) / 64][(static_cast<int>(BatPosX[i] + BatR[i] - 1)) / 64] == 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 1;
				}

				//�����Ƀu���b�N���Ȃ������Ƃ�n
				if (map[(static_cast<int>(BatPosY[i] + BatR[i] - 1)) / 64][(static_cast<int>(BatPosX[i] - BatR[i])) / 64] == 0
					&& BatAlive[i] == 1) {
					BatMoveFlag[i] = 0;
				}
			}
			////�S�[�X�g
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

			//�G�ƃv���C���[�̓����蔻��
			//�S�[�X�g(��)
			for (int i = 0; i < GORST_MAX; i++) {
				if (collision(PlayerPosX - PlayerR - scrollX, PlayerPosY - PlayerR - scrollX,
					PlayerPosX + PlayerR - scrollX, PlayerPosY + PlayerR - scrollX,
					GorstPosX[i] - (GorstR[i] - 40) - scrollX, GorstPosY[i] - (GorstR[i] - 40) - scrollX,
					GorstPosX[i] + (GorstR[i] - 40) - scrollX, GorstPosY[i] + (GorstR[i] - 40) - scrollX) == 1
					&& GorstAlive[i] == 1) {
					PlayerAlive = 0;
				}
			}

			//�X���C��
			for (int i = 0; i < SLIME_MAX; i++) {
				if (collision(PlayerPosX - PlayerR - scrollX, PlayerPosY - PlayerR - scrollX,
					PlayerPosX + PlayerR - scrollX, PlayerPosY + PlayerR - scrollX,
					SlimePosX[i] - (SlimeR[i] - 40) - scrollX, SlimePosY[i] - (SlimeR[i] - 40) - scrollX,
					SlimePosX[i] + (SlimeR[i] - 40) - scrollX, SlimePosY[i] + (SlimeR[i] - 40) - scrollX) == 1
					&& SlimeAlive[i] == 1) {
					PlayerAlive = 0;
				}
			}

			//�R�E����
			for (int i = 0; i < GORST_MAX; i++) {
				if (collision(PlayerPosX - PlayerR - scrollX, PlayerPosY - PlayerR - scrollX,
					PlayerPosX + PlayerR - scrollX, PlayerPosY + PlayerR - scrollX,
					BatPosX[i] - (BatR[i] - 40) - scrollX, BatPosY[i] - (BatR[i] - 40) - scrollX,
					BatPosX[i] + (BatR[i] - 40) - scrollX, BatPosY[i] + (BatR[i] - 40) - scrollX) == 1
					&& BatAlive[i] == 1) {
					PlayerAlive = 0;
				}
			}

			//�G�ƃv���C���[�̓����蔻��
			//�S�[�X�g(��)
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

			//�S�[���ɂ����������̃V�[���Ǘ�
			if (GetChipParam(PlayerPosX, PlayerPosY) == 3) {
				if (mapNumber != 4) {
					scene = 4;
				}

				else {
					scene = 5;
				}
			}

			// �ړ��ʂɊ�Â��ăL�����N�^�̍��W���ړ�
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

			//�傫�Ȃ��΂����o�����
			if (PlayerPosX > 6000 && mapNumber == 2) {
				Gorst2Alive = 1;
			}

			//�傫�Ȃ��΂����o�����
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

			//���y��ς���
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

		//�V�[��3(�Q�[���I�[�o�[�j
		else if (scene == 3) {

			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				scene = 2;
				ChangeVolumeSoundMem(Volume * 50 / 50, StageBgm);
				PlaySoundMem(StageBgm, DX_PLAYTYPE_LOOP, TopPositionFlag2 = 1);
			}
			//�v���C���[�̕ϐ�
			PlayerPosX = 300.0f; //�v���C���[�̏������W
			PlayerPosY = 700.0f; //�v���C���[�̏������W
			PlayerR = 50;//���a
			PlayerspeedX = 8.0f;	//�v���C���[�̈ړ���
			PlayerG = 0.0f;// �v���C���[�̗������x
			PlayerJumpFlag = FALSE;
			playerDirection = 0;//�v���C���[�̌����Ă����	
			PlayerAlive = 1;//�v���C���[�̐����t���O
			PlayerTimer = 0;//�A�j���[�V�����֌W
			PlayerCount = 0;
			DeathTimer = 0;//���񂾎��̃A�j���[�V����
			DeathCount = 0;
			right = 0;//�Ƃ炷����
			LeftTimer = 10.0f;//�������鎞��
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

			//�X���C���̈ʒu�̏�����
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

			//�R�E�����̈ʒu�̏�����
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
				//���΂��̃����X�^�[(��)�̕ϐ�
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

			//�R�E�����̈ʒu�̏�����
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

			//���΂��̃����X�^�[(��)�̕ϐ�
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
			//�^�C�g����ʂ̕ϐ�
			TitleTimer = 0;
			TitleCount = 0;
			//������ʂ̕ϐ�
			RuleTimer = 0;
			RuleCount = 0;
		}

		//�V�[��4�i�X�e�[�W�N���A)
		else if (scene == 4) {
			StopSoundMem(StageBgm);
			StopSoundMem(StageBgm2);
			if (keys[KEY_INPUT_SPACE] == 1 && oldkeys[KEY_INPUT_SPACE] == 0) {
				ChangeVolumeSoundMem(Volume * 50 / 50, StageBgm);
				PlaySoundMem(StageBgm, DX_PLAYTYPE_LOOP, TopPositionFlag2 = 1);
				mapNumber++;
				scene = 2;
				//�ϐ��̍ēx������
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


				//�X���C���̈ʒu�̏�����
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

				//�R�E�����̈ʒu�̏�����
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
					//���΂��̃����X�^�[(��)�̕ϐ�
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

				//���΂��̈ʒu�̏�����
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

		//�V�[��4�i�Q�[���N���A)
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
		//�`�揈��
		//�V�[��0
		if (scene == 0) {
			DrawGraph(0, 0, Title[TitleCount], TRUE);
		}
		//�V�[��1
		else if (scene == 1) {
			DrawGraph(0, 0, Rule[RuleCount], TRUE);
		}
		//�V�[��2
		else if (scene == 2) {
			//�w�i�̕`��
			SetDrawBright(right + 80, right + 80, right + 80);
			DrawGraph(-1920 - scrollX, 0, haikei, TRUE);
			for (int i = 0; i < 10; i++) {
				DrawGraph(1920 * i - scrollX, 0, haikei, TRUE);
			}
			SetDrawBright(255, 255, 255);
			//�}�b�v�z�u
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

			//������̃Q�[�W
			DrawLineAA((PlayerPosX - PlayerR) + (LeftTimer / 10) * 60 - scrollX, PlayerPosY - 30, PlayerPosX - PlayerR - scrollX, PlayerPosY - 30, GetColor(50, 200, 200), 10);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, right);
			//�X���C��
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

			//�R�E����
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
			//���΂�(��)
			for (int i = 0; i < GORST_MAX; i++) {
				if (GorstAlive[i] == 1) {
					DrawExtendGraph(GorstPosX[i] - GorstR[i] - scrollX, GorstPosY[i] - GorstR[i],
						GorstPosX[i] + GorstR[i] - scrollX, GorstPosY[i] + GorstR[i],
						obake[GorstCount[i]], TRUE);
				}
			}

			//���΂�(��)
			if (Gorst2Alive == 1) {
				SetDrawBright(255, Gorst2Bright, Gorst2Bright);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, Gorst2Alpha);
				DrawExtendGraph(Gorst2PosX - GorstR2 - scrollX, Gorst2PosY - GorstR2,
					Gorst2PosX + GorstR2 - scrollX, Gorst2PosY + GorstR2,
					obake2[GorstCount[0]], TRUE);
			}

			SetDrawBright(255, 255, 255);

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			//�v���C���[
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

		//�V�[��3
		else if (scene == 3) {
			DrawGraph(0, 0, GameOver, TRUE);
		}

		//�V�[��4
		else if (scene == 4) {
			DrawGraph(0, 0, Stageclear, TRUE);
		}

		//�V�[��5
		else if (scene == 5) {
			DrawGraph(0, 0, Clear[ClearCount], TRUE);
		}

		//---------  �����܂łɃv���O�������L�q  ---------//
		ScreenFlip();//�i�_�u���o�b�t�@�j����
		// 20�~���b�ҋ@�i�^��60FPS�j
		WaitTimer(20);
		// Windows �V�X�e�����炭�������������
		if (ProcessMessage() == -1)
		{
			break;
		}
		// �d�r�b�L�[�������ꂽ�烋�[�v���甲����
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)
		{
			break;
		}
	}
	//Dx���C�u�����I������
	DxLib_End();

	return 0;
}

// �L�����N�^���}�b�v�Ƃ̓����蔻����l�����Ȃ���ړ�����
int CharMove(float* X, float* Y, float* DownSP,
	float MoveX, float MoveY, float Size, char* JumpFlag)
{
	float Dummy = 0.0F;
	float hsize;

	// �L�����N�^�̍���A�E��A�����A�E�������������蔻��̂���
	// �}�b�v�ɏՓ˂��Ă��邩���ׁA�Փ˂��Ă�����␳����

	// �����̃T�C�Y���Z�o
	hsize = Size * 0.5F;

	// �悸�㉺�ړ����������Ń`�F�b�N
	{
		// �����̃`�F�b�N�A�����u���b�N�̏�ӂɒ����Ă����痎�����~�߂�
		if (MapHitCheck(*X - hsize, *Y + hsize, &Dummy, &MoveY) == 3) {
			*DownSP = 0.0F;
		}

		// �E���̃`�F�b�N�A�����u���b�N�̏�ӂɒ����Ă����痎�����~�߂�
		if (MapHitCheck(*X + hsize, *Y + hsize, &Dummy, &MoveY) == 3) {
			*DownSP = 0.0F;
		}

		// ����̃`�F�b�N�A�����u���b�N�̉��ӂɓ������Ă����痎��������
		if (MapHitCheck(*X - hsize, *Y - hsize, &Dummy, &MoveY) == 4) {
			*DownSP *= -1.0F;
		}

		// �E��̃`�F�b�N�A�����u���b�N�̉��ӂɓ������Ă����痎��������
		if (MapHitCheck(*X + hsize, *Y - hsize, &Dummy, &MoveY) == 4) {
			*DownSP *= -1.0F;
		}

		// �㉺�ړ����������Z
		*Y += MoveY;
	}

	// ��ɍ��E�ړ����������Ń`�F�b�N
	{
		// �����̃`�F�b�N
		MapHitCheck(*X - hsize, *Y + hsize, &MoveX, &Dummy);

		// �E���̃`�F�b�N
		MapHitCheck(*X + hsize, *Y + hsize, &MoveX, &Dummy);

		// ����̃`�F�b�N
		MapHitCheck(*X - hsize, *Y - hsize, &MoveX, &Dummy);

		// �E��̃`�F�b�N
		MapHitCheck(*X + hsize, *Y - hsize, &MoveX, &Dummy);

		// ���E�ړ����������Z
		*X += MoveX;
	}

	// �ڒn����
	{
		// �L�����N�^�̍����ƉE���̉��ɒn�ʂ����邩���ׂ�
		if (GetChipParam(*X - Size * 0.5F, *Y + Size * 0.5F + 1.0F) == 0 &&
			GetChipParam(*X + Size * 0.5F, *Y + Size * 0.5F + 1.0F) == 0)
		{
			// ���ꂪ����������W�����v���ɂ���
			*JumpFlag = TRUE;
		}
		else
		{
			// ���ꂪ�݂�����ڒn���ɂ���
			*JumpFlag = FALSE;
		}
	}

	// �I��
	return 0;
}

// �}�b�v�Ƃ̓����蔻��( �߂�l 0:������Ȃ�����  1:���ӂɓ�������  2:�E�ӂɓ�������
//                                                3:��ӂɓ�������  4:���ӂɓ�������
// ���ӁFMoveX �� MoveY �A�ǂ������Е����O����Ȃ��Ƃ܂Ƃ��ɓ��삵�܂���(��)
int MapHitCheck(float pPosX, float pPosY,
	float* MoveX, float* MoveY)
{
	float playerPosX, playerPosY;

	// �ړ��ʂ𑫂�
	playerPosX = pPosX + *MoveX;
	playerPosY = pPosY + *MoveY;

	// �����蔻��̂���u���b�N�ɓ������Ă��邩�`�F�b�N
	if (GetChipParam(playerPosX, playerPosY) == 1 || GetChipParam(playerPosX, playerPosY) == 2)
	{
		float playerLeftPosX, playerTopPosY, playerRightPosX, playerButtumPosY;

		// �������Ă�����ǂ��痣���������s��

		// �v���C���[�̊e�ӂ̍��W���Z�o
		playerLeftPosX = (float)((int)playerPosX / blocksize) * blocksize;        // ���ӂ� X ���W
		playerRightPosX = (float)((int)playerPosX / blocksize + 1) * blocksize;    // �E�ӂ� X ���W

		playerTopPosY = (float)((int)playerPosY / blocksize) * blocksize;        // ��ӂ� Y ���W
		playerButtumPosY = (float)((int)playerPosY / blocksize + 1) * blocksize;    // ���ӂ� Y ���W

		//DrawFormatString(100, 450, GetColor(255, 255, 255), "playerPosX:%.0f", playerPosX);

		// ��ӂɓ������Ă����ꍇ
		if (*MoveY > 0.0F)
		{
			// �ړ��ʂ�␳����
			*MoveY = playerTopPosY - pPosY - 1.0f;

			// ��ӂɓ��������ƕԂ�
			return 3;
		}

		// ���ӂɓ������Ă����ꍇ
		if (*MoveY < 0.0F)
		{
			// �ړ��ʂ�␳����
			*MoveY = playerButtumPosY - pPosY + 1.0F;

			// ���ӂɓ��������ƕԂ�
			return 4;
		}

		// ���ӂɓ������Ă����ꍇ
		if (*MoveX > 0.0F)
		{
			// �ړ��ʂ�␳����
			*MoveX = playerLeftPosX - pPosX - 1.0f;
			return 1;
		}

		// �E�ӂɓ������Ă����ꍇ
		if (*MoveX < 0.0F)
		{
			// �ړ��ʂ�␳����
			*MoveX = playerRightPosX - pPosX + 1.0F;

			// �E�ӂɓ��������ƕԂ�
			return 2;
		}

		// �����ɗ�����K���Ȓl��Ԃ�
		return 4;
	}

	// �ǂ��ɂ�������Ȃ������ƕԂ�
	return 0;
}

// �}�b�v�`�b�v�̒l���擾����֐�
int GetChipParam(float pPosX, float pPosY)
{
	int x, y;

	// �����l�֕ϊ�
	x = (int)pPosX / blocksize;
	y = (int)pPosY / blocksize;

	// �}�b�v����͂ݏo�Ă����� 0 ��Ԃ�
	if (x >= MAX_MAPCHIPX || y >= MAX_MAPCHIPY || x < 0 || y < 0) {
		return 0;
	}

	// �w��̍��W�ɊY������}�b�v�̏���Ԃ�
	return map[y][x];
}