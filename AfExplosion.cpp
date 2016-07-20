/*******************************************************************************
* �^�C�g��	:����.cpp
* �t�@�C����:AfAfExplosion.cpp
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
#include "AfExplosion.h"
#include "input.h"
#include "unoHitChecker.h"
#include "unoDirectX.h"
#include "barrier.h"
#include "player01.h"
#include "enemy.h"
#include "AfEnemy.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define AF_EXPLOSION_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define AF_EXPLOSION_ANIM_MAX		( 4 )	/*�A�j���[�V�����R�}��*/
#define AF_EXPLOSION_ANIM_SPAN	( 20 )	/*�A�j���[�V�����Ԋu*/
#define AF_EXPLOSION_ANIM_DIV_X	( 4 )	/*��������*/
#define AF_EXPLOSION_ANIM_DIV_Y	( 3 )	/*����������*/

#define AF_EXPLOSION_OUTPUT_SIZE	( 0.2 ) /*�����\���{��*/ 

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
}AF_EXPLOSION;

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffAfAfExplosion = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureAfAfExplosion;	/*�����̃e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static AF_EXPLOSION g_aAfExplosion[AF_EXPLOSION_MAX];	/*�����̍\����*/
static bool frameSwitch = false;	/*���݂ɐ؂�ւ��A�t���[�����ς�����������ʂ���B*/

static int createAfExplosionCount = 0;	/*�������J�E���g*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;	/*�A�j���[�V�����Ԋu�J�E���g*/
static int g_nPatternAnim = 0;	/*�p�^�[�����J�E���g*/

static int lastAfExplosion = 0;	/*���������݂����Ō�̕���*/

static bool g_soundOn[AF_EXPLOSION_TYPE_MAX];
/*******************************************************************************
* �֐���	:HRESULT InitAfExplosion( void )
*		�����̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfExplosion( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_EXPLOSION_VERTEX_MAX * AF_EXPLOSION_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffAfAfExplosion , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfExplosion.png" , &g_pTextureAfAfExplosion );
	D3DXGetImageInfoFromFile( "data/TEXTURE/AfExplosion.png" , &info );

	ZeroMemory( &g_aAfExplosion[0] , sizeof( AF_EXPLOSION ) * AF_EXPLOSION_MAX );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffAfAfExplosion->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	int outputSizeX,outputSizeY;
	outputSizeX = ( int )( ( info.Width / AF_EXPLOSION_ANIM_DIV_X ) * AF_EXPLOSION_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / AF_EXPLOSION_ANIM_DIV_Y ) * AF_EXPLOSION_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , AF_EXPLOSION_MAX );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfAfExplosion->Unlock();

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
	
	createAfExplosionCount = 0;
	
	for( int cnt = 0; cnt < AF_EXPLOSION_TYPE_MAX; cnt++ )
	{
		g_soundOn[cnt] = false;
	}

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfExplosion( void )
*		�����̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfExplosion( void )
{
	if( g_pVtxBuffAfAfExplosion != NULL )
	{
		g_pVtxBuffAfAfExplosion->Release();
		g_pVtxBuffAfAfExplosion = NULL;
	}
	if( g_pTextureAfAfExplosion != NULL )
	{
		g_pTextureAfAfExplosion->Release();
		g_pTextureAfAfExplosion = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfExplosion( void )
*		�����̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfExplosion( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	for( int cnt = 0; cnt < AF_EXPLOSION_TYPE_MAX; cnt++ )
	{
		g_soundOn[cnt] = false;
	}

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffAfAfExplosion->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*�����̍X�V*/
	for( int nCntAfExplosion = 0; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == true )
		{
				
			g_aAfExplosion[nCntAfExplosion].nLife--;
			//�����`�F�b�N
			if( g_aAfExplosion[nCntAfExplosion].nLife <= 0 )
			{
				g_aAfExplosion[nCntAfExplosion].bUse = false;
				continue;
			}
			
			g_aAfExplosion[nCntAfExplosion].posX += g_aAfExplosion[nCntAfExplosion].moveX;
			g_aAfExplosion[nCntAfExplosion].posY += g_aAfExplosion[nCntAfExplosion].moveY;


			/*�p�^�[���A�j���[�V�������O�v�Z����*/
			if( ++g_nCounterAnim %= AF_EXPLOSION_ANIM_SPAN )
			{
				++g_nPatternAnim %= AF_EXPLOSION_ANIM_MAX;
				//g_nPatternAnim = ( ++g_nPatternAnim == AF_EXPLOSION_ANIM_MAX ? 0 : g_nPatternAnim ); 
			}
			texWidth = 1.0f / AF_EXPLOSION_ANIM_DIV_X;
			texHeight = 1.0f / AF_EXPLOSION_ANIM_DIV_Y;
			nowPatternX = ( float )( g_nPatternAnim % AF_EXPLOSION_ANIM_DIV_X );
			

			nowPatternY = ( float )( ( int )( g_nPatternAnim / AF_EXPLOSION_ANIM_DIV_X ) ) + g_aAfExplosion[nCntAfExplosion].type;

			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 122 );
			
			
			/*�e�N�X�`�����W����*/
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].tex.x = texWidth * nowPatternX;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].tex.y = texHeight * nowPatternY;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].tex.y = texHeight * nowPatternY;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].tex.x = texWidth * nowPatternX;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].tex.y = texHeight * nowPatternY + texHeight;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].tex.x = texWidth * nowPatternX + texWidth;
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].tex.y = texHeight * nowPatternY + texHeight;
			
			g_aAfExplosion[nCntAfExplosion].posZ += g_aAfExplosion[nCntAfExplosion].moveZ;
			if( g_aAfExplosion[nCntAfExplosion].type == AF_EXPLOSION_002 )
			{
				float bonbSize = ( float )( g_aAfExplosion[nCntAfExplosion].posZ ) * 0.015;
				if( bonbSize <= 0 )
				{
					bonbSize = 1.0f;

					g_rotPolygon.z =  g_rotPolygon.z + ( rand() % 100 - rand() % 100 ) * 0.01;  
				}
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
				polygonMag = ( float ) bonbSize;
				polygonMag *= 1.4;
			
			}
			else if( g_aAfExplosion[nCntAfExplosion].type == AF_EXPLOSION_003 )
			{
				
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
				pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 155 );
			}
			else
			{
				polygonMag *= 1.4;
			}
			polygonMag *= 0.76;


			/*���_���W�ݒ�*/
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 0].pos.z = 0.0f;
											      
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 1].pos.z = 0.0f;
											      
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 2].pos.z = 0.0f;
											      
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].pos.x = g_aAfExplosion[nCntAfExplosion].posX * 0.001 + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].pos.y = g_aAfExplosion[nCntAfExplosion].posY * 0.001 + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag );
			pVtx[nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX + 3].pos.z = 0.0f;

		}
	}
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffAfAfExplosion->Unlock();
	
	createAfExplosionCount = 0;

	//if( frameSwitch  )
	//bool frameSwitch;	/*���݂ɐ؂�ւ��A�t���[�����ς�����������ʂ���B*/
}

