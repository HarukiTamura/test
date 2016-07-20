//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM_OP = 0,		// �I�[�v�j���O
	SOUND_LABEL_BGM_ED,			// �G���f�B���O
	SOUND_LABEL_BGM_MENU,		// ���j���[
	SOUND_LABEL_BGM_CHARACTER,	// �L�����N�^�[�Z���N�g
	SOUND_LABEL_BGM_GAMEOVER,	// �Q�[���I�[�o�[
	SOUND_LABEL_BGM_CONTINUE,	// �R���e�B�j���[
	SOUND_LABEL_BGM_PAUSE,		// �|�[�Y
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

	SOUND_LABEL_SE_SHOT,		// �e���ˉ�
	SOUND_LABEL_SE_HIT,			// �q�b�g��
	SOUND_LABEL_SE_MISSILESHOOT,	// ������
	SOUND_LABEL_SE_MISSILEFLY,	// ������
	SOUND_LABEL_SE_EXPLOSION,	// ������
	SOUND_LABEL_SE_LOCKON,		// ���b�N�I��
	SOUND_LABEL_SE_ENEMYCLOSE,	// ����Ⴂ
	SOUND_LABEL_SE_FLYAWAY,		// �o��
	SOUND_LABEL_SE_ENTER,		// ����
	SOUND_LABEL_SE_MISSILEFLY2,	// ������
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
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);

//=============================================================================
// ����:�Z�O�����g�ꎞ��~(���x���w��)
//=============================================================================
void StopSoundOneTime(SOUND_LABEL label);

//=============================================================================
// ����:�Z�O�����g�ꎞ��~����(���x���w��)
//=============================================================================
void PlaySoundOneMoreTime(SOUND_LABEL label);

void StopSound(void);

//=============================================================================
// ���암�F�~���[�g�֐�
// �Ԃ�l�Fture �L�����@false ������
//=============================================================================
bool SoundMute( void );

//=============================================================================
// ���암�F�Đ�BGM�擾�֐�
// �Ԃ�l�FSOUND_LABEL(int)	���ݍĐ�����BGM���x��
//=============================================================================
int GetNowPlaySound( void );//�������F����ȃ��x����Ԃ��Ȃ��B

#endif
