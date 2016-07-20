/*******************************************************************************
* �^�C�g��	:�e.cpp
* �t�@�C����:bullet.cpp
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
#include "bullet.h"
#include "explosion.h"
#include "input.h"
#include "unoHitChecker.h"
#include "unoDirectX.h"
#include "barrier.h"
#include "player01.h"
#include "enemy.h"
#include "comboNumber.h"
#include "score.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define BULLET_DELETE_RADIUS	( 600000 )

#define BULLET_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define BULLET_ANIM_MAX		( 3 )	/*�A�j���[�V�����R�}��*/
#define BULLET_ANIM_SPAN	( 10 )	/*�A�j���[�V�����Ԋu*/
#define BULLET_ANIM_DIV_X	( 3 )	/*��������*/
#define BULLET_ANIM_DIV_Y	( 5 )	/*����������*/

#define BULLET_OUTPUT_SIZE	( 0.65 ) /*�����\���{��*/ 

#define MOVE_NUM ( 2.5f )	/*�ړ���*/
#define SPIN_SPEED	( 0.12f )	/*��]�X�s�[�h*/
#define MAG_SPEED	( 0.20f )	/*�g��X�s�[�h*/

#define MAG_MAX		( 10.00f )	/*�g����E*/
#define MAG_MIN 	( 0.00f )	/*�k�����E*/

#define DEFAULT_MOVE	( 0.0f )	/*�ړ��ʍŏI�l*/
#define MOVE_ATTEN		( 0.05f )	/*������*/


/*******************************************************************************
* �\���̒�`
*******************************************************************************/
//�e�̃X�e�[�^�X
typedef struct
{
	//�����y���ׂ̈�int�^�Ƃ��Ĉ����B���O���������_�ȉ��Ƃ��Ĉ����B
	int posX;	//�ʒu
	int posY;	//�ʒu
	int posZ;	//�ʒu
	int moveX;	//�ړ���
	int moveY;	//�ړ���
	int moveZ;	//�ړ���
	int vector;	//�p�x
	int nowMode;	//���݃��[�h
	//32byte
	int attack;	//�U����
	int radius;	//�ӂ蔼�a
	int nCurveCnt;		//�J�[�u�E�o�E���h��
	int nLife;			//����
	bool bCurve;		//�J�[�u�������ǂ���
	bool bUse;			//�g�p���Ă��邩�ǂ���
	BULLET_TYPE type;	//�e�̎��
	CATEGORY category;	//��������
}BULLET;

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffbullet = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturebullet;	/*�e�̃e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static BULLET g_aBullet[BULLET_MAX];	/*�e�̍\����*/
static bool frameSwitch = false;	/*���݂ɐ؂�ւ��A�t���[�����ς�����������ʂ���B*/
static int createBulletCount = 0;	/*�e���J�E���g*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;	/*�A�j���[�V�����Ԋu�J�E���g*/
static int g_nPatternAnim = 0;	/*�p�^�[�����J�E���g*/

static int lastBullet = 0;	/*�e�����݂����Ō�̕���*/

