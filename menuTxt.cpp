/*******************************************************************************
* �^�C�g��	:���j���[�e�L�X�g.cpp
* �t�@�C����:menuTxt.cpp
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
#include "menu.h"
#include "menuTxt.h"
#include "input.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define MENUTXT_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define OUTPUT_LOGO_POSX	( SCREEN_WIDTH / 2 )
#define OUTPUT_LOGO_POSY	( 0 )

#define LOGO_WAVE	( 20 )	//���S�̗h���͈�
#define COLOR_WAVE	( 10 )	//�F�̗h���͈�

#define WAVELOGO_NUM			( 600 )

#define MENUTXT_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define MENUTXT_ANIM_SPAN		( 10 )	/*�A�j���[�V�����Ԋu*/
#define MENUTXT_ANIM_DIV_X	( 1 )	/*��������*/
#define MENUTXT_ANIM_DIV_Y	( WAVELOGO_NUM )	/*����������*/

#define MENUTXT_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

#define USE_QUOTE				( WAVELOGO_NUM + 1 )	//�g�p�l�p�`��

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffmenuTxt = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturemenuTxt;			/*�\���摜�̃e�N�X�`���̃|�C���^*/
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
static float g_TxtPosXPlus;		//�e�L�X�g�X���C�h�C���p
static float g_TxtPosXMinus;	//�e�L�X�g�X���C�h�p
/*******************************************************************************
* �֐���	:HRESULT InitMenuTxt( void )
*		���j���[�e�L�X�g�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitMenuTxt( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * MENUTXT_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffmenuTxt , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/menuTxt.png" , &g_pTexturemenuTxt );
	D3DXGetImageInfoFromFile( "data/TEXTURE/menuTxt.png" , &info );
	
	/*�|�C���^�̃��b�N*/
	g_pVtxBuffmenuTxt->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / MENUTXT_ANIM_DIV_X ) * MENUTXT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUTXT_ANIM_DIV_Y ) * MENUTXT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffmenuTxt->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	outputTexWidth = 1.0f / ( float )( USE_QUOTE );
	g_TxtPosXPlus = 1.0f;
	g_TxtPosXMinus = 0.0f;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitMenuTxt( void )
