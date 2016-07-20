/*******************************************************************************
* �^�C�g��	:�Q�[��.cpp
* �t�@�C����:Game.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/05/29
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "game.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

#include "cursor.h"
#include "backGround.h"
#include "backGround2.h"
#include "backGround3.h"
#include "backGround4.h"
#include "backGround5.h"
#include "player01.h"
#include "barrier.h"

#include "explosion.h"
#include "enemy.h"
#include "bullet.h"
#include "sideCharacter.h"
#include "startCounter.h"
#include "frameLeft.h"
#include "frameRight.h"

#include "hystericGauge.h"
#include "timer.h"
#include "score.h"
#include "comboNumber.h"
#include "comboUi.h"
#include "pauseGraphic.h"
#include "stageManager.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static GAME_SEQUENCE g_gameSequence;
static int soundLabelBufferPause;
static int timerFpsSwitch;
/*******************************************************************************
* �֐���	:HRESULT InitGame( void )
*		�Q�[���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitGame( void )
{
	
	/*�w�i�̐ݒ�*/
	if( InitBackGround() != S_OK )
	{
		Error("�w�i�f�[�^�̏������Ɏ��s���܂����B");
	}
	if( InitBackGround2() != S_OK )
	{
		Error("�w�i�f�[�^2�̏������Ɏ��s���܂����B");
	}
	if( InitBackGround3() != S_OK )
	{
		Error("�w�i�f�[�^3�̏������Ɏ��s���܂����B");
	}
	if( InitBackGround4() != S_OK )
	{
		Error("�w�i�f�[�^4�̏������Ɏ��s���܂����B");
	}
	if( InitBackGround5() != S_OK )
	{
		Error("�w�i�f�[�^5�̏������Ɏ��s���܂����B");
	}
	/*�|���S���̐ݒ�*/
	if( InitCursor() != S_OK )
	{
		Error("�|���S���̏������Ɏ��s���܂����B");
	}
	/*�v���C���[�̐ݒ�*/
	if( InitPlayer01() != S_OK )
	{
		Error("�v���C���[�̏������Ɏ��s���܂����B");
	}
	/*�o���A�[�̐ݒ�*/
	if( InitBarrier() != S_OK )
	{
		Error("�o���A�[�̏������Ɏ��s���܂����B");
	}
	/*�e�̐ݒ�*/
	if( InitBullet() != S_OK )
	{
		Error("�e�̏������Ɏ��s���܂����B");
	}
	/*�����̐ݒ�*/
	if( InitExplosion() != S_OK )
	{
		Error("�����̏������Ɏ��s���܂����B");
	}
	/*�e�̐ݒ�*/
	if( InitEnemy() != S_OK )
	{
		Error("�e�̏������Ɏ��s���܂����B");
	}
	/*�t�B�[���h�g�̐ݒ�*/
	if( InitFrameLeft() != S_OK )
	{
		Error("���t���[���g�̏������Ɏ��s���܂����B");
	}
	if( InitFrameRight() != S_OK )
	{
		Error("�E�t���[���g�̏������Ɏ��s���܂����B");
	}
	/*�L�����N�^�[�w�i�̐ݒ�*/
	if( InitSideCharacter() != S_OK )
	{
		Error("�L�����N�^�[�w�i�̏������Ɏ��s���܂����B");
	}
	/*�q�X�e���b�N�Q�[�W�̐ݒ�*/
	if( InitHystericGauge() != S_OK )
	{
		Error("�Q�[�WUI�̏������Ɏ��s���܂����B");
	}
	/*�^�C�}�[�̐ݒ�*/
	if( InitTimer() != S_OK )
	{
		Error("�^�C�}�[�̏������Ɏ��s���܂����B");
	}
	/*�X�R�A�̐ݒ�*/
	if( InitScore() != S_OK )
	{
		Error("�X�R�A�̏������Ɏ��s���܂����B");
	}
	/*�R���{���̐ݒ�*/
	if( InitComboNumber() != S_OK )
	{
		Error("�R���{���̏������Ɏ��s���܂����B");
	}
	/*�R���{�\���̐ݒ�*/
	if( InitComboUi() != S_OK )
	{
		Error("�R���{�\���̏������Ɏ��s���܂����B");
	}
	/*�|�[�Y�\���̐ݒ�*/
	if( InitPauseGraphic() != S_OK )
	{
		Error("�|�[�Y�\���̏������Ɏ��s���܂����B");
	}
	/*�X�^�[�g�J�E���g�̐ݒ�*/
	if( InitStartCounter() != S_OK )
	{
		Error("�X�^�[�g�J�E���g�̏������Ɏ��s���܂����B");
	}
	//�X�e�[�W�}�l�[�W���[�̐ݒ�
	if( InitStageManager() != S_OK )
	{
		Error("�X�e�[�W�}�l�[�W���[�̏������Ɏ��s���܂����B");
	}

	g_gameSequence = SEQUENCE_PLAY;		//��������Update�ʂ�O�񂾂����ׁA�ً}�����
	UpdateGame();
	
	/*�^�C�}�[�[���Œ�*/
	resetTimer( 0 );
	UpdateTimer();
	g_gameSequence = SEQUENCE_COUNTDOWN;