/*******************************************************************************
* �֐���	:HRESULT InitBullet( void )
*		�e�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitBullet( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * BULLET_VERTEX_MAX * BULLET_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffbullet , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/bullet.png" , &g_pTexturebullet );
	D3DXGetImageInfoFromFile( "data/TEXTURE/bullet.png" , &info );

	ZeroMemory( &g_aBullet[0] , sizeof( BULLET ) * BULLET_MAX );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / BULLET_ANIM_DIV_X ) * BULLET_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / BULLET_ANIM_DIV_Y ) * BULLET_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , BULLET_MAX );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffbullet->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	/*�e�X�V�p�X�e�[�^�X�̏�����*/
	frameSwitch = false;
	createBulletCount = 0;
	
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitBullet( void )
*		�e�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitBullet( void )
{
	if( g_pVtxBuffbullet != NULL )
	{
		g_pVtxBuffbullet->Release();
		g_pVtxBuffbullet = NULL;
	}
	if( g_pTexturebullet != NULL )
	{
		g_pTexturebullet->Release();
		g_pTexturebullet = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateBullet( void )
*		�e�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateBullet( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffbullet->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*�e�̍X�V*/
	for( int nCntBullet = 0; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
#ifdef _DEBUG
			/*��������*/
			if( GetKeyboardPress( DIK_UP ) )
			{
				g_aBullet[nCntBullet].moveY-=100;
			}
			if( GetKeyboardPress( DIK_DOWN ) )
			{
				g_aBullet[nCntBullet].moveY+=100;
			}
			if( GetKeyboardPress( DIK_LEFT ) )
			{
				g_aBullet[nCntBullet].moveX-=100;
			}
			if( GetKeyboardPress( DIK_RIGHT ) )
			{
				g_aBullet[nCntBullet].moveX+=100;
			}
#endif		
		
				
			if( g_aBullet[nCntBullet].type == BULLET_NORMAL )
			{
				//�����`�F�b�N&�ʏ�e���Ŕ���(��ʊO)
				if( g_aBullet[nCntBullet].nLife <= 0 ||
					HitTwinCircle( INT_FIELD_CENTER_X , INT_FIELD_CENTER_Y , BULLET_DELETE_RADIUS , 
					g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) != true  )
				{
					
					/*//�ʏ�e���Ŕ���(��ʊO)
					if( g_aBullet[nCntBullet].bUse == true &&	//�����y���i�O�����Œe�������Ă����ꍇ��������������ׁj
						( g_aBullet[nCntBullet].posX  < INT_FIELD_LEFT || g_aBullet[nCntBullet].posX > INT_FIELD_RIGHT ||
						g_aBullet[nCntBullet].posY < INT_FIELD_TOP || g_aBullet[nCntBullet].posY > INT_FIELD_BOTTOM  ) )
					{
						g_aBullet[nCntBullet].bUse = false;
						continue;
					}
					*/
					g_aBullet[nCntBullet].bUse = false;
					createBulletCount--;
				}

				/*//���V�є��]�e
				if( ( g_aBullet[nCntBullet].posX  < INT_FIELD_LEFT || g_aBullet[nCntBullet].posX > INT_FIELD_RIGHT ) )
				{
					g_aBullet[nCntBullet].moveX = -g_aBullet[nCntBullet].moveX;
				}
				if( ( g_aBullet[nCntBullet].posY < INT_FIELD_TOP || g_aBullet[nCntBullet].posY > INT_FIELD_BOTTOM ) )
				{
					g_aBullet[nCntBullet].moveY = -g_aBullet[nCntBullet].moveY;
				}*/
			}
			if( g_aBullet[nCntBullet].type == BULLET_HOMING )
			{
				//�����`�F�b�N
				if( g_aBullet[nCntBullet].nLife <= 0 )
				{
					g_aBullet[nCntBullet].bUse = false;
				}
				//�ʏ�e���Ŕ���(��ʊO)
				if( g_aBullet[nCntBullet].bUse == true &&	//�����y���i�O�����Œe�������Ă����ꍇ��������������ׁj
					( g_aBullet[nCntBullet].posX < INT_SCREEN_LEFT || g_aBullet[nCntBullet].posX > INT_SCREEN_RIGHT ||
					g_aBullet[nCntBullet].posY < INT_SCREEN_TOP || g_aBullet[nCntBullet].posY > INT_SCREEN_BOTTOM ) )
				{
					g_aBullet[nCntBullet].bUse = false;
				}
			}
			g_aBullet[nCntBullet].posX += g_aBullet[nCntBullet].moveX;
			g_aBullet[nCntBullet].posY += g_aBullet[nCntBullet].moveY;
			
			//�����蔻��
			if( ( g_aBullet[nCntBullet].posX < INT_FIELD_LEFT || g_aBullet[nCntBullet].posX > INT_FIELD_RIGHT ||
				g_aBullet[nCntBullet].posY < INT_FIELD_TOP || g_aBullet[nCntBullet].posY > INT_FIELD_BOTTOM ) )//��ʊO�ɂ���ꍇ������X���[
			{

			}
			else if( ( g_aBullet[nCntBullet].category == CATEGORY_ENEMY || g_aBullet[nCntBullet].category == CATEGORY_LIAR ) )
			{
				if(	GetPlayerState01()->mode !=	PLAYER_MODE_HYPERMODE &&
					HitTwinCircle( GetPlayerState01()->intPosX , GetPlayerState01()->intPosY , PLAYER_RADIUS , 
					g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
				{
					GetPlayerState01()->mode = PLAYER_MODE_DAMAGE;
				}

				if( HitTwinCircle( GetPlayerState01()->intPosX , GetPlayerState01()->intPosY , GetBarrierRadiusInt() , 
					g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
				{
					//SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );
					if(	g_aBullet[nCntBullet].category == CATEGORY_LIAR )
					{					
						GetPlayerState01()->liarNum = ( GetPlayerState01()->liarNum < LIAR_MAX ? GetPlayerState01()->liarNum + 3 : GetPlayerState01()->liarNum );
					}
					SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_002 );
					
					g_aBullet[nCntBullet].bUse = false;
				}
			}
			else if( g_aBullet[nCntBullet].category == CATEGORY_PLAYER )
			{
				for( int enemyCnt = 0; enemyCnt < ENEMY_MAX; enemyCnt++ )
				{
					if( GetEnemy( enemyCnt )->bUse == true && GetEnemy( enemyCnt )->state != ENEMY_ST_EXPLOSION &&
						HitTwinCircle( GetEnemy( enemyCnt )->posX , GetEnemy( enemyCnt )->posY , GetEnemy( enemyCnt )->radius , 
						g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
					{	
						g_aBullet[nCntBullet].bUse = false;
						SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );							
						GetEnemy( enemyCnt )->nLife -= g_aBullet[nCntBullet].attack;
						PlusComboNumber( 1 );
						if( GetEnemy( enemyCnt )->nLife <= 0 )
						{
							PlusScore( GetEnemy( enemyCnt )->radius * ( 1 + ( 0.03 * GetComboNumber() ) ) );
						}
						GetEnemy( enemyCnt )->state = ENEMY_ST_DAMAGE;
						//GetPlayerState01()->nCombo = 1;
						
						break;
					}
				}
			}


			/*�p�^�[���A�j���[�V�������O�v�Z����*/
			if( ++g_nCounterAnim %= BULLET_ANIM_SPAN )
			{
				++g_nPatternAnim %= BULLET_ANIM_MAX;
				//g_nPatternAnim = ( ++g_nPatternAnim == BULLET_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
			
			texWidth = 1.0f / BULLET_ANIM_DIV_X;
			texHeight = 1.0f / BULLET_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % BULLET_ANIM_DIV_X );
			
			if( g_aBullet[nCntBullet].type == BULLET_NORMAL )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 0;
			}
			else if( g_aBullet[nCntBullet].type == BULLET_RAPID )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 1;
			}
			else if( g_aBullet[nCntBullet].type == BULLET_HOMING )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 2;
			}
			else if( g_aBullet[nCntBullet].type == BULLET_PENETRATE )
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 3;
			}
			else
			{
				nowPatternY = ( float )( ( int )( g_nPatternAnim / BULLET_ANIM_DIV_X ) ) + 2;
			}
			
			switch( g_aBullet[nCntBullet].category )
			{
				case CATEGORY_PLAYER:
					{
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
						/*
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 255 , 122 , 255 );
						*/
						break;
					}
				case CATEGORY_ENEMY:
					{
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 54 , 182 , 250 , 200 );
						break;
					}
				case CATEGORY_NEUTRAL:
					{
						
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 255 , 0 , 122 );
						break;
					}
				case CATEGORY_ANTIBREAK:
					{
						break;
					}
				case CATEGORY_LIAR:
					{
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 0 , 0 , 255 );
						break;
					}
				default:
					{
						break;
					}
			}

			/*�e�N�X�`�����W����*/
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

			/*���_���W�ݒ�*/
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].pos.x = g_aBullet[nCntBullet].posX * 0.001 + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].pos.y = g_aBullet[nCntBullet].posY * 0.001 + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].pos.z = 0.0f;
											      
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].pos.x = g_aBullet[nCntBullet].posX * 0.001 + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].pos.y = g_aBullet[nCntBullet].posY * 0.001 + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].pos.z = 0.0f;
											      
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].pos.x = g_aBullet[nCntBullet].posX * 0.001 + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].pos.y = g_aBullet[nCntBullet].posY * 0.001 + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].pos.z = 0.0f;
											      
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].pos.x = g_aBullet[nCntBullet].posX * 0.001 + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].pos.y = g_aBullet[nCntBullet].posY * 0.001 + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].pos.z = 0.0f;

		}
		if( g_aBullet[nCntBullet].bUse == false )
		{
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
			pVtx[nCntBullet * BULLET_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 0 );
		}
	}
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffbullet->Unlock();


	//if( frameSwitch  )
	//bool frameSwitch;	/*���݂ɐ؂�ւ��A�t���[�����ς�����������ʂ���B*/
	createBulletCount = 0;
}

