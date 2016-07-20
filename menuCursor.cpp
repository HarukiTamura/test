/*******************************************************************************
* �^�C�g��	:���j���[�J�[�\��.cpp
* �t�@�C����:menuCursor.cpp
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
#include "menuCursor.h"
#include "input.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define MENUCURSOR_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define CURSOR_SWING	( 20 )	/*�J�[�\���h�ꕝ*/

#define OUTPUT_CURSOR_POSX	( SCREEN_WIDTH / 2 - SCREEN_WIDTH / 4 )
#define OUTPUT_CURSOR_POSY	( SCREEN_HEIGHT / 2 )

#define MENUCURSOR_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define MENUCURSOR_ANIM_SPAN		( 10 )	/*�A�j���[�V�����Ԋu*/
#define MENUCURSOR_ANIM_DIV_X	( 1 )	/*��������*/
#define MENUCURSOR_ANIM_DIV_Y	( 1 )	/*����������*/

#define MENUCURSOR_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

#define USE_QUOTE				( 1 )	//�g�p�l�p�`��

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffmenuCursor = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturemenuCursor;			/*�\���摜�̃e�N�X�`���̃|�C���^*/
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
static int g_pastCommand;
static float g_nowCursorPosX;
static float g_CursorPosXPlus;	//���j���[�˓����̃X���C�h�C���p
/*******************************************************************************
* �֐���	:HRESULT InitMenuCursor( void )
*		���j���[�J�[�\���̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitMenuCursor( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * MENUCURSOR_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffmenuCursor , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/menuCursor.png" , &g_pTexturemenuCursor );
	D3DXGetImageInfoFromFile( "data/TEXTURE/menuCursor.png" , &info );
	
	/*�|�C���^�̃��b�N*/
	g_pVtxBuffmenuCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / MENUCURSOR_ANIM_DIV_X ) * MENUCURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUCURSOR_ANIM_DIV_Y ) * MENUCURSOR_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffmenuCursor->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	outputTexWidth = 1.0f / ( float )( USE_QUOTE );

	g_pastCommand = 0;
	g_nowCursorPosX = 0;
	g_CursorPosXPlus = 600;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitMenuCursor( void )
*		���j���[�J�[�\���̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitMenuCursor( void )
{
	if( g_pVtxBuffmenuCursor != NULL )
	{
		g_pVtxBuffmenuCursor->Release();
		g_pVtxBuffmenuCursor = NULL;
	}
	if( g_pTexturemenuCursor != NULL )
	{
		g_pTexturemenuCursor->Release();
		g_pTexturemenuCursor = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateMenuCursor( int g_menuCommand )
*		���j���[�J�[�\���̍X�V�����֐��ł��B
* ����		:int g_menuCommand		���ݑI�����Ă���R�}���h�̔ԍ��ł��B
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateMenuCursor( int g_menuCommand )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	g_CursorPosXPlus = ( g_CursorPosXPlus - 25.0f >= 0 ? g_CursorPosXPlus - 25.0f : 0 );
	if( g_pastCommand != g_menuCommand )
	{
		g_nowCursorPosX = 0;
	}
	else
	{
		g_nowCursorPosX = ( g_nowCursorPosX + 0.025f > 1.0f ? 0.0f : g_nowCursorPosX + 0.025f );
	}

	
	/*�|�C���^�̃��b�N*/ 
	g_pVtxBuffmenuCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	
	outputSizeX = ( int )( ( info.Width / MENUCURSOR_ANIM_DIV_X ) * MENUCURSOR_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / MENUCURSOR_ANIM_DIV_Y ) * MENUCURSOR_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );

	texWidth = 1.0f / MENUCURSOR_ANIM_DIV_X;
	texHeight = 1.0f / MENUCURSOR_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % MENUCURSOR_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / MENUCURSOR_ANIM_DIV_X ) ) + 0;

	/*�e�N�X�`�����W����*/
	/*PleasePushAnykey*/
	pVtx[0].tex.x = 0.0f;
	pVtx[0].tex.y = 0.0f;
	pVtx[1].tex.x = 1.0f;
	pVtx[1].tex.y = 0.0f;
	pVtx[2].tex.x = 0.0f;
	pVtx[2].tex.y = 1.0f;
	pVtx[3].tex.x = 1.0f;
	pVtx[3].tex.y = 1.0f;
		
	pVtx[0].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );
	pVtx[1].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );
	pVtx[2].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );
	pVtx[3].col = D3DCOLOR_RGBA( 255, 255 , 255 , 158 );

	g_posPolygon.x = OUTPUT_CURSOR_POSX + CURSOR_SWING * sin( 2 * D3DX_PI * g_nowCursorPosX ) - ( 2 * CURSOR_SWING * sin( 2 * D3DX_PI * g_nowCursorPosX ) ) + g_CursorPosXPlus;
	g_posPolygon.y = SCREEN_HEIGHT / MENU_MAX * g_menuCommand + ( SCREEN_HEIGHT / MENU_MAX / 2 );
	
	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffmenuCursor->Unlock();

	
	g_pastCommand = g_menuCommand;

}

/*******************************************************************************
* �֐���	:void DrawMenuCursor( void )
*		���j���[�J�[�\���̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawMenuCursor( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffmenuCursor , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturemenuCursor );
	

	/*���j���[�J�[�\���̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}