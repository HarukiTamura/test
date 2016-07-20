//=============================================================================
//
// サウンド処理 [sound.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM_OP = 0,		// オープニング
	SOUND_LABEL_BGM_ED,			// エンディング
	SOUND_LABEL_BGM_MENU,		// メニュー
	SOUND_LABEL_BGM_CHARACTER,	// キャラクターセレクト
	SOUND_LABEL_BGM_GAMEOVER,	// ゲームオーバー
	SOUND_LABEL_BGM_CONTINUE,	// コンティニュー
	SOUND_LABEL_BGM_PAUSE,		// ポーズ
	//STAGE
	SOUND_LABEL_BGM_STAGE1,		// STAGE1
	SOUND_LABEL_BGM_STAGE2,		// STAGE2
	SOUND_LABEL_BGM_STAGE3,		// STAGE3
	SOUND_LABEL_BGM_STAGE4,		// STAGE4
	SOUND_LABEL_BGM_STAGE5,		// STAGE5
	//BOSS
	SOUND_LABEL_BGM_BOSS1,		// BOSS1
	SOUND_LABEL_BGM_BOSS2,		// BOSS2
	SOUND_LABEL_BGM_BOSS3,		// BOSS3
	SOUND_LABEL_BGM_BOSS4,		// BOSS4
	SOUND_LABEL_BGM_BOSS5,		// BOSS5
	SOUND_LABEL_BGM_BOSS6,		// BOSS6
	SOUND_LABEL_BGM_BOSS7,		// BOSS7

	SOUND_LABEL_SE_SHOT,		// 弾発射音
	SOUND_LABEL_SE_HIT,			// ヒット音
	SOUND_LABEL_SE_MISSILESHOOT,	// 爆発音
	SOUND_LABEL_SE_MISSILEFLY,	// 爆発音
	SOUND_LABEL_SE_EXPLOSION,	// 爆発音
	SOUND_LABEL_SE_LOCKON,		// ロックオン
	SOUND_LABEL_SE_ENEMYCLOSE,	// すれ違い
	SOUND_LABEL_SE_FLYAWAY,		// 出発
	SOUND_LABEL_SE_ENTER,		// 決定
	SOUND_LABEL_SE_MISSILEFLY2,	// 爆発音
	SOUND_LABEL_MAX
} SOUND_LABEL;

typedef enum
{
	SOUND_NORMAL = 0,
	SOUND_MUTEON,
	SOUND_MUTEOFF,
	SOUND_VOLCHANGE,
	SOUND_STATUS_MAX,
} SOUND_STATUS;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);

//=============================================================================
// 自作:セグメント一時停止(ラベル指定)
//=============================================================================
void StopSoundOneTime(SOUND_LABEL label);

//=============================================================================
// 自作:セグメント一時停止解除(ラベル指定)
//=============================================================================
void PlaySoundOneMoreTime(SOUND_LABEL label);

void StopSound(void);

//=============================================================================
// 自作部：ミュート関数
// 返り値：ture 有効化　false 無効化
//=============================================================================
bool SoundMute( void );

//=============================================================================
// 自作部：再生BGM取得関数
// 返り値：SOUND_LABEL(int)	現在再生中のBGMラベル
//=============================================================================
int GetNowPlaySound( void );//未完成：正常なラベルを返さない。

#endif