#ifndef _DEBUG
	g_gameSequence = SEQUENCE_COUNTDOWN;
#endif
	timerFpsSwitch = 0;

	PlaySound(SOUND_LABEL_BGM_STAGE1);
	soundLabelBufferPause = SOUND_LABEL_BGM_STAGE1;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitGame( void )
*		�Q�[���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitGame( void )
{
	/*�X�e�[�W�}�l�[�W���[�̉��*/
	UninitStageManager();

	/*�w�i�̊J��*/
	UninitBackGround();
	UninitBackGround2();
	UninitBackGround3();
	UninitBackGround4();
	UninitBackGround5();

	/*�|���S���̊J��*/
	UninitCursor();
	
	/*�o���A�[�̉��*/
	UninitBarrier();

	/*�v���C���[�̉��*/
	UninitPlayer01();
	
	/*�t�B�[���h�g�̉��*/
	UninitFrameLeft();
	UninitFrameRight();

	/*�L�����N�^�[�w�i�̉��*/
	UninitSideCharacter();

	/*�q�X�e���b�N�Q�[�W�̉��*/
	UninitHystericGauge();

	
	/*�^�C�}�[�̉��*/
	UninitTimer();

	/*�X�R�A�̉��*/
	UninitScore();
	
	/*�R���{���̉��*/
	UninitComboNumber();

	/*�R���{�\���̉��*/
	UninitComboUi();

	/*�����̉��*/
	UninitExplosion();

	/*�e�̉��*/
	UninitBullet();

	/*�G�l�~�[�̉��*/
	UninitEnemy();

	/*�|�[�Y�\���̉��*/
	UninitPauseGraphic();

	/*�X�^�[�g�J�E���g�̊J��*/
	UninitStartCounter();
}

/*******************************************************************************
* �֐���	:void UpdateGame( void )
*		�Q�[���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateGame( void )
{
	int clearFlag = false;

	
#ifdef _DEBUG

	if( GetKeyboardTrigger( DIK_E) )
	{
		SetBulletPlayer( 500000 , 12 , 0 , 0 , -10000 , 0 , 12 );
	}
	if( GetKeyboardTrigger( DIK_R ) )
	{
		SetBulletLiar( 500000 ,12, 0 , 0 , -10000 , 0 , 12 , BULLET_HOMING );
	}
	if( GetKeyboardTrigger( DIK_1 ) )
	{
		SetEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 50 , CATEGORY_ENEMY , ENEMY_002 );
		//	SetEnemyEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
	}
	
	if( GetKeyboardTrigger( DIK_2 ) )
	{
		SetEnemy( 400000 , 100000 , 0 ,0 , 0 , 0 , 50 , CATEGORY_ENEMY , ENEMY_002 );
		//	SetEnemyEnemy( 400000 , 100000 , 0 ,0 , 100 , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
	}
	
	if( GetKeyboardPress( DIK_T ) )
	{
		for( int i = 0; i<= 40; i++ )
		{
			SetBulletEnemy( 400000 , 100 , 0 , 5900 * sin( D3DX_PI * 0.05 * i + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * i + 0.01 ) , 0 , 12 );
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
		}
	}
	if( GetKeyboardPress( DIK_Y ) )
	{
		for( int i = 0; i<= 40; i++ )
		{
			SetBulletLiar( 400000 , 30000 , 0 , 5900 * sin( D3DX_PI * 0.05 * i + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * i + 0.01 ) , 0 , 12 , BULLET_NORMAL);
		//	SetBullet( D3DXVECTOR3( 10 + 20*i  , 50 , 0.0f ) , D3DXVECTOR3( 0,4,0.0f ) , 12 );
		}
	}

	if( GetKeyboardPress( DIK_UP ) )
	{	
		PlusScore( 12345678 );
	}
	if( GetKeyboardPress( DIK_DOWN ) )
	{
		PlusScore( -12345 );
	}


#endif

	switch( g_gameSequence )
	{
		case SEQUENCE_COUNTDOWN :
			{
				/*�X�^�[�g�J�E���g�̍X�V*/

				/*�^�C�}�[�[���Œ�*/
				resetTimer( 0 );

				/*�w�i�̍X�V*/
				UpdateBackGround();
				UpdateBackGround2();
				UpdateBackGround3();
				UpdateBackGround4();
				UpdateBackGround5();
				
				/*�t�B�[���h�g�̍X�V*/
				UpdateFrameLeft();
				UpdateFrameRight();

				/*�L�����N�^�[�w�i�̍X�V*/
				UpdateSideCharacter();

				if( UpdateStartCounter() == true )//�J�E���g���Ă���Œ��͓����Ȃ��i�uGO�I�v����͓�����j
				{
					break;
				}
				g_gameSequence = SEQUENCE_PLAY;
			}
		case SEQUENCE_PLAY :
			{
				if( g_gameSequence == SEQUENCE_PLAY )
				{
					timerFpsSwitch = ( timerFpsSwitch + 1 == 3 ? 0 : timerFpsSwitch + 1 );
					if( timerFpsSwitch % 3 == 0 )
					{
						PlusTimer( 16 );
					}
					else
					{
						PlusTimer( 17 );
					}
					clearFlag = UpdateStageManager();
				}
				if( GetKeyboardTrigger( DIK_P ) || GetGamePadTrigger( 9 ) )
				{
					StopSoundOneTime( ( SOUND_LABEL )( soundLabelBufferPause ) );
					g_gameSequence = SEQUENCE_PAUSE;
					PlaySound( SOUND_LABEL_BGM_PAUSE );
					break;
				}
				UpdateStartCounter();

				/*�w�i�̍X�V*/
				UpdateBackGround();
				UpdateBackGround2();
				UpdateBackGround3();
				UpdateBackGround4();
				UpdateBackGround5();

				/*�|���S���̍X�V*/
				UpdateCursor();

				/*�v���C���[�̍X�V*/
				UpdatePlayer01();

				/*�o���A�[�̍X�V*/
				UpdateBarrier();
	
				/*�e�̍X�V*/
				UpdateBullet();

				/*�G�l�~�[�̍X�V*/
				UpdateEnemy();

				/*�����̍X�V*/
				UpdateExplosion();
				
				/*�t�B�[���h�g�̍X�V*/
				UpdateFrameLeft();
				UpdateFrameRight();

				/*�L�����N�^�[�w�i�̍X�V*/
				//UpdateSideCharacter();

				/*�q�X�e���b�N�Q�[�W�̍X�V*/
				UpdateHystericGauge();
	
				/*�R���{���̍X�V*/
				UpdateComboNumber();

				/*�R���{�\���̍X�V*/
				UpdateComboUi();

				/*�^�C�}�[�̍X�V*/
				UpdateTimer();

				/*�X�R�A�̍X�V*/
				UpdateScore();

				break;
			}
		case SEQUENCE_PAUSE :
			{
				/*�|�[�Y�\���̍X�V*/
				UpdatePauseGraphic();
				if( GetKeyboardTrigger( DIK_P ) || GetGamePadTrigger( 9 ) )
				{
					StopSound( SOUND_LABEL_BGM_PAUSE );
					PlaySoundOneMoreTime( ( SOUND_LABEL )( soundLabelBufferPause ) );
					g_gameSequence = SEQUENCE_PLAY;
				}
				break;
			}
		case SEQUENCE_CONTINUE :
			{
				break;
			}
		default :
			{
				Error("�Q�[�����̃V�[�N�G���X�����肦�Ȃ��J�ڂ����܂����B");
			}
	}

	if( GetFade() == FADE_NONE && ( GetKeyboardTrigger( DIK_RETURN ) ) )//|| GetGamePadTrigger( 0 ) ))
	{
		clearFlag = true;
	}

	//���[�h�ڍs�ɔ����ŐV�X�R�A�ۑ�
	if( GetFade() == FADE_NONE && clearFlag == true )
	{
		int endScore = getScore();
		FILE *fp;
		
		if( ( fp = fopen( "data/scs.dat" , "wb" ) ) == NULL )
		{
			 Error( "�X�R�A�̋L�^�Ɏ��s���܂���" );
		}
		else
		{
			fwrite( &endScore ,sizeof( endScore ) , 1 , fp );
			fclose(fp);
		}
		SetFade( FADE_OUT , MODE_RESULT ); 
	}
}

