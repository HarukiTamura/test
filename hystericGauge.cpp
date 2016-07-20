/*******************************************************************************
* �^�C�g��	:�q�X�e���b�N�Q�[�W.cpp
* �t�@�C����:hystericGauge.cpp
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
#include "hystericGauge.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define HYSTERICGAUGE_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define OUTPUT_GAUGE_POSX	( SCREEN_RIGHT - 206 )
#define OUTPUT_GAUGE_POSY	( 340 )

#define HYSTERICGAUGE_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define HYSTERICGAUGE_ANIM_SPAN		( 0 )	/*�A�j���[�V�����Ԋu*/
#define HYSTERICGAUGE_ANIM_DIV_X	( 5 )	/*��������*/
#define HYSTERICGAUGE_ANIM_DIV_Y	( 1 )	/*����������*/

#define HYSTERICGAUGE_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

#define USE_HYSTERIC_POINT		( 60 )		//�g�p����Q�[�W�̃|�C���g��
#define HYSTERIC_POINT_SPACE	( 3 )		//�g�p����Q�[�W�̃|�C���g�Ԋu
#define USE_QUOTE				( 4 + USE_HYSTERIC_POINT - 1 )	//�g�p�l�p�`��

typedef enum
{
	CHARACTER_NULL = -1,
	CHARACTER_NORMAL,
	CHARACTER_DAMAGE,
	CHARACTER_HYPERMODE,
	CHARACTER_MODEMAX
}CHARACTER_MODE;
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffhystericGauge = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturehystericGauge;	/*�q�X�e���b�N�Q�[�W�̃e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;	/*�A�j���[�V�����Ԋu�J�E���g*/
static int g_nPatternAnim = 0;	/*�p�^�[�����J�E���g*/

