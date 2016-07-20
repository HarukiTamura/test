/*******************************************************************************
* �^�C�g��	:���j���[�w�i.cpp
* �t�@�C����:menuBackGround.cpp
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
#include "menuBackGround.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define MENUBACKGROUND_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define MENUBACKGROUND_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define MENUBACKGROUND_ANIM_SPAN		( 10 )	/*�A�j���[�V�����Ԋu*/
#define MENUBACKGROUND_ANIM_DIV_X	( 1 )	/*��������*/
#define MENUBACKGROUND_ANIM_DIV_Y	( 1 )	/*����������*/

#define MENUBACKGROUND_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

#define USE_QUOTE				( 4 )	//�g�p�l�p�`��

#define VERTEX_HEIGHT_DEV_X	( 30 )		//���̍�
#define VERTEX_HEIGHT_DEV_Y	( 40 )		//�����̍�

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffmenuBackGround = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturemenuBackGround;			/*�\���摜�̃e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;				/*�\���摜���*/

static D3DXVECTOR3 g_movePolygon;	/*�|���S���̈ʒu*/
static D3DXVECTOR3 g_posPolygon;	/*�|���S���̈ړ���*/
static D3DXVECTOR3 g_rotPolygon;	/*�|���S���̉�]�p*/
static float g_fLengthPolygon;		/*�|���S���̑Ίp���̒���*/
static float g_fAnglePolygon;		/*�|���S���̑Ίp���̊p�x*/
static float polygonWidth , polygonHeight;	/*�|���S���̕��^�����i�S�̂̔����j*/

static float polygonMag;	/*�\���{��*/

static int g_nCounterAnim = 0;	/*�A�j���[�V�����Ԋu�J�E���g*/
static int g_nPatternAnim = 0;	/*�p�^�[�����J�E���g*/

static int outputSizeX,outputSizeY;
static float outputTexWidth;

static float scrollNum;
/*******************************************************************************
* �֐���	:HRESULT InitMenuBackGround( void )
*		���j���[�w�i�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitMenuBackGround( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * MENUBACKGROUND_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffmenuBackGround , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/menuBackGround.png" , &g_pTexturemenuBackGround );
	D3DXGetImageInfoFromFile( "data/TEXTURE/menuBackGround.png" , &info );
	
	/*�|�C���^�̃��b�N*/
	g_pVtxBuffmenuBackGround->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / MENUBACKGROUND_ANIM_DIV_X ) * MENUBACKGROUND_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUBACKGROUND_ANIM_DIV_Y ) * MENUBACKGROUND_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffmenuBackGround->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	scrollNum = 0;

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitMenuBackGround( void )
*		���j���[�w�i�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitMenuBackGround( void )
{
	if( g_pVtxBuffmenuBackGround != NULL )
	{
		g_pVtxBuffmenuBackGround->Release();
		g_pVtxBuffmenuBackGround = NULL;
	}
	if( g_pTexturemenuBackGround != NULL )
	{
		g_pTexturemenuBackGround->Release();
		g_pTexturemenuBackGround = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateMenuBackGround( void )
*		���j���[�w�i�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateMenuBackGround( void )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffmenuBackGround->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / MENUBACKGROUND_ANIM_DIV_X ) * MENUBACKGROUND_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUBACKGROUND_ANIM_DIV_Y ) * MENUBACKGROUND_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	texWidth = 1.0f / MENUBACKGROUND_ANIM_DIV_X;
	texHeight = 1.0f / MENUBACKGROUND_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % MENUBACKGROUND_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / MENUBACKGROUND_ANIM_DIV_X ) ) + 0;

	/*�w�i�X�N���[���p�ϐ�*/
	scrollNum = ( scrollNum + 0.005f == 0.0f ? 1.0f : scrollNum + 0.005f );

	for( quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		/*�e�N�X�`�����W����*/
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.x = 0.0f;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.y = 0.0f + scrollNum;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.x = 0.5f;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.y = 0.0f + scrollNum;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.x = 0.0;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.y = 2.0f + scrollNum;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.x = 0.5f;
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.y = 2.0f + scrollNum;
		if( quoteCnt == 0 || quoteCnt == 3 )
		{
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.x = 0.0f;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.y = 0.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.x = 0.5f;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.y = 0.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.x = 0.0;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.y = 2.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.x = 0.5f;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.y = 2.0f + scrollNum;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].tex.y	 += 0.603;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].tex.y += 0.603;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].tex.y += 1.017;
			pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].tex.y += 1.017;
		}

		
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 255 );
		pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 0 , 0 , 0 , 255 );
	}


	pVtx[0].pos = D3DXVECTOR3( 0 - VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_WIDTH / 4	- VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( 0 , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	- VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT	+ VERTEX_HEIGHT_DEV_Y , 0.0f );

	pVtx[4].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	- VERTEX_HEIGHT_DEV_X , 0.0f , 0.0f );
	pVtx[5].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4 * 2	, 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[6].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	- VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[7].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	* 2	, SCREEN_HEIGHT	, 0.0f );

	pVtx[8].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4	* 2	 , 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[9].pos = D3DXVECTOR3( SCREEN_WIDTH	/ 4 * 3	+ VERTEX_HEIGHT_DEV_X , 0.0f , 0.0f );
	pVtx[10].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 2 , SCREEN_HEIGHT , 0.0f );
	pVtx[11].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 3 + VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y , 0.0f );

	pVtx[12].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 3 + VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );
	pVtx[13].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 4 + VERTEX_HEIGHT_DEV_X , 0.0f - VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[14].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 3 + VERTEX_HEIGHT_DEV_X , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y , 0.0f );
	pVtx[15].pos = D3DXVECTOR3( SCREEN_WIDTH / 4 * 4 , SCREEN_HEIGHT + VERTEX_HEIGHT_DEV_Y * 3 , 0.0f );

	pVtx[0].pos.x -= VERTEX_HEIGHT_DEV_X;
	pVtx[13].pos.x += VERTEX_HEIGHT_DEV_X;
	
	pVtx[1].pos.x -= VERTEX_HEIGHT_DEV_X;
	pVtx[4].pos.x -= VERTEX_HEIGHT_DEV_X;
	
	pVtx[9].pos.x += VERTEX_HEIGHT_DEV_X;
	pVtx[12].pos.x += VERTEX_HEIGHT_DEV_X;

	/*
	pVtx[3].pos.x += VERTEX_HEIGHT_DEV_X;
	pVtx[6].pos.x += VERTEX_HEIGHT_DEV_X;
	
	pVtx[11].pos.x -= VERTEX_HEIGHT_DEV_X;
	pVtx[14].pos.x -= VERTEX_HEIGHT_DEV_X;
	*/
	/*
	g_posPolygon.x = OUTPUT_LOGO_POSX;
	g_posPolygon.y = 300;
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUBACKGROUND_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	*/

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffmenuBackGround->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawMenuBackGround( void )
*		���j���[�w�i�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawMenuBackGround( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffmenuBackGround , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturemenuBackGround );
		
	/*���j���[�w�i�̕`��*/
	
	for( int quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		/*�e�N�X�`�����W����*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , MENUBACKGROUND_VERTEX_MAX * quoteCnt , 2 );
	}

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DEST�����̂܂ܕ`�悷���
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	

	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/
	//pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}