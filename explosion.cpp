/*******************************************************************************
* �^�C�g��	:����.cpp
* �t�@�C����:explosion.cpp
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
#include "explosion.h"
#include "input.h"
#include "unoHitChecker.h"
#include "unoDirectX.h"
#include "barrier.h"
#include "player01.h"
#include "enemy.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define EXPLOSION_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define EXPLOSION_ANIM_MAX		( 4 )	/*�A�j���[�V�����R�}��*/
#define EXPLOSION_ANIM_SPAN	( 20 )	/*�A�j���[�V�����Ԋu*/
#define EXPLOSION_ANIM_DIV_X	( 4 )	/*��������*/
#define EXPLOSION_ANIM_DIV_Y	( 2 )	/*����������*/

#define EXPLOSION_OUTPUT_SIZE	( 0.2 ) /*�����\���{��*/ 

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
//�����̃X�e�[�^�X
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
	int type;
	int attack;	//�U����
	int radius;	//�ӂ蔼�a
	int nLife;			//����
	bool bCurve;		//�J�[�u�������ǂ���
	bool bUse;			//�g�p���Ă��邩�ǂ���
}EXPLOSION;

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffexplosion = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureexplosion;	/*�����̃e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static EXPLOSION g_aExplosion[EXPLOSION_MAX];	/*�����̍\����*/
static bool frameSwitch = false;	/*���݂ɐ؂�ւ��A�t���[�����ς�����������ʂ���B*/

static int createExplosionCount = 0;	/*�������J�E���g*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;	/*�A�j���[�V�����Ԋu�J�E���g*/
static int g_nPatternAnim = 0;	/*�p�^�[�����J�E���g*/

static int lastExplosion = 0;	/*���������݂����Ō�̕���*/

/*******************************************************************************
* �֐���	:HRESULT InitExplosion( void )
*		�����̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitExplosion( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * EXPLOSION_VERTEX_MAX * EXPLOSION_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffexplosion , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/explosion.png" , &g_pTextureexplosion );
	D3DXGetImageInfoFromFile( "data/TEXTURE/explosion.png" , &info );

	ZeroMemory( &g_aExplosion[0] , sizeof( EXPLOSION ) * EXPLOSION_MAX );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffexplosion->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / EXPLOSION_ANIM_DIV_X ) * EXPLOSION_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / EXPLOSION_ANIM_DIV_Y ) * EXPLOSION_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , EXPLOSION_MAX );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffexplosion->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	/*�����X�V�p�X�e�[�^�X�̏�����*/
	frameSwitch = false;
	
	createExplosionCount = 0;
	
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitExplosion( void )
*		�����̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitExplosion( void )
{
	if( g_pVtxBuffexplosion != NULL )
	{
		g_pVtxBuffexplosion->Release();
		g_pVtxBuffexplosion = NULL;
	}
	if( g_pTextureexplosion != NULL )
	{
		g_pTextureexplosion->Release();
		g_pTextureexplosion = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateExplosion( void )
*		�����̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateExplosion( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffexplosion->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*�����̍X�V*/
	for( int nCntExplosion = 0; nCntExplosion < EXPLOSION_MAX; nCntExplosion++ )
	{
		if( g_aExplosion[nCntExplosion].bUse == true )
		{
				
			g_aExplosion[nCntExplosion].nLife--;
			//�����`�F�b�N
			if( g_aExplosion[nCntExplosion].nLife <= 0 )
			{
				g_aExplosion[nCntExplosion].bUse = false;
				continue;
			}
			
			g_aExplosion[nCntExplosion].posX += g_aExplosion[nCntExplosion].moveX;
			g_aExplosion[nCntExplosion].posY += g_aExplosion[nCntExplosion].moveY;


			/*�p�^�[���A�j���[�V�������O�v�Z����*/
			if( ++g_nCounterAnim %= EXPLOSION_ANIM_SPAN )
			{
				++g_nPatternAnim %= EXPLOSION_ANIM_MAX;
				//g_nPatternAnim = ( ++g_nPatternAnim == EXPLOSION_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
			texWidth = 1.0f / EXPLOSION_ANIM_DIV_X;
			texHeight = 1.0f / EXPLOSION_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % EXPLOSION_ANIM_DIV_X );
			

			nowPatternY = ( float )( ( int )( g_nPatternAnim / EXPLOSION_ANIM_DIV_X ) ) + g_aExplosion[nCntExplosion].type;

			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			
			
			/*�e�N�X�`�����W����*/
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;

			/*���_���W�ݒ�*/
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 0].pos.x = g_aExplosion[nCntExplosion].posX * 0.001 + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 0].pos.y = g_aExplosion[nCntExplosion].posY * 0.001 + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 0].pos.z = 0.0f;
											      
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 1].pos.x = g_aExplosion[nCntExplosion].posX * 0.001 + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 1].pos.y = g_aExplosion[nCntExplosion].posY * 0.001 + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 1].pos.z = 0.0f;
											      
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 2].pos.x = g_aExplosion[nCntExplosion].posX * 0.001 + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 2].pos.y = g_aExplosion[nCntExplosion].posY * 0.001 + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 2].pos.z = 0.0f;
											      
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 3].pos.x = g_aExplosion[nCntExplosion].posX * 0.001 + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 3].pos.y = g_aExplosion[nCntExplosion].posY * 0.001 + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntExplosion * EXPLOSION_VERTEX_MAX + 3].pos.z = 0.0f;

		}
	}
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffexplosion->Unlock();
	
	createExplosionCount = 0;

	//if( frameSwitch  )
	//bool frameSwitch;	/*���݂ɐ؂�ւ��A�t���[�����ς�����������ʂ���B*/
}

