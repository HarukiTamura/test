/*******************************************************************************
* �^�C�g��	:���e.cpp
* �t�@�C����:AfMissilePlayer.cpp
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
#include "AfMissilePlayer.h"
#include "AfPlayer.h"
#include "AfScore.h"
#include "AfEnemy.h"
#include "AfCursor.h"
#include "unoDirectX.h"
#include "AfExplosion.h"
#include "unoHitChecker.h"
#include "AfComboNumber.h"
#include "afGame.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_MISSILE_PLAYER_DEFAULT_POSX	( SCREEN_WIDTH / 2 )
#define AF_MISSILE_PLAYER_DEFAULT_POSY	( SCREEN_HEIGHT / 2 )
#define AF_MISSILE_PLAYER_DEFAULT_VECTOR	( 0.0f )
#define AF_MISSILE_PLAYER_DEFAULT_POSZ	( 0.0f )
#define AF_MISSILE_PLAYER_DEFAULT_LIFE		( 100 )
#define AF_MISSILE_PLAYER_DEFAULT_SCORE		( 10 )
#define AF_MISSILE_PLAYER_ATTACK	( 10 )

#define AF_MISSILE_PLAYER_MOVE	( 3 )
#define AF_MISSILE_PLAYER_SPEED	( 0.003f )
#define AF_MISSILE_PLAYER_OUTSCREEN	( 1.20f )
#define AF_MISSILE_PLAYER_DELETE		( 0.00f )

#define AF_MISSILE_PLAYER_SPIN	( 1.23 )

#define AF_MISSILE_PLAYER_VERTEX_MAX		( 4 )	/*�ō����_��*/
#define AF_MISSILE_PLAYER_POLYGON_NUM	( MISSILE_PLAYER_MAX_NUM )	/*���@�{����*/

#define AF_MISSILE_PLAYER_ANIM_MAX		( 3 )	/*�A�j���[�V�����R�}��*/
#define AF_MISSILE_PLAYER_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define AF_MISSILE_PLAYER_ANIM_DIV_X	( 3 )	/*��������*/
#define AF_MISSILE_PLAYER_ANIM_DIV_Y	( 5 )	/*����������*/

#define AF_MISSILE_PLAYER_OUTPUT_SIZE	( 1.0f ) /*�����\���{��*/ 

#define MOVE_NUM ( 0.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.10f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.23f )	/*������*/
#define ROT_ATTEN		( 0.85f )	/*������*/
#define SIPIN_ROT_ATTEN ( 0.12f )

#define CHASE_SPEED		( 0.18f )

#define CHASE_START		( 0.90 )
#define STAY_END		( 30 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfMissilePlayer = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfMissilePlayer;	/*���e�e�N�X�`���̃|�C���^*/
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

static MISSILE_PLAYER_STATUS missilePlayerState[MISSILE_PLAYER_MAX_NUM]; 

/*******************************************************************************
* �֐���	:HRESULT InitAfMissilePlayer( void )
*		���e�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfMissilePlayer( void )
{
	HRESULT result;
	result = InitDirectAfMissilePlayer();
	if( result != S_OK )
	{
		Error("�`���ʒu�f�[�^�̏������Ɏ��s���܂����B");
	}
	InitValueAfMissilePlayer();
	return result;
}

/*******************************************************************************
* �֐���	:HRESULT InitDirectAfMissilePlayer( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitDirectAfMissilePlayer( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * QUOTE_VERTEX * AF_MISSILE_PLAYER_POLYGON_NUM , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfMissilePlayer , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfMissilePlayer.png" , &g_pTextureAfMissilePlayer );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfMissilePlayer.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfMissilePlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	vertexInitQuad( pVtx , AF_MISSILE_PLAYER_POLYGON_NUM );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfMissilePlayer->Unlock();
}

/*******************************************************************************
* �֐���	:HRESULT InitValueAfMissilePlayer( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
void InitValueAfMissilePlayer( void )
{
	int outputSizeX , outputSizeY;
	
	//���O�T�C�Y�v�Z
	outputSizeX = ( int )( ( info.Width / AF_MISSILE_PLAYER_ANIM_DIV_X ) * AF_MISSILE_PLAYER_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_MISSILE_PLAYER_ANIM_DIV_Y ) * AF_MISSILE_PLAYER_OUTPUT_SIZE );

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
	memset( &missilePlayerState , 0 , sizeof( MISSILE_PLAYER_STATUS ) * MISSILE_PLAYER_MAX_NUM );
	
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
		missilePlayerState[cnt].bUse = false;
		missilePlayerState[cnt].posStatus.posX = AF_MISSILE_PLAYER_DEFAULT_POSX;
		missilePlayerState[cnt].posStatus.posY = AF_MISSILE_PLAYER_DEFAULT_POSY;
		missilePlayerState[cnt].posStatus.vector = AF_MISSILE_PLAYER_DEFAULT_VECTOR;
		missilePlayerState[cnt].moveZ = 0.0f;
		missilePlayerState[cnt].posZ = AF_MISSILE_PLAYER_DEFAULT_POSZ;

		missilePlayerState[cnt].intPosX = ( int )( missilePlayerState[cnt].posStatus.posX ) * 1000;
		missilePlayerState[cnt].intPosY = ( int )( missilePlayerState[cnt].posStatus.posY ) * 1000;
				  
		missilePlayerState[cnt].nLife = AF_MISSILE_PLAYER_DEFAULT_LIFE;
	}

	g_posPolygon.x = AF_MISSILE_PLAYER_DEFAULT_POSX;
	g_posPolygon.y = AF_MISSILE_PLAYER_DEFAULT_POSY;
	g_posPolygon.z = 0.0f;

	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�A�j���[�V�����p�X�e�[�^�X*/
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;
	
}