/*******************************************************************************
* �֐���	:void DrawGame( void )
*		�Q�[���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawGame( void )
{
	switch( g_gameSequence )
	{	
		case SEQUENCE_PAUSE :
			{
				//�v���C���̉�ʂ𓧂������߂�break�͂��Ȃ�
			}
		case SEQUENCE_COUNTDOWN :
			{
				//�v���C���̉�ʂ𓧂������߂�break�͂��Ȃ�

			}
		case SEQUENCE_PLAY :
			{
				/*�w�i�̕`��:���i��*/
				DrawBackGround5();
				DrawBackGround4();
				DrawBackGround();
				DrawBackGround2();
				DrawBackGround3();
	
	
				/*�R���{���̕`��*/
				DrawComboNumber();

				/*�R���{�\���̕`��*/
				DrawComboUi();
	
				/*�o���A�[�̕`��*/
				DrawBarrier();
	
				/*�v���C���[�̕`��*/
				DrawPlayer01();
	


				DrawBullet();
	
				DrawEnemy();
				DrawExplosion();
				
				/*�R���{���̕`��*/
				DrawComboNumber2();
				
				/*�R���{�\���̕`��*/
				DrawComboUi2();

				DrawPlayer01core();

				/*�t�B�[���h�g�̕`��*/
				DrawFrameLeft();
				DrawFrameRight();

				/*�L�����N�^�[�w�i�̕`��*/
				//DrawSideCharacter();

				/*�q�X�e���b�N�Q�[�W�̕`��*/
				DrawHystericGauge();

				
	
				/*�^�C�}�[�̕`��*/
				DrawTimer();

				/*�X�R�A�̕`��*/
				DrawScore();


				/*�J�[�\���̕`��*/
				DrawCursor();
				
				/*�X�^�[�g�J�E���g�̕`��*/
				DrawStartCounter();

				if( g_gameSequence == SEQUENCE_PAUSE )
				{
					/*�|�[�Y�\���̕`��*/
					DrawPauseGraphic();
				}
				break;
			}
		case SEQUENCE_CONTINUE :
			{
				break;
			}
		case SEQUENCE_GAMEOVER :
			{
				break;
			}
		default :
			{
				Error("�Q�[�����̃V�[�N�G���X�����肦�Ȃ��J�ڂ����܂����B");
			}
	}

	DrawStageManager();
}