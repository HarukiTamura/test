/*******************************************************************************
* �^�C�g��	:�G�l�~�[.cpp
* �t�@�C����:enemy.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/06/24
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include <math.h>
#include "main.h"
#include "game.h"
#include "enemy.h"
#include "input.h"
#include "unoHitChecker.h"
#include "unoDirectX.h"
#include "barrier.h"
#include "bullet.h"
#include "player01.h"
#include "explosion.h"
#include "stageManager.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define ENEMY_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define ENEMY_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define ENEMY_ANIM_SPAN		( 10 )	/*�A�j���[�V�����Ԋu*/
#define ENEMY_ANIM_DIV_X	( 1 )	/*��������*/
#define ENEMY_ANIM_DIV_Y	( 1 )	/*����������*/

#define ENEMY_OUTPUT_SIZE	( 0.25 ) /*�����\���{��*/ 

#define MOVE_NUM ( 2.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.20f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.05f )	/*������*/

#define ENEMY_DELETE_RADIUS	( 800000 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffenemy = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureenemy;	/*�G�l�~�[�̃e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static ENEMY g_aEnemy[ENEMY_MAX];	/*�G�l�~�[�̍\����*/
static bool frameSwitch = false;	/*���݂ɐ؂�ւ��A�t���[�����ς�����������ʂ���B*/
static int browseEnemyCount = ENEMY_MAX;	/*�G�l�~�[�������J�E���g�̊J�n�n�_*/
static int createEnemyCount = 0;	/*�G�l�~�[���J�E���g*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;	/*�A�j���[�V�����Ԋu�J�E���g*/
static int g_nPatternAnim = 0;	/*�p�^�[�����J�E���g*/

static int lastEnemy = 0;	/*�G�l�~�[�����݂����Ō�̕���*/