/*******************************************************************************
* �֐���	:void DrawExplosion( void )
*		�����̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawExplosion( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	
	

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffexplosion , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureexplosion );
	
	for( int nCntExplosion = 0; nCntExplosion < EXPLOSION_MAX; nCntExplosion++ )
	{
		if( g_aExplosion[nCntExplosion].bUse == true )
		{
			/*�����̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntExplosion * EXPLOSION_VERTEX_MAX , 2 );
		}
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DEST�����̂܂ܕ`�悷���
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	for( int nCntExplosion = 0; nCntExplosion < EXPLOSION_MAX; nCntExplosion++ )
	{
		if( g_aExplosion[nCntExplosion].bUse == true )
		{
			/*�����̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntExplosion * EXPLOSION_VERTEX_MAX , 2 );
		}
	}
	
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/
	
}

/*******************************************************************************
* �֐���	:void SetExplosion( D3DXVECTOR3 pos , D3DXVECTOR3 vecMove , int nLife , ENEMY_TYPE type )
*		�����𐶐�����֐��ł��B
* ����		:D3DXVECTOR3 pos		�����̏o���ʒu�ł��B
*			 D3DXVECTOR3 vecMove	�����̈ړ��ʂł��B
*			 int nLife				�����̎����ł��B
*			(ENEMY_TYPE type		�����̎�ނł��B)	
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void SetExplosion( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , EXPLOSION_TYPE type = EXPLOSION_001 )
{
	for( int nCntExplosion = createExplosionCount; nCntExplosion < EXPLOSION_MAX; nCntExplosion++ )
	{
		if( g_aExplosion[nCntExplosion].bUse == false )
		{
			memset( &g_aExplosion[nCntExplosion] , 0 , sizeof( EXPLOSION ) );
			g_aExplosion[nCntExplosion].posX = posX;
			g_aExplosion[nCntExplosion].posY = posY;
			g_aExplosion[nCntExplosion].posZ = posZrot;
			g_aExplosion[nCntExplosion].moveX = moveX;
			g_aExplosion[nCntExplosion].moveY = moveY;
			g_aExplosion[nCntExplosion].moveZ = moveZrot;
			g_aExplosion[nCntExplosion].nLife = nLife;
			g_aExplosion[nCntExplosion].type = type;

			g_aExplosion[nCntExplosion].attack = 1;
			g_aExplosion[nCntExplosion].radius = 100;
			g_aExplosion[nCntExplosion].bUse = true;
			createExplosionCount = nCntExplosion;	
			break;
		}
	}
}