/*******************************************************************************
* �^�C�g��	:�G�l�~�[.cpp
* �t�@�C����:AfEnemy.cpp
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
#include "AfEnemy.h"
#include "AfPlayer.h"
#include "AfCursor.h"
#include "AfLockonCursor.h"
#include "unoDirectX.h"
#include "unoHitChecker.h"
#include "AfLife.h"
#include "AfPlayer.h"
#include "AfExplosion.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_ENEMY_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_ENEMY_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_ENEMY_DEFAULT_VECTOR	( 0.0f )
#define AF_ENEMY_DEFAULT_POSZ	( -1.0f )
#define AF_ENEMY_DEFAULT_LIFE		( 100 )
#define AF_ENEMY_DEFAULT_LIFE_BOSS1		( 22000 )
#define AF_ENEMY_DEFAULT_SCORE		( 10 )

#define AF_ENEMY_OUTSCREEN	( 1.50f )

#define AF_ENEMY_SPIN	( 1.23 )

#define AF_ENEMY_VERTEX_MAX		( 4 )	/*�ō����_��*/
#define AF_ENEMY_POLYGON_NUM	( ENEMY_MAX_NUM )	/*���@�{����*/

#define AF_ENEMY_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define AF_ENEMY_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_ENEMY_ANIM_DIV_X	( 2 )	/*��������*/
#define AF_ENEMY_ANIM_DIV_Y	( 5 )	/*����������*/

#define AF_ENEMY_OUTPUT_SIZE	( 1.0f ) /*�����\���{��*/ 

#define MOVE_NUM ( 0.7f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.23f )	/*������*/
#define ROT_ATTEN		( 0.85f )	/*������*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define DAMAGE_OUTPUT_FPS ( 60 )	/*�_���[�W���_��FPS*/

#define PGPCHANGE_HEIGHT_0	( SCREEN_HEIGHT * 0.5 + 62 )
#define PGPCHANGE_HEIGHT_1	( SCREEN_HEIGHT * 0.5 + 33 )
#define PGPCHANGE_HEIGHT_3	( SCREEN_HEIGHT * 0.5 - 33 )
#define PGPCHANGE_HEIGHT_4	( SCREEN_HEIGHT * 0.5 - 62 )

#define ENEMY_TO_PLAYER_DAMAGE ( 33 )
#define ENEMY_PLAYER_HIT_ZONE ( 0.02f )

typedef enum
{
	PGP_H_PULL_UP = 0,
	PGP_L_PULL_UP,
	PGP_NEUTRAL,
	PGP_L_PULL_DOWN,
	PGP_H_PULL_DOWN
}ENEMY_GRAPHIC_PATTERN;