static int hystericPoint[USE_HYSTERIC_POINT];
static int outputSizeX,outputSizeY;
/*******************************************************************************
* �֐���	:HRESULT InitHystericGauge( void )
*		�q�X�e���b�N�Q�[�W�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitHystericGauge( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * HYSTERICGAUGE_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffhystericGauge , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/hystericGauge.png" , &g_pTexturehystericGauge );
	D3DXGetImageInfoFromFile( "data/TEXTURE/hystericGauge.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffhystericGauge->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / HYSTERICGAUGE_ANIM_DIV_X ) * HYSTERICGAUGE_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / HYSTERICGAUGE_ANIM_DIV_Y ) * HYSTERICGAUGE_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffhystericGauge->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	//�n�C�p�[���[�h�ێ��p�G�l���M�[
	memset( &hystericPoint[0] , 0 , sizeof( int ) * USE_HYSTERIC_POINT );
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitHystericGauge( void )
*		�q�X�e���b�N�Q�[�W�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitHystericGauge( void )
{
	if( g_pVtxBuffhystericGauge != NULL )
	{
		g_pVtxBuffhystericGauge->Release();
		g_pVtxBuffhystericGauge = NULL;
	}
	if( g_pTexturehystericGauge != NULL )
	{
		g_pTexturehystericGauge->Release();
		g_pTexturehystericGauge = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateHystericGauge( void )
*		�q�X�e���b�N�Q�[�W�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateHystericGauge( void )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffhystericGauge->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

	int pointCnt = USE_HYSTERIC_POINT - 1;
	while( pointCnt-- )//for( int pointCnt = 0; pointCnt < USE_HYSTERIC_POINT - 1; pointCnt++ )
	{
		hystericPoint[pointCnt+1] = hystericPoint[pointCnt];
	}
	hystericPoint[0] = GetPlayerState01()->liarNum;
				
	texWidth = 1.0f / HYSTERICGAUGE_ANIM_DIV_X;
	texHeight = 1.0f / HYSTERICGAUGE_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % HYSTERICGAUGE_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / HYSTERICGAUGE_ANIM_DIV_X ) ) + 0;

	/*�e�N�X�`�����W����*/
	/*�Q�[�W��b�i�E�j*/
	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 0.0f;
	pVtx[1].tex.x = 0.2f;
	pVtx[1].tex.y = 0.0f;
	pVtx[2].tex.x = 0.0f;
	pVtx[2].tex.y = 1.0f;
	pVtx[3].tex.x = 0.2f;
	pVtx[3].tex.y = 1.0f;

	/*���ݒl���C��*/
	pVtx[4].tex.x = 0.2f;
	pVtx[4].tex.y = 0.0f;
	pVtx[5].tex.x = 0.4f;
	pVtx[5].tex.y = 0.0f;
	pVtx[6].tex.x = 0.2f;
	pVtx[6].tex.y = 1.0f;
	pVtx[7].tex.x = 0.4f;
	pVtx[7].tex.y = 1.0f;
	
	for( quoteCnt = 2; quoteCnt < USE_QUOTE - 2; quoteCnt++ )
	{
		/*�|�C���g*/
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.x = 0.4f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.y = 0.0f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.x = 0.6f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.y = 0.0f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.x = 0.4f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.y = 1.0f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.x = 0.6f;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.y = 1.0f;
	}
	
	/*�Q�[�W����*/
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.x = 0.6f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.x = 0.6f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].tex.y = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].tex.y = 1.0f;
	
	/*�Q�[�W�g*/
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 4].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 4].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 5].tex.x = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 5].tex.y = 0.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 6].tex.x = 0.8f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 6].tex.y = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 7].tex.x = 1.0f;
	pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 7].tex.y = 1.0f;
	
	for( quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

		/*���_���W�ݒ�*/
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.x = SCREEN_WIDTH - outputSizeX;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.y = OUTPUT_GAUGE_POSY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.z = 0.0f;
		
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.x = SCREEN_WIDTH;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.y = OUTPUT_GAUGE_POSY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.z = 0.0f;
		
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.x = SCREEN_WIDTH - outputSizeX;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.y = OUTPUT_GAUGE_POSY + outputSizeY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.z = 0.0f;

		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.x = SCREEN_WIDTH;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.y = OUTPUT_GAUGE_POSY + outputSizeY;
		pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.z = 0.0f;
		
		
		
		if( quoteCnt >= 1 && quoteCnt <= USE_QUOTE - 2 )
		{
			if( quoteCnt < USE_QUOTE - 2 )
			{
				hystericPer = ( float )( hystericPoint[quoteCnt - 1] ) / ( float )( LIAR_MAX );
				
				outputPointPos = outputSizeHalf * hystericPer;

				if( hystericPoint[quoteCnt - 1] > 0 )
				{
					outputPointColor = 255 - ( int )( 255 * hystericPer );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
				}
				else
				{				
					outputPointColor = 255 + ( int )( 255 * hystericPer );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].col = D3DCOLOR_RGBA( 255 , outputPointColor , outputPointColor , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( outputPointColor , outputPointColor , 255 , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( outputPointColor , outputPointColor , 255 , 255 );
					pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( outputPointColor , outputPointColor , 255 , 255 );
				}

				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.y		-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.y	-= outputPointPos;

				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.x		-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.x	-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.x	-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.x	-= ( quoteCnt - 1 ) * HYSTERIC_POINT_SPACE;
			}
			else
			{
				hystericPer = ( float )( hystericPoint[0] ) / ( float )( LIAR_MAX );
				outputPointPos = outputSizeHalf * hystericPer;

				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX].pos.y		-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 1].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 2].pos.y	-= outputPointPos;
				pVtx[quoteCnt * HYSTERICGAUGE_VERTEX_MAX + 3].pos.y	-= outputPointPos;
			}
		}	
	}		

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffhystericGauge->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawHystericGauge( void )
*		�q�X�e���b�N�Q�[�W�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawHystericGauge( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffhystericGauge , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturehystericGauge );
	
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DEST�����̂܂ܕ`�悷���
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	for( int quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		/*�q�X�e���b�N�Q�[�W�̂̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , quoteCnt * HYSTERICGAUGE_VERTEX_MAX , 2 );
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/
}