/*******************************************************************************
* �֐���	:void UninitAfMissilePlayer( void )
*		���e�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfMissilePlayer( void )
{
	if( g_pVtxBuffAfMissilePlayer != NULL )
	{
		g_pVtxBuffAfMissilePlayer->Release();
		g_pVtxBuffAfMissilePlayer = NULL;
	}
	if( g_pTextureAfMissilePlayer != NULL )
	{
		g_pTextureAfMissilePlayer->Release();
		g_pTextureAfMissilePlayer = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfMissilePlayer( void )
*		���e�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfMissilePlayer( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;
	int targetNum = 0;
	float targetPosX , targetPosY;
	bool soundFlag = false;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfMissilePlayer->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	targetPosY = SCREEN_HEIGHT * 0.5f; 
				///getViewStatus()->centerPosX;
	targetPosX = SCREEN_WIDTH  * 0.5f; 
				///getViewStatus()->centerPosX;
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
	//	
	//	
		for( ; targetNum < ENEMY_MAX_NUM; targetNum++ )
		{
			if( GetEnemyState(cnt)->bUse == true && GetEnemyState(cnt)->bLockOn == true )
			{		
				if( missilePlayerState[cnt].posZ >= GetEnemyState(cnt)->posZ )
				{
					targetPosX = GetEnemyState(cnt)->posStatus.DrawPosX;
					targetPosY = GetEnemyState(cnt)->posStatus.DrawPosY;
					targetNum++;
					break;
				}
			}
		}

		if( missilePlayerState[cnt].bUse == true )
		{
			for( int cnt2 = 0; cnt2 < AF_MISSILE_PLAYER_MOVE; cnt2++ )
			{
				//�s��
				missilePlayerState[cnt].moveZ = -AF_MISSILE_PLAYER_SPEED;
				
				pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

				if( missilePlayerState[cnt].posZ >= CHASE_START )
				{
					//�_�ΑO
					pVtx[QUOTE_VERTEX * cnt].col	 = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 100 , 155 , 155 , 255 );

					missilePlayerState[cnt].posStatus.posX += missilePlayerState[cnt].moveX;
					missilePlayerState[cnt].posStatus.posY += missilePlayerState[cnt].moveY;
				}
				else if( missilePlayerState[cnt].stayCnt < STAY_END )
				{
					if( soundFlag == false )
					{
						PlaySound( SOUND_LABEL_SE_MISSILEFLY2 );
						soundFlag = true;
					}
					pVtx[QUOTE_VERTEX * cnt].col	 = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
					missilePlayerState[cnt].stayCnt++;
					missilePlayerState[cnt].posStatus.posX += missilePlayerState[cnt].moveX;
					missilePlayerState[cnt].posStatus.posY += missilePlayerState[cnt].moveY;
				}
				else if( missilePlayerState[cnt].posZ >= GetEnemyState(cnt)->posZ  )
				{
					if( targetPosX < missilePlayerState[cnt].posStatus.posX )
					{
						missilePlayerState[cnt].posStatus.posX -= ( missilePlayerState[cnt].posStatus.posX - targetPosX ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
					if( targetPosX > missilePlayerState[cnt].posStatus.posX )
					{
						missilePlayerState[cnt].posStatus.posX += ( targetPosX - missilePlayerState[cnt].posStatus.posX ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
					if( targetPosY < missilePlayerState[cnt].posStatus.posY )
					{
						missilePlayerState[cnt].posStatus.posY -= ( missilePlayerState[cnt].posStatus.posY - targetPosY ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
					if( targetPosY > missilePlayerState[cnt].posStatus.posY )
					{
						missilePlayerState[cnt].posStatus.posY += ( targetPosY - missilePlayerState[cnt].posStatus.posY ) * CHASE_SPEED * sin( missilePlayerState[cnt].posZ );
					}
				}
				//���S�m�F
				if( missilePlayerState[cnt].nLife <= 0 || missilePlayerState[cnt].posZ >= AF_MISSILE_PLAYER_OUTSCREEN || missilePlayerState[cnt].posZ <= AF_MISSILE_PLAYER_DELETE )
				{
					missilePlayerState[cnt].bUse = false;
				}
				//�����ʒu�m��

				//�`����W�E�T�C�Y�m��
				missilePlayerState[cnt].posZ += missilePlayerState[cnt].moveZ;
				missilePlayerState[cnt].posStatus.DrawPosX = missilePlayerState[cnt].posStatus.posX;
				missilePlayerState[cnt].posStatus.DrawPosY = missilePlayerState[cnt].posStatus.posY;
				missilePlayerState[cnt].posStatus.DrawVector = missilePlayerState[cnt].posStatus.vector - getViewStatus()->centerVector;
				polygonMag = 0.8 - sin( 1.0 - sin( ( missilePlayerState[cnt].posZ >= 0.0f ? missilePlayerState[cnt].posZ : 0.0f ) ) );
				//polygonMag = missilePlayerState[cnt].posZ;
				
				SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , missilePlayerState[cnt].posZ * 100 , 0 , 0 , missilePlayerState[cnt].moveZ * 1300  , 4 , AF_EXPLOSION_002 );
				
				//missilePlayerState[cnt].posStatus.DrawPosX = ( missilePlayerState[cnt].posStatus.DrawPosX * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + SCREEN_WIDTH  * 0.5 * ( 1.0f - ( polygonMag * 0.5 >= 1.0f ? 1.0f : polygonMag * 0.5 ) ); 
				//missilePlayerState[cnt].posStatus.DrawPosY = ( missilePlayerState[cnt].posStatus.DrawPosY * ( polygonMag <= 0.0f ? 0.0f : polygonMag ) ) + ( SCREEN_HEIGHT - getViewStatus()->centerPosY ) * ( 1.0f - ( polygonMag >= 1.0f ? 1.0f : polygonMag) );
			
				g_posPolygon.x = missilePlayerState[cnt].posStatus.DrawPosX;
				g_posPolygon.y = missilePlayerState[cnt].posStatus.DrawPosY;
				g_rotPolygon.z = missilePlayerState[cnt].posStatus.DrawVector;
			
				missilePlayerState[cnt].intPosX = ( int )( missilePlayerState[cnt].posStatus.DrawPosX ) * 1000;
				missilePlayerState[cnt].intPosY = ( int )( missilePlayerState[cnt].posStatus.DrawPosY ) * 1000;
				
				for( int cnt3 = 0; cnt3 < ENEMY_MAX_NUM; cnt3++ )
				{
					if( GetEnemyState(cnt3)->bUse == true && 
						HitTwinCircle( GetEnemyState(cnt3)->intPosX , GetEnemyState(cnt3)->intPosY , ENEMY_RADIUS ,
						missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , MISSILE_PLAYER_RADIUS ) == true &&
						GetEnemyState(cnt3)->posZ > missilePlayerState[cnt].posZ - MISSILE_PLAYER_HIT_ZONE && 
						GetEnemyState(cnt3)->posZ < missilePlayerState[cnt].posZ + MISSILE_PLAYER_HIT_ZONE )
					{
						GetEnemyState(cnt3)->nLife = GetEnemyState(cnt3)->nLife - AF_MISSILE_PLAYER_ATTACK;
						if( GetEnemyState(cnt3)->nLife <= 0 )
						{
							busterEnemyNumPlusOne();
							GetEnemyState(cnt3)->bUse = false;
							GetEnemyState(cnt3)->bLockOn = false;
							missilePlayerState[cnt].bUse = false;
							SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_001 );
							SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , GetEnemyState(cnt3)->posZ * 800 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 800 , 18 , AF_EXPLOSION_001 );
							SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_001 );
							if( GetEnemyState(cnt3)->aiPattern == ENEMY_AI_BOSS1 )
							{
								busterEnemyBoss1();
								SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_003 );
								SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , GetEnemyState(cnt3)->posZ * 800 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 800 , 18 , AF_EXPLOSION_003 );
								SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_003 );
							
								SetAfExplosion( missilePlayerState[cnt].intPosX , missilePlayerState[cnt].intPosY , GetEnemyState(cnt3)->posZ * 800 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 800 , 18 , AF_EXPLOSION_003 );
								SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 100 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 100 , 10 , AF_EXPLOSION_003 );
								PlusAfScore( 1000000 );
								PlusAfScore( ( 100 - GetPlayerState()->nLife ) * 1000 );
							}
							PlusAfComboNumber( 1 );
							PlusAfScore( 100 * GetAfComboNumber() );
						}
						SetAfExplosion( missilePlayerState[cnt].intPosX + rand() % 3 - rand() % 3 , missilePlayerState[cnt].intPosY + rand() % 3 - rand() % 3 , GetEnemyState(cnt3)->posZ * 1 , 0 , 0 , GetEnemyState(cnt3)->moveZ * 1 , 10 , AF_EXPLOSION_001 );

						PlusAfScore( 3 * GetAfComboNumber() );
						break;
					}
				}
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


			//�A�j���[�V�����ݒ�
			if( ++g_nCounterAnim == AF_MISSILE_PLAYER_ANIM_SPAN )
			{
				g_nCounterAnim = 0;
				g_nPatternAnim = ( ++g_nPatternAnim == AF_MISSILE_PLAYER_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
	
			texWidth = 1.0f / AF_MISSILE_PLAYER_ANIM_DIV_X;
			texHeight = 1.0f / AF_MISSILE_PLAYER_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_MISSILE_PLAYER_ANIM_DIV_X );
			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_MISSILE_PLAYER_ANIM_DIV_X ) ) + 0;
	
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
										g_posPolygon.y + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
			pVtx[QUOTE_VERTEX * cnt + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

			pVtx[QUOTE_VERTEX * cnt + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
										g_posPolygon.y + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
			
		}
		if( missilePlayerState[cnt].bUse == false )
		{			
			pVtx[QUOTE_VERTEX * cnt].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
			pVtx[QUOTE_VERTEX * cnt + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 0 );
		}
	}

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfMissilePlayer->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfMissilePlayer( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfMissilePlayer( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfMissilePlayer );
	
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
		if( missilePlayerState[cnt].bUse != false )
		{			
			/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
			pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfMissilePlayer , 0 , sizeof( VERTEX_2D ) );

			/*���e�̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , QUOTE_VERTEX * cnt , 2 );
		}
	}
}

/*******************************************************************************
* �֐���	:void DrawAfMissilePlayercore( void )
*		���e�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfMissilePlayercore( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfMissilePlayer );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfMissilePlayer , 0 , sizeof( VERTEX_2D ) );
	
	
	/*���e�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 + AF_MISSILE_PLAYER_VERTEX_MAX, 2 );
}

/*******************************************************************************
* �֐���	:MISSILE_PLAYER_STATUS* GetMissilePlayerState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
MISSILE_PLAYER_STATUS* GetMissilePlayerState( int num )
{
	return &missilePlayerState[num];
}

/*******************************************************************************
* �֐���	:MISSILE_PLAYER_STATUS* GetMissilePlayerState( int num )
*		���e�̃X�e�[�^�X���擾���܂��B
* ����		:int num	���肷��\���̂̔ԍ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:���e�̃X�e�[�^�X���擾���܂��B�B
*******************************************************************************/
void SetMissilePlayerState( float posX , float posY , float posZ , float moveX , float moveY , float moveZ )
{
	for( int cnt = 0; cnt < MISSILE_PLAYER_MAX_NUM; cnt++ )
	{
		if( missilePlayerState[cnt].bUse == false )
		{
			missilePlayerState[cnt].bUse = true;
			missilePlayerState[cnt].posStatus.posX = posX;
			missilePlayerState[cnt].posStatus.posY = posY;
			missilePlayerState[cnt].posStatus.vector = 0.0f;
			missilePlayerState[cnt].moveX = moveX;
			missilePlayerState[cnt].moveY = moveY;
			missilePlayerState[cnt].moveZ = moveZ;
			missilePlayerState[cnt].posZ = posZ;
			missilePlayerState[cnt].stayCnt = 0;

			missilePlayerState[cnt].intPosX = ( int )( missilePlayerState[cnt].posStatus.posX ) * 1000;
			missilePlayerState[cnt].intPosY = ( int )( missilePlayerState[cnt].posStatus.posY ) * 1000;
				  
			missilePlayerState[cnt].nLife = AF_MISSILE_PLAYER_DEFAULT_LIFE;
			break;
		}
	}
}