/*******************************************************************************
* �֐���	:void DrawAfExplosion( void )
*		�����̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfExplosion( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	
	

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffAfAfExplosion , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureAfAfExplosion );
	
	for( int nCntAfExplosion = 0; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == true )
		{
			/*�����̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX , 2 );
		}
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DEST�����̂܂ܕ`�悷���
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	for( int nCntAfExplosion = 0; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == true )
		{
			/*�����̂̕`��*/
			pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , nCntAfExplosion * AF_EXPLOSION_VERTEX_MAX , 2 );
		}
	}
	
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/
	
}

/*******************************************************************************
* �֐���	:void SetAfExplosion( D3DXVECTOR3 pos , D3DXVECTOR3 vecMove , int nLife , ENEMY_TYPE type )
*		�����𐶐�����֐��ł��B
* ����		:D3DXVECTOR3 pos		�����̏o���ʒu�ł��B
*			 D3DXVECTOR3 vecMove	�����̈ړ��ʂł��B
*			 int nLife				�����̎����ł��B
*			(ENEMY_TYPE type		�����̎�ނł��B)	
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void SetAfExplosion( int posX , int posY , int posZrot , int moveX , int moveY , int moveZrot , int nLife , AF_EXPLOSION_TYPE type = AF_EXPLOSION_001 )
{
	for( int nCntAfExplosion = createAfExplosionCount; nCntAfExplosion < AF_EXPLOSION_MAX; nCntAfExplosion++ )
	{
		if( g_aAfExplosion[nCntAfExplosion].bUse == false )
		{
			memset( &g_aAfExplosion[nCntAfExplosion] , 0 , sizeof( AF_EXPLOSION ) );
			g_aAfExplosion[nCntAfExplosion].posX = posX;
			g_aAfExplosion[nCntAfExplosion].posY = posY;
			g_aAfExplosion[nCntAfExplosion].posZ = posZrot;
			g_aAfExplosion[nCntAfExplosion].moveX = moveX;
			g_aAfExplosion[nCntAfExplosion].moveY = moveY;
			g_aAfExplosion[nCntAfExplosion].moveZ = moveZrot;
			g_aAfExplosion[nCntAfExplosion].nLife = nLife;
			g_aAfExplosion[nCntAfExplosion].type = type;
			switch( type )
			{
				case AF_EXPLOSION_001:
					if( g_soundOn[AF_EXPLOSION_001] == false )
					{
						PlaySound(SOUND_LABEL_SE_MISSILESHOOT);
						g_soundOn[AF_EXPLOSION_001] = true;
					}
					break;
				case AF_EXPLOSION_002:
					if( g_soundOn[AF_EXPLOSION_002] == false )
					{
						//������̂Ŏg�p���Ȃ�
						//PlaySound(SOUND_LABEL_SE_MISSILEFLY);
						g_soundOn[AF_EXPLOSION_002] = true;
					}
					//
					break;
				default:
					break;
			}
			g_aAfExplosion[nCntAfExplosion].attack = 1;
			g_aAfExplosion[nCntAfExplosion].radius = 100;
			g_aAfExplosion[nCntAfExplosion].bUse = true;
			createAfExplosionCount = nCntAfExplosion;	
			break;
		}
	}
}