*		���j���[�e�L�X�g�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitMenuTxt( void )
{
	if( g_pVtxBuffmenuTxt != NULL )
	{
		g_pVtxBuffmenuTxt->Release();
		g_pVtxBuffmenuTxt = NULL;
	}
	if( g_pTexturemenuTxt != NULL )
	{
		g_pTexturemenuTxt->Release();
		g_pTexturemenuTxt = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateMenuTxt( int g_menuCommand )
*		���j���[�e�L�X�g�̍X�V�����֐��ł��B
* ����		:int g_menuCommand		���ݑI�����Ă���R�}���h�̔ԍ��ł��B
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateMenuTxt( int g_menuCommand )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	float TxtPosXPlus = 0.0f;
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;
	int colRed = 0, colGreen = 0, colBlue = 0;
	int posPolygonY = 0;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffmenuTxt->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	g_TxtPosXPlus = ( g_TxtPosXPlus - 0.05f >= 0.0f ? g_TxtPosXPlus - 0.05f : 0 ); 
	if( g_TxtPosXPlus == 0 )
	{
		g_TxtPosXMinus = ( g_TxtPosXMinus + 0.012f >= 0.5f ? 0.5f : g_TxtPosXMinus + 0.012f ); 
	}
	

	if( GetAnykeyPress() )
	{
		g_TxtPosXPlus = 0.0f;
		g_TxtPosXMinus = 0.5f;
	}
	outputSizeX = ( int )( ( info.Width / MENUTXT_ANIM_DIV_X ) * MENUTXT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUTXT_ANIM_DIV_Y ) * MENUTXT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	texWidth = 1.0f / MENUTXT_ANIM_DIV_X;
	texHeight = 1.0f / MENUTXT_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % MENUTXT_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / MENUTXT_ANIM_DIV_X ) ) + 0;

	for( quoteCnt = 0; quoteCnt < WAVELOGO_NUM; quoteCnt++ )
	{
		/*��b�J���[�ݒ�*/
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 100 , 55 - rand() % COLOR_WAVE , 55 - rand() % COLOR_WAVE , 155 );

		/*���_���W�ݒ�*/
		if( quoteCnt == 0 )
		{
			g_posPolygon.x = OUTPUT_LOGO_POSX;
			g_posPolygon.y = OUTPUT_LOGO_POSY;
		}
		else
		{	
			g_posPolygon.y += outputSizeY;
			if( g_posPolygon.y >= SCREEN_HEIGHT / MENU_MAX * g_menuCommand &&  g_posPolygon.y <= SCREEN_HEIGHT / MENU_MAX * g_menuCommand + ( SCREEN_HEIGHT / MENU_MAX ) )
			{
				//�I�𒆃R�}���h�͗h�炳�Ȃ��B
				g_posPolygon.x = OUTPUT_LOGO_POSX;
				//�I�𒆃R�}���h��p�J���[
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX].col		= D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
				pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 225 );
			}
			else
			{
				g_posPolygon.x = OUTPUT_LOGO_POSX + rand() % LOGO_WAVE - rand() % LOGO_WAVE;
			}
		}
		
		/*�e�N�X�`�����W����*/
		/*PleasePushAnykey*/
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.x = 0.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.y = outputTexWidth * quoteCnt;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.x = 1.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.y = outputTexWidth * quoteCnt + outputTexWidth;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.x = 0.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.y = outputTexWidth * quoteCnt;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.x = 1.0f;
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.y = outputTexWidth * quoteCnt + outputTexWidth;

		g_posPolygon.x -= ( SCREEN_WIDTH * 0.25f ) * sin( D3DX_PI * g_TxtPosXMinus );
		g_posPolygon.y = - 10 + SCREEN_HEIGHT * sin( D3DX_PI * g_TxtPosXPlus );
		

		pVtx[quoteCnt * MENUTXT_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
		
		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

		pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
									g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	}

	
	outputSizeX = ( int )( ( info.Width / MENUTXT_ANIM_DIV_X ) * MENUTXT_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / 1 ) * MENUTXT_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );

	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.x = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX].tex.y = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.x = 1.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].tex.y = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.x = 0.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].tex.y = 1.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.x = 1.0f;
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].tex.y = 1.0f;
	

	/*
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 0].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].col = D3DCOLOR_RGBA( 255, 255 - rand() % COLOR_WAVE , 255 - rand() % COLOR_WAVE , 155 );
	*/

	/*���_���W�ݒ�*/
	g_posPolygon.x = OUTPUT_LOGO_POSX - ( SCREEN_WIDTH * 0.25f ) * sin( D3DX_PI * g_TxtPosXMinus );
	g_posPolygon.y = 300;
	g_posPolygon.y *= cos( D3DX_PI * g_TxtPosXPlus );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[quoteCnt * MENUTXT_VERTEX_MAX + 3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffmenuTxt->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawMenuTxt( void )
*		���j���[�e�L�X�g�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawMenuTxt( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffmenuTxt , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturemenuTxt );
	
	for( int quoteCnt = 0; quoteCnt < USE_QUOTE; quoteCnt++ )
	{
		if( quoteCnt >= WAVELOGO_NUM )
		{
			GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
			GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// DEST�����̂܂ܕ`�悷���
			GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}
	
		/*���j���[�e�L�X�g�̕`��*/
		pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , quoteCnt * MENUTXT_VERTEX_MAX , 2 );
	}
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// SRC�𔼓������������
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );	/*�A���t�@�u�����h�L����*/
	GetDevice()->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );		/*�X�N���[�����u�����h�ݒ�*/
	GetDevice()->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	/*�|���S�����u�����h�ݒ�*/
	//pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}