/*******************************************************************************
* �֐���	:void DrawBullet( void )
*		�e�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawBullet( void )
{
	int lastBnemy = 0;
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffbullet , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturebullet );
	
	for( int nCntBullet = 0; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
			/*�e�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntBullet * BULLET_VERTEX_MAX , 2 );
			lastBullet = nCntBullet;
		}
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DEST�����̂܂ܕ`�悷���
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	for( int nCntBullet = 0; nCntBullet <= lastBullet; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
			/*�e�̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntBullet * BULLET_VERTEX_MAX , 2 );
		}
	}
	
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/
	
}

/*******************************************************************************
* �֐���	:void SetBullet( D3DXVECTOR3 pos , D3DXVECTOR3 vecMove , int nLife , ENEMY_TYPE type )
*		�e�𐶐�����֐��ł��B
* ����		:D3DXVECTOR3 pos		�e�̏o���ʒu�ł��B
*			 D3DXVECTOR3 vecMove	�e�̈ړ��ʂł��B
*			 int nLife				�e�̎����ł��B
*			(ENEMY_TYPE type		�e�̎�ނł��B)	
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void SetBullet( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , CATEGORY category , BULLET_TYPE type )
{
	
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 100;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = category;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}
/*�v���C���[�w���*/
void SetBulletPlayer( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , BULLET_TYPE type )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 100;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = CATEGORY_PLAYER;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}
/*�G�l�~�[�w���*/
void SetBulletEnemy( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , BULLET_TYPE type )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 1;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = CATEGORY_ENEMY;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}
/*�R�����w���*/
void SetBulletLiar( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , BULLET_TYPE type )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == false )
		{
			memset( &g_aBullet[nCntBullet] , 0 , sizeof( BULLET ) );
			g_aBullet[nCntBullet].posX = posX;
			g_aBullet[nCntBullet].posY = posY;
			g_aBullet[nCntBullet].posZ = posZrot;
			g_aBullet[nCntBullet].moveX = moveX;
			g_aBullet[nCntBullet].moveY = moveY;
			g_aBullet[nCntBullet].moveZ = moveZrot;
			g_aBullet[nCntBullet].nLife = nLife;
			g_aBullet[nCntBullet].attack = 1;
			g_aBullet[nCntBullet].radius = 100;
			g_aBullet[nCntBullet].type = type;
			g_aBullet[nCntBullet].bUse = true;
			g_aBullet[nCntBullet].category = CATEGORY_LIAR;
			createBulletCount = nCntBullet;	
			break;
		}
	}
}


/*******************************************************************************
* �֐���	:void DeleteBullet( int posX , int posY , int radius )
*		�e�����𔭐�������֐��ł��B
* ����		:int posX		�����n�_X���W	
*			 int posY		�����n�_Y���W
*			 int radius		���a
* �߂�l	:void �߂�l�Ȃ�
* ����		:�w��n�_�𒆐S�Ƃ����~�̒��ɂ���G�e�����ł�����֐��ł��B
*******************************************************************************/
void DeleteBullet( int posX , int posY , int radius )
{
	for( int nCntBullet = createBulletCount; nCntBullet < BULLET_MAX; nCntBullet++ )
	{
		if( g_aBullet[nCntBullet].bUse == true )
		{
			
			if( HitTwinCircle( posX , posY , radius , g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , g_aBullet[nCntBullet].radius ) == true )
			{	
				g_aBullet[nCntBullet].bUse = false;
				SetExplosion( g_aBullet[nCntBullet].posX , g_aBullet[nCntBullet].posY , 0 , 0 , 0 , 0 , 12 , EXPLOSION_001 );							
			}
		}
	}
}