/*******************************************************************************
* �֐���	:HRESULT InitEnemy( void )
*		�G�l�~�[�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitEnemy( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * ENEMY_VERTEX_MAX * ENEMY_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffenemy , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/enemy.png" , &g_pTextureenemy );
	D3DXGetImageInfoFromFile( "data/TEXTURE/enemy.png" , &info );

	ZeroMemory( &g_aEnemy[0] , sizeof( ENEMY ) * ENEMY_MAX );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffenemy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / ENEMY_ANIM_DIV_X ) * ENEMY_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / ENEMY_ANIM_DIV_Y ) * ENEMY_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , ENEMY_MAX );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffenemy->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	/*�G�l�~�[�X�V�p�X�e�[�^�X�̏�����*/
	frameSwitch = false;
	browseEnemyCount = ENEMY_MAX;
	createEnemyCount = 0;
	
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitEnemy( void )
*		�G�l�~�[�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitEnemy( void )
{
	if( g_pVtxBuffenemy != NULL )
	{
		g_pVtxBuffenemy->Release();
		g_pVtxBuffenemy = NULL;
	}
	if( g_pTextureenemy != NULL )
	{
		g_pTextureenemy->Release();
		g_pTextureenemy = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateEnemy( void )
*		�G�l�~�[�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateEnemy( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffenemy->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*�G�l�~�[�̍X�V*/
	for( int nCntEnemy = 0; nCntEnemy < ENEMY_MAX; nCntEnemy++ )
	{
		if( g_aEnemy[nCntEnemy].bUse == true )
		{
			if( HitTwinCircle( INT_FIELD_CENTER_X , INT_FIELD_CENTER_Y , ENEMY_DELETE_RADIUS , 
				g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , g_aEnemy[nCntEnemy].radius ) != true  )
			{
				g_aEnemy[nCntEnemy].bUse = false;
			}
			if( g_aEnemy[nCntEnemy].state == ENEMY_ST_EXPLOSION )
			{
				g_aEnemy[nCntEnemy].nLife--;
				SetExplosion( g_aEnemy[nCntEnemy].posX - rand() % g_aEnemy[nCntEnemy].radius * 2 + rand() % g_aEnemy[nCntEnemy].radius * 2 ,
					g_aEnemy[nCntEnemy].posY - rand() % g_aEnemy[nCntEnemy].radius * 2 + rand() % g_aEnemy[nCntEnemy].radius * 2, 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );
			}
			//�����`�F�b�N
			if( g_aEnemy[nCntEnemy].nLife <= 0 )
			{
				if( g_aEnemy[nCntEnemy].state == ENEMY_ST_EXPLOSION )
				{
					g_aEnemy[nCntEnemy].bUse = false;
				}
				else
				{
					g_aEnemy[nCntEnemy].nLife = 100;
					g_aEnemy[nCntEnemy].state = ENEMY_ST_EXPLOSION;
				}
			}

			switch( g_aEnemy[nCntEnemy].aiType )
			{
				case ENEMY_AI_NORMAL:	/*���͒l�ňړ�*/
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
					break;
				case ENEMY_AI_0001://���͕����ɏ��X�ɉ������Ȃ���E�o
					if( getStageFps() % 31 == 30 )
					{
						int cnt = 0;
						for( cnt = 0; cnt < 100; cnt++ )
						{
							SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 5900 * sin( D3DX_PI * 0.05 * cnt + 0.01 ) , 5900  * cos( D3DX_PI * 0.05 * cnt + 0.01 ) , 0 , 12 , BULLET_NORMAL);
						}
					}
					if( getStageFps() % 200 == 199 )
					{
						int cnt = 0;
						for( cnt = 0; cnt < 100; cnt++ )
						{
							SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 2000 * sin( D3DX_PI * 0.05 * cnt + 0.01 ) , 2000  * cos( D3DX_PI * 0.05 * cnt + 0.01 ) , 0 , 12 , BULLET_NORMAL);
						}
					}
					g_aEnemy[nCntEnemy].moveY += 1.25;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
					break;
				case ENEMY_AI_0002:
					if( getStageFps() % 36 <= 0 )
					{

						float angle = atan2f( ( float )( GetPlayerState01()->intPosX - g_aEnemy[nCntEnemy].posX ), ( float )( GetPlayerState01()->intPosY - g_aEnemy[nCntEnemy].posY ) );
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 5000 * sin( angle ) , 5000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
					}
					g_aEnemy[nCntEnemy].moveY += 1.25;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
					break;
				case ENEMY_AI_0003:
					if( getStageFps() % 36 <= 0 )
					{
						float angle = atan2f( ( float )( GetPlayerState01()->intPosX - g_aEnemy[nCntEnemy].posX ), ( float )( GetPlayerState01()->intPosY - g_aEnemy[nCntEnemy].posY ) );
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 7000 * sin( angle ) , 7000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 7000 * sin( angle - 0.2 ) , 7000  * cos( angle - 0.2 ) , 0 , 12 , BULLET_NORMAL);
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 7000 * sin( angle + 0.2 ) , 7000  * cos( angle + 0.2 ) , 0 , 12 , BULLET_NORMAL);
					}
					g_aEnemy[nCntEnemy].moveY += 1.02;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
				case ENEMY_AI_0004:
					if( getStageFps() % 36 <= 0 )
					{
						float angle = atan2f( ( float )( GetPlayerState01()->intPosX - g_aEnemy[nCntEnemy].posX ), ( float )( GetPlayerState01()->intPosY - g_aEnemy[nCntEnemy].posY ) );
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle ) , 3000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 4000 * sin( angle ) , 2000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 1000 * sin( angle ) , 1000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);

						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle ) , 3000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 4000 * sin( angle ) , 2000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 1000 * sin( angle ) , 1000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);

						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle ) , 3000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 4000 * sin( angle ) , 2000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);
						SetBulletLiar( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 1000 * sin( angle ) , 1000  * cos( angle ) , 0 , 12 , BULLET_NORMAL);

						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle - 0.2 ) , 3000  * cos( angle - 0.2 ) , 0 , 12 , BULLET_NORMAL);
						SetBulletEnemy( g_aEnemy[nCntEnemy].posX , g_aEnemy[nCntEnemy].posY , 0 , 3000 * sin( angle + 0.2 ) , 3000  * cos( angle + 0.2 ) , 0 , 12 , BULLET_NORMAL);

					}
					g_aEnemy[nCntEnemy].moveY += 1.02;
					g_aEnemy[nCntEnemy].posX += g_aEnemy[nCntEnemy].moveX;
					g_aEnemy[nCntEnemy].posY += g_aEnemy[nCntEnemy].moveY;
				case ENEMY_AI_0005:
				case ENEMY_AI_0006:
				case ENEMY_AI_0007:
				case ENEMY_AI_0008:
				case ENEMY_AI_0009:
				case ENEMY_AI_0010:
				case ENEMY_AI_0011:
				case ENEMY_AI_0012:
				case ENEMY_AI_MAX:
				default:
					{
						break;
					}
			}
			

			/*�p�^�[���A�j���[�V�������O�v�Z����*/
			if( ++g_nCounterAnim %= ENEMY_ANIM_SPAN )
			{
				++g_nPatternAnim %= ENEMY_ANIM_MAX;
				//g_nPatternAnim = ( ++g_nPatternAnim == ENEMY_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
			texWidth = 1.0f / ENEMY_ANIM_DIV_X;
			texHeight = 1.0f / ENEMY_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % ENEMY_ANIM_DIV_X );
			
		
			nowPatternY = ( float )( ( int )( g_nPatternAnim / ENEMY_ANIM_DIV_X ) ) + g_aEnemy[nCntEnemy].type;
			
			
			switch( g_aEnemy[nCntEnemy].state )
			{
				case ENEMY_ST_NORMAL:
					{
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
						break;
					}
				case ENEMY_ST_DAMAGE:
					{
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 122 , 122 , 220 , 235 );					
						g_aEnemy[nCntEnemy].state = ENEMY_ST_NORMAL;
						break;
					}
				case ENEMY_ST_EXPLOSION:
					{
						int color = ( int )( 255 * ( float( g_aEnemy[nCntEnemy].nLife ) / 255.0 ) );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , color );
						break;
					}
				default:
					{
						break;
					}
			}

			/*�e�N�X�`�����W����*/
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

			/*���_���W�ݒ�*/
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].pos.z = 0.0f;
											      
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].pos.z = 0.0f;
											      
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].pos.z = 0.0f;
											      
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].pos.x = g_aEnemy[nCntEnemy].posX * 0.001 + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].pos.y = g_aEnemy[nCntEnemy].posY * 0.001 + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].pos.z = 0.0f;
		}
		if( g_aEnemy[nCntEnemy].bUse == false )
		{
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntEnemy * ENEMY_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );					
		}
	}
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffenemy->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawEnemy( void )
*		�G�l�~�[�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawEnemy( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffenemy , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureenemy );
	
	for( int nCntEnemy = 0; nCntEnemy < ENEMY_MAX; nCntEnemy++ )
	{
		if( g_aEnemy[nCntEnemy].bUse == true )
		{
			/*�G�l�~�[�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntEnemy * ENEMY_VERTEX_MAX , 2 );
		}
	}
}

/*******************************************************************************
* �֐���	:void SetEnemy( D3DXVECTOR3 pos , D3DXVECTOR3 vecMove , int nLife , ENEMY_TYPE type )
*		�G�l�~�[�𐶐�����֐��ł��B
* ����		:D3DXVECTOR3 pos		�G�l�~�[�̏o���ʒu�ł��B
*			 D3DXVECTOR3 vecMove	�G�l�~�[�̈ړ��ʂł��B
*			 int nLife				�G�l�~�[�̎����ł��B
*			(ENEMY_TYPE type		�G�l�~�[�̎�ނł��B)	
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void SetEnemy( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , CATEGORY category , ENEMY_TYPE type , ENEMY_AI_TYPE aiType )
{
	int nCntEnemy = ENEMY_MAX;//browseEnemyCount;
	while( nCntEnemy-- )
	{
		if( g_aEnemy[nCntEnemy].bUse == false )
		{
			memset( &g_aEnemy[nCntEnemy] , 0 , sizeof( ENEMY ) );
			g_aEnemy[nCntEnemy].posX = posX;
			g_aEnemy[nCntEnemy].posY = posY;
			g_aEnemy[nCntEnemy].posZ = posZrot;
			g_aEnemy[nCntEnemy].moveX = moveX;
			g_aEnemy[nCntEnemy].moveY = moveY;
			g_aEnemy[nCntEnemy].moveZ = moveZrot;
			g_aEnemy[nCntEnemy].nLife = nLife;
			g_aEnemy[nCntEnemy].radius = 70000;
			g_aEnemy[nCntEnemy].bUse = true;
			g_aEnemy[nCntEnemy].category = category;
			g_aEnemy[nCntEnemy].type = type;
			g_aEnemy[nCntEnemy].aiType = aiType;
			browseEnemyCount = nCntEnemy;	
			createEnemyCount++;
			break;
		}
	}
}

/*******************************************************************************
* �֐���	:ENEMY* GetEnemy( void )
*		�G�l�~�[��n�������֐��ł��B
* ����		:int enemyNum	�G�l�~�[�̗v�f
* �߂�l	:ENEMY*			�G�l�~�[�\���̂̃A�h���X
* ����		:�G�l�~�[�̔��a��n���֐��ł��B
*******************************************************************************/
ENEMY* GetEnemy( int enemyNum )
{
	return &g_aEnemy[enemyNum];
}