/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfEnemy = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfEnemy;	/*�G�l�~�[�e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;
static int g_nPatternAnim = 0;

static AF_ENEMY_STATUS enemyState[ENEMY_MAX_NUM]; 

static bool g_SoundFlag000 = false;
static bool noEnemySet = false;

static int bossMoveX,bossMoveY;
/*******************************************************************************
* �֐���	:HRESULT InitAfEnemy( void )
*		�G�l�~�[�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfEnemy( void )
{
	HRESULT result;
	result = InitDirectAfEnemy();
	if( result != S_OK )
	{
		Error("�`���ʒu�f�[�^�̏������Ɏ��s���܂����B");
	}
	InitValueAfEnemy();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfEnemy( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfEnemy( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_ENEMY_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfEnemy , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfEnemy.png" , &g_pTextureAfEnemy );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfEnemy.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfEnemy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_ENEMY_POLYGON_NUM );


	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfEnemy->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfEnemy( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfEnemy( void )
{
	int outputSizeX , outputSizeY;
	
	//���O�T�C�Y�v�Z
	outputSizeX = ( int )( ( info.Width / AF_ENEMY_ANIM_DIV_X ) * AF_ENEMY_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_ENEMY_ANIM_DIV_Y ) * AF_ENEMY_OUTPUT_SIZE );

	//���O�T�C�Y�ϊ�
	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	
	//������
	memset( &enemyState , 0 , sizeof( AF_ENEMY_STATUS ) * ENEMY_MAX_NUM );
	
	for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
	{
		enemyState[cnt].bUse = false;
		enemyState[cnt].bLockOn = false;
		enemyState[cnt].posStatus.posX = AF_ENEMY_DEFAULT_POSX;
		enemyState[cnt].posStatus.posY = AF_ENEMY_DEFAULT_POSY;
		enemyState[cnt].posStatus.vector = AF_ENEMY_DEFAULT_VECTOR;
		enemyState[cnt].moveZ = 0.0f;
		enemyState[cnt].posZ = AF_ENEMY_DEFAULT_POSZ;

		enemyState[cnt].intPosX = ( int )( enemyState[cnt].posStatus.posX ) * 1000;
		enemyState[cnt].intPosY = ( int )( enemyState[cnt].posStatus.posY ) * 1000;
		
		enemyState[cnt].intMoveX = 0;
		enemyState[cnt].intMoveY = 0;

		enemyState[cnt].nLife = AF_ENEMY_DEFAULT_LIFE;
		enemyState[cnt].nScore = AF_ENEMY_DEFAULT_SCORE;
		enemyState[cnt].mode = ENEMY_MODE_NORMAL;   
		enemyState[cnt].flightPattern = ENF_NO_MOVE;
		enemyState[cnt].aiPattern = ENAI_NO_SHOT;
	}

	g_posPolygon.x = AF_ENEMY_DEFAULT_POSX;
	g_posPolygon.y = AF_ENEMY_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	bossMoveX = 0;
	bossMoveY = 0;
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�A�j���[�V�����p�X�e�[�^�X*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	noEnemySet = false;
	g_SoundFlag000 = false;
}

/*******************************************************************************
* �֐���	:void UninitAfEnemy( void )
*		�G�l�~�[�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfEnemy( void )
{
	if( g_pVtxBuffAfEnemy != NULL )
	{
		g_pVtxBuffAfEnemy->Release();
		g_pVtxBuffAfEnemy = NULL;
	}
	if( g_pTextureAfEnemy != NULL )
	{
		g_pTextureAfEnemy->Release();
		g_pTextureAfEnemy = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfEnemy( void )
*		�G�l�~�[�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfEnemy( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	ENEMY_GRAPHIC_PATTERN nowPatternNum = PGP_NEUTRAL;

	g_SoundFlag000 = false;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfEnemy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
	{
		nowPatternNum = PGP_NEUTRAL;
		if( enemyState[cnt].bUse == true )
		{
			//�s���p�^�[���ݒ�
			switch( enemyState[cnt].flightPattern )
			{
				case ENF_NO_MOVE :
					break;
				case ENF_STRAIGHT :
					//enemyState[cnt].moveZ = 0.011;
					enemyState[cnt].moveZ = 0.025;
					break;
				case ENF_PLAYER_TRACKING:
					/*
					enemyState[cnt].intPosX = 0;
					enemyState[cnt].intPosY = 0;
					*/
					break;
				default:
					Error("���݂��Ȃ��s���p�^�[���̃G�l�~�[����������܂����B");
					break;
			}
			//�U���p�^�[���ݒ�
			switch( enemyState[cnt].aiPattern )
			{
				case ENAI_NO_SHOT :
					break;
				case ENAI_MISSILE :
					//setMissile
					break;
				case ENEMY_AI_BOSS1:
					if( enemyState[cnt].posZ < PLAYER_Z_POS )
					{
						enemyState[cnt].moveZ -= GetPlayerState()->speed * 2;
					}
					if( enemyState[cnt].posZ <= 0.6 )
					{
						enemyState[cnt].moveZ = 0;
					}
					else
					{
						enemyState[cnt].moveZ -= GetPlayerState()->speed;
					}
					
					if( enemyState[cnt].nLife < AF_ENEMY_DEFAULT_LIFE_BOSS1	* 0.060 )
					{
						noEnemySet = true;
					}
					if( enemyState[cnt].posZ < PLAYER_Z_POS )
					{
						enemyState[cnt].moveZ -= GetPlayerState()->speed * 2;
					}
					if( enemyState[cnt].posZ >= 0.9 )
					{
						enemyState[cnt].moveZ -= GetPlayerState()->speed * 4;
					}
					else
					{
						enemyState[cnt].moveZ += GetPlayerState()->speed * 5;
					}
					if( enemyState[cnt].nLife < AF_ENEMY_DEFAULT_LIFE_BOSS1	* 0.060 )
					{
						noEnemySet = true;
					}

					if( GetPlayerState()->intPosX + 40000 >= enemyState[cnt].intPosX && GetPlayerState()->intPosX - 40000 <= enemyState[cnt].intPosX )
					{
						if( GetPlayerState()->intPosX * 0.001 >= SCREEN_WIDTH * 0.2 )
						{
							enemyState[cnt].intMoveX += 44000;
						}
						if( GetPlayerState()->intPosX * 0.001 <= SCREEN_WIDTH * 0.2 )
						{
							enemyState[cnt].intMoveX -= 44000;
						}
						if( GetPlayerState()->intPosX * 0.001 >= SCREEN_WIDTH * 0.3 )
						{
							enemyState[cnt].intMoveX -= 23200;
						}
						if( GetPlayerState()->intPosX * 0.001 <= SCREEN_WIDTH * 0.3 )
						{
							enemyState[cnt].intMoveX += 23200;
						}
					}
					else
					{
						if( GetPlayerState()->intPosX * 0.001 >= SCREEN_WIDTH * 0.7 )
						{
							enemyState[cnt].intMoveX -= 13200;
						}
						if( GetPlayerState()->intPosX * 0.001 <= SCREEN_WIDTH * 0.2 )
						{
							enemyState[cnt].intMoveX += 13200;
						}
					}
					if( enemyState[cnt].bLockOn == true )
					{
						
						enemyState[cnt].moveZ -= GetPlayerState()->speed * 2;
						enemyState[cnt].intMoveX *= 8;
						if( enemyState[cnt].intPosX >= SCREEN_WIDTH * 11200 * 1.5 || enemyState[cnt].intPosX <= -( SCREEN_WIDTH * 11200 * 0.5 ) )
						{
							enemyState[cnt].intMoveY -= 5400;
						}
						
						enemyState[cnt].intMoveY += 2800;
					}
					else
					{
						enemyState[cnt].moveZ += GetPlayerState()->speed;
						enemyState[cnt].intMoveX * 0.33f;
						enemyState[cnt].intMoveY * 0.33f;
					}
					if( GetPlayerState()->intPosY + 10000 >= enemyState[cnt].intPosY && GetPlayerState()->intPosY - 10000 <= enemyState[cnt].intPosY )
					{
						enemyState[cnt].intMoveY += 5400;
						if(  GetPlayerState()->intPosY <= enemyState[cnt].intPosY )
						{
							enemyState[cnt].intMoveY -= 9400;	
						}
					}
					else
					{
						
						if(  GetPlayerState()->intPosY >= enemyState[cnt].intPosY )
						{
							enemyState[cnt].intMoveY += 12400;	
						}
						if(  GetPlayerState()->intPosY <= enemyState[cnt].intPosY )
						{
							enemyState[cnt].intMoveY -= 12400;	
						}
					}
					
					if( GetPlayerState()->intPosX * 0.001 >= SCREEN_WIDTH * 0.4 )
					{
						enemyState[cnt].intMoveX -= 83200;
					}
					if( GetPlayerState()->intPosX * 0.001 <= SCREEN_WIDTH * 0.3 )
					{
						enemyState[cnt].intMoveX += 43200;
					}
					if( enemyState[cnt].intMoveX >= 40000 )
					{
						enemyState[cnt].intMoveX = 40000;
					}
					if( enemyState[cnt].intMoveX <= -40000 )
					{
						enemyState[cnt].intMoveX = -40000;
					}
					enemyState[cnt].intPosX += enemyState[cnt].intMoveX;
					enemyState[cnt].intPosY += enemyState[cnt].intMoveY;
					enemyState[cnt].posStatus.posX = enemyState[cnt].intPosX * 0.001f;
					enemyState[cnt].posStatus.posY = enemyState[cnt].intPosY * 0.001f;
					
					enemyState[cnt].posStatus.DrawVector = enemyState[cnt].intMoveX * 0.000001f;
					/*
					if( enemyState[cnt].bLockOn == true )
					{
						if( rand() % 40 == 0 )
						{	
							SetAfExplosion(	enemyState[cnt].posStatus.DrawPosX * 1000 , enemyState[cnt].posStatus.DrawPosY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
					
							enemyState[cnt].posStatus.DrawVector += 0.1;
							if( enemyState[cnt].nLife < AF_ENEMY_DEFAULT_LIFE_BOSS1	* 0.70 )
							{
								if( enemyState[cnt].nLife > AF_ENEMY_DEFAULT_LIFE_BOSS1 * 0.05 && rand() % 15 >= 13 )
								{
									int randX = rand() % SCREEN_WIDTH,randY = rand() % SCREEN_HEIGHT;
									SetAfExplosion( randX * 1000 , randY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
								
									SetEnemyState( randX , randY , 0.4f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
								}

								if( enemyState[cnt].nLife < AF_ENEMY_DEFAULT_LIFE_BOSS1	* 0.50 )
								{
									if( enemyState[cnt].nLife > AF_ENEMY_DEFAULT_LIFE_BOSS1 * 0.15 && rand() % 15 >= 12 )
									{
										int randX = rand() % SCREEN_WIDTH,randY = rand() % SCREEN_HEIGHT;
										SetAfExplosion( randX * 1000 , randY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
								
										SetEnemyState( randX , randY , 0.4f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
									}
									enemyState[cnt].posStatus.DrawVector += 1.1f * ( rand()%10 );
								}
								if( GetPlayerState()->posStatus.DrawPosY < enemyState[cnt].posStatus.posY )
								{
									enemyState[cnt].posStatus.posY -= 200 * cos( enemyState[cnt].posStatus.DrawVector );
								}
								else
								{
									enemyState[cnt].posStatus.posY += 200 * cos( enemyState[cnt].posStatus.DrawVector );
								}
								if( GetPlayerState()->posStatus.DrawPosX < enemyState[cnt].posStatus.posX )
								{
									enemyState[cnt].posStatus.posX -= 20 * cos( enemyState[cnt].posStatus.DrawVector );
								}
								else
								{
									enemyState[cnt].posStatus.posX += 20 * cos( enemyState[cnt].posStatus.DrawVector );
								}
							}
						}
						if( enemyState[cnt].nLife < AF_ENEMY_DEFAULT_LIFE_BOSS1	* 0.40 )
						{
							SetAfExplosion(	enemyState[cnt].posStatus.DrawPosX * 1000 , enemyState[cnt].posStatus.DrawPosY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
					
							if( enemyState[cnt].nLife > AF_ENEMY_DEFAULT_LIFE_BOSS1 * 0.05 && rand() % 15 >= 11 )
							{
								int randX = rand() % SCREEN_WIDTH,randY = rand() % SCREEN_HEIGHT;
								SetAfExplosion( randX * 1000 , randY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
								
								SetEnemyState( randX , randY , 0.4f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
							}
							enemyState[cnt].posStatus.DrawVector += 1.1f * ( rand()%10 );
						
							if( GetPlayerState()->posStatus.DrawPosX > enemyState[cnt].posStatus.posX )
							{
								enemyState[cnt].posStatus.posX += rand() % 20;
							}
							else
							{
								enemyState[cnt].posStatus.posX -= rand() % 20;
							}
							
							if( GetPlayerState()->posStatus.DrawPosY > enemyState[cnt].posStatus.posY )
							{
								enemyState[cnt].posStatus.posY += rand() % 20;
							}
							else
							{
								enemyState[cnt].posStatus.posY -= rand() % 20;
							}
						}
						if( enemyState[cnt].posStatus.posX <= GetPlayerState()->posStatus.posX )
						{
							enemyState[cnt].posStatus.posX += 10;
							enemyState[cnt].posStatus.posY -= 10;
							if( enemyState[cnt].posZ < 0.7 )
							{
								enemyState[cnt].moveZ += 0.010;
							}
							else
							{
								enemyState[cnt].moveZ = ( enemyState[cnt].moveZ >= 1.0 ? enemyState[cnt].moveZ + 0.010 : enemyState[cnt].moveZ - 0.010 );
							}
						}
						
						if(	HitTwinCircle( enemyState[cnt].intPosX , enemyState[cnt].intPosY , 500 ,
						GetPlayerState()->intPosX , GetPlayerState()->intPosY , PLAYER_RADIUS ) == true )
						{
							enemyState[cnt].posStatus.DrawVector *= 0.123;
						}
						else
						{
							if( rand() % 30 == 0 )
							{
								SetAfExplosion(	enemyState[cnt].posStatus.DrawPosX * 1000 , enemyState[cnt].posStatus.DrawPosY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
					
								enemyState[cnt].intPosX = GetPlayerState()->posStatus.posX + rand() % 50 - rand() % 50;
								enemyState[cnt].intPosY = GetPlayerState()->posStatus.posY + rand() % 50 - rand() % 50;
							}
						}
					}
					else
					{	
						if( enemyState[cnt].posStatus.posX > GetPlayerState()->posStatus.posX )
						{
							enemyState[cnt].posStatus.posX -= 18;
						}
						if( enemyState[cnt].posStatus.posX > GetPlayerState()->posStatus.posX )
						{
							enemyState[cnt].posStatus.posX -= 18;
						}
						if( enemyState[cnt].posStatus.posY > GetPlayerState()->posStatus.posY )
						{
							enemyState[cnt].posStatus.posY -= 3;
						}
						if( enemyState[cnt].posStatus.posY > GetPlayerState()->posStatus.posY )
						{
							enemyState[cnt].posStatus.posY -= 3;
						}
					}
					if( rand() % 20 == 0 )
					{
						SetAfExplosion(	enemyState[cnt].posStatus.DrawPosX * 1000 , enemyState[cnt].posStatus.DrawPosY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
					
						if( enemyState[cnt].posStatus.posY >= SCREEN_HEIGHT * 8 || enemyState[cnt].posStatus.posY <= SCREEN_HEIGHT * 2 )
						{
							enemyState[cnt].posStatus.posY = SCREEN_HEIGHT * ( ( rand() % SCREEN_HEIGHT ) * 0.001 );
						}
						if( enemyState[cnt].posStatus.posX >= SCREEN_WIDTH * 8 || enemyState[cnt].posStatus.posX <= SCREEN_WIDTH * 2 )
						{
							enemyState[cnt].posStatus.posX = SCREEN_WIDTH * ( ( rand() % SCREEN_WIDTH ) * 0.001 );
						}
					}
					if( enemyState[cnt].nLife < AF_ENEMY_DEFAULT_LIFE_BOSS1	* 0.20 )
					{
						if( enemyState[cnt].nLife > AF_ENEMY_DEFAULT_LIFE_BOSS1 * 0.10 && rand() % 15 >= 6 )
						{
							int randX = rand() % SCREEN_WIDTH,randY = rand() % SCREEN_HEIGHT;
							SetAfExplosion( randX * 1000 , randY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
							
							SetEnemyState( randX , randY , 0.4f , 0.0f , ENF_STRAIGHT , ENAI_NO_SHOT );
						}
						enemyState[cnt].posZ = 0.013 * ( rand() % 60 );
						if( enemyState[cnt].nLife < AF_ENEMY_DEFAULT_LIFE_BOSS1	* 0.10 )
						{
							SetAfExplosion(	enemyState[cnt].posStatus.DrawPosX * 1000 , enemyState[cnt].posStatus.DrawPosY * 1000 , 0.4f , 0.0f , 0.0f , 0.0f , 8 , AF_EXPLOSION_003 );
					
							enemyState[cnt].posStatus.posX = GetPlayerState()->posStatus.posX + rand() % 20 - rand() % 20;
							enemyState[cnt].posStatus.posY = GetPlayerState()->posStatus.posY + rand() % 20 - rand() % 20;
						}
					}*/
						
					//setMissile
					break;
				default:
					Error("���݂��Ȃ��헪�p�^�[���̃G�l�~�[����������܂����B");
					break;
			}
			//���S�m�F
			if( enemyState[cnt].nLife <= 0 || enemyState[cnt].posZ >= AF_ENEMY_OUTSCREEN )
			{
				enemyState[cnt].bUse = false;
				enemyState[cnt].bLockOn = false;
			}
			//�����ʒu�m��

			//�`����W�E�T�C�Y�m��
			enemyState[cnt].posZ += enemyState[cnt].moveZ + GetPlayerState()->speed;
			enemyState[cnt].posStatus.DrawPosX = enemyState[cnt].posStatus.posX;
			enemyState[cnt].posStatus.DrawPosY = enemyState[cnt].posStatus.posY;
			enemyState[cnt].posStatus.DrawVector = enemyState[cnt].posStatus.vector - getViewStatus()->centerVector;
			polygonMag = 0.8 - sin( 1.0 - sin( ( enemyState[cnt].posZ >= 0.0f ? enemyState[cnt].posZ : 0.0f ) ) );

			//float plusVecPos = SCREEN_HEIGHT * 0.012 * sin( D3DX_PI * getViewStatus()->centerVector );

			enemyState[cnt].posStatus.DrawPosX = ( enemyState[cnt].posStatus.DrawPosX * ( polygonMag <= 0.1f ? 0.1f : polygonMag ) ) + SCREEN_WIDTH  * 0.5 * ( 1.0f - ( polygonMag * 0.5 >= 1.0f ? 1.0f : polygonMag * 0.5 ) ); 
			enemyState[cnt].posStatus.DrawPosY = ( enemyState[cnt].posStatus.DrawPosY * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + ( SCREEN_HEIGHT - getViewStatus()->centerPosY ) * ( 1.0f - ( polygonMag >= 1.0f ? 1.0f : polygonMag) );
			g_posPolygon.x = enemyState[cnt].posStatus.DrawPosX;
			g_posPolygon.y = enemyState[cnt].posStatus.DrawPosY;
			g_rotPolygon.z = enemyState[cnt].posStatus.DrawVector;

			
			if( enemyState[cnt].bUse == true && 
				HitTwinCircle( enemyState[cnt].intPosX , enemyState[cnt].intPosY , ENEMY_RADIUS ,
				GetPlayerState()->intPosX , GetPlayerState()->intPosY , PLAYER_RADIUS ) == true &&
				PLAYER_Z_POS >enemyState[cnt].posZ - ENEMY_PLAYER_HIT_ZONE && 
				PLAYER_Z_POS <enemyState[cnt].posZ + ENEMY_PLAYER_HIT_ZONE )
			{
				SetAfExplosion( enemyState[cnt].intPosX , enemyState[cnt].intPosY , enemyState[cnt].posZ * 800 , 0 , 0 , enemyState[cnt].moveZ * 800 , 18 , AF_EXPLOSION_001 );
				SetAfExplosion( enemyState[cnt].intPosX + rand() % 3 - rand() % 3 , enemyState[cnt].intPosY + rand() % 3 - rand() % 3 , enemyState[cnt].posZ * 100 , 0 , 0 , enemyState[cnt].moveZ * 100 , 10 , AF_EXPLOSION_001 );
				SetAfExplosion( enemyState[cnt].intPosX + rand() % 3 - rand() % 3 , enemyState[cnt].intPosY + rand() % 3 - rand() % 3 , enemyState[cnt].posZ * 100 , 0 , 0 , enemyState[cnt].moveZ * 100 , 10 , AF_EXPLOSION_001 );
				
				PlusAfLife( ENEMY_TO_PLAYER_DAMAGE );
			}
			enemyState[cnt].intPosX = ( int )( enemyState[cnt].posStatus.DrawPosX ) * 1000;
			enemyState[cnt].intPosY = ( int )( enemyState[cnt].posStatus.DrawPosY ) * 1000;

			//���b�N�I������
			if( enemyState[cnt].posZ >= AF_LOCKON_CURSOR_LOST )
			{
				if( g_SoundFlag000 == false ) 
				{
					PlaySound( SOUND_LABEL_SE_ENEMYCLOSE );
					g_SoundFlag000 = true;
				}
				enemyState[cnt].bLockOn = false;
				GetLockonCursorState(cnt)->bUse = false;
			}

			//�I�[�o�[�C��
			if( g_posPolygon.x < SCREEN_LEFT )
			{
				g_posPolygon.x = SCREEN_LEFT;
			}					 
			if( g_posPolygon.x > SCREEN_RIGHT )
			{
				g_posPolygon.x = SCREEN_RIGHT;
			}
			if( g_posPolygon.y < FIELD_TOP )
			{
				g_posPolygon.y = FIELD_TOP;
			}
			if( g_posPolygon.y > FIELD_HEIGHT )
			{
				g_posPolygon.y = FIELD_HEIGHT;
			}
	
			if( g_rotPolygon.z > D3DX_PI )
			{
				g_rotPolygon.z = -D3DX_PI + ( D3DX_PI - g_rotPolygon.z );
			}
			if( g_rotPolygon.z < -D3DX_PI )
			{
				g_rotPolygon.z = D3DX_PI - ( D3DX_PI - g_rotPolygon.z );
			}

			if( enemyState[cnt].posStatus.posY < PGPCHANGE_HEIGHT_4 )
			{
				nowPatternNum = PGP_H_PULL_DOWN;
			}
			else if( enemyState[cnt].posStatus.posY < PGPCHANGE_HEIGHT_3 )
			{
				nowPatternNum = PGP_L_PULL_DOWN;
			}
			else if( enemyState[cnt].posStatus.posY > PGPCHANGE_HEIGHT_0 )
			{
				nowPatternNum = PGP_H_PULL_UP;
			}
			else if( enemyState[cnt].posStatus.posY > PGPCHANGE_HEIGHT_1 )
			{
				nowPatternNum = PGP_L_PULL_UP;
			}

			//�A�j���[�V�����ݒ�
			if( ++g_nCounterAnim == AF_ENEMY_ANIM_SPAN )
			{
				g_nCounterAnim = 0;
				g_nPatternAnim = ( ++g_nPatternAnim == AF_ENEMY_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
	
			texWidth = 1.0f / AF_ENEMY_ANIM_DIV_X;
			texHeight = 1.0f / AF_ENEMY_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_ENEMY_ANIM_DIV_X );
			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_ENEMY_ANIM_DIV_X ) ) + nowPatternNum;
	
			//BOSS�O���ɕύX
			if( enemyState[cnt].aiPattern == ENEMY_AI_BOSS1 )
			{
				nowPatternX = 1;
			}

			//�e�N�X�`�����W�ݒ�
			pVtx[QUOTE_VERTEX * cnt].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 1].tex.y = texHeight * nowPatternY;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.x = texWidth * nowPatternX;
			pVtx[QUOTE_VERTEX * cnt + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[QUOTE_VERTEX * cnt + 3].tex.y = texHeight * nowPatternY + texHeight;

			//���_���W�ݒ�
			pVtx[QUOTE_VERTEX * cnt].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
			pVtx[QUOTE_VERTEX * cnt + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		}
		if( enemyState[cnt].bUse == false )
		{			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		}
	}

	for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
	{
		for( int cnt2 = 0; cnt2 < ENEMY_MAX_NUM; cnt2++ )
		{
			if( enemyState[cnt].posZ < enemyState[cnt].posZ )
			{
				AF_ENEMY_STATUS Buff;
				LOCKON_CURSOR_STATUS BuffC;

				Buff = enemyState[cnt];
				enemyState[cnt] = enemyState[cnt2];
				enemyState[cnt2] = Buff;
				
				BuffC = *GetLockonCursorState(cnt);
				*GetLockonCursorState(cnt) = *GetLockonCursorState(cnt2);
				*GetLockonCursorState(cnt2) = BuffC;
			}
		}
	}
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfEnemy->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfEnemy( void )
*		�G�l�~�[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfEnemy( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfEnemy );
	
	for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
	{
		/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
		pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfEnemy , 0 , sizeof( VERTEX_2D ) );

		/*�G�l�~�[�̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , QUOTE_VERTEX * cnt , 2 );
	}
}

/*******************************************************************************
* �֐���	:void DrawAfEnemycore( void )
*		�G�l�~�[�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfEnemycore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfEnemy );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfEnemy , 0 , sizeof( VERTEX_2D ) );
	
	
	/*�G�l�~�[�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_ENEMY_VERTEX_MAX , 2 );

}

/*******************************************************************************
* �֐���	:AF_ENEMY_STATUS* GetEnemyState( int num )
*		�G�l�~�[�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�G�l�~�[�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
AF_ENEMY_STATUS* GetEnemyState( int num )
{
	return &enemyState[num];
}

/*******************************************************************************
* �֐���	:AF_ENEMY_STATUS* GetEnemyState( int num )
*		�G�l�~�[�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�G�l�~�[�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
void SetEnemyState( float posX , float posY , float posZ , float moveZ , ENEMY_FLIGHT_PATTERN ENFlightPattern , ENEMY_AI_PATTERN ENAIPattern )
{
	if( noEnemySet == false )
	{
		for( int cnt = 0; cnt < ENEMY_MAX_NUM; cnt++ )
		{
			if( enemyState[cnt].bUse == false )
			{
				enemyState[cnt].bUse = true;
				enemyState[cnt].bLockOn = false;

				enemyState[cnt].posStatus.posX = posX;
				enemyState[cnt].posStatus.posY = posY;
				enemyState[cnt].posStatus.vector = 0.0f;
				enemyState[cnt].moveZ = moveZ;
				enemyState[cnt].posZ = posZ;

				enemyState[cnt].intPosX = ( int )( enemyState[cnt].posStatus.posX ) * 1000;
				enemyState[cnt].intPosY = ( int )( enemyState[cnt].posStatus.posY ) * 1000;
				 
				enemyState[cnt].nScore = AF_ENEMY_DEFAULT_SCORE;
				enemyState[cnt].mode = ENEMY_MODE_NORMAL;   
				enemyState[cnt].flightPattern = ENFlightPattern;
				enemyState[cnt].aiPattern = ENAIPattern;
				switch( enemyState[cnt].aiPattern )
				{
					case ENEMY_AI_BOSS1:
						enemyState[cnt].nLife = AF_ENEMY_DEFAULT_LIFE_BOSS1;
						break;
					default:
						enemyState[cnt].nLife = AF_ENEMY_DEFAULT_LIFE;
						break;
				}
				break;
			}
		}
	}
}