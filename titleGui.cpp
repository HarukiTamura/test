/*******************************************************************************
* �^�C�g��	:�^�C�g���C���^�[�t�F�[�X.cpp
* �t�@�C����:titleGui.cpp
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
#include "titleGui.h"
#include "input.h"
#include "player01.h"
#include "unoDirectX.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/

#define TITLEGUI_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define OUTPUT_GUI_POSX	( 300 )
#define OUTPUT_GUI_POSY	( 530 )

#define TITLEGUI_ANIM_MAX		( 1 )	/*�A�j���[�V�����R�}��*/
#define TITLEGUI_ANIM_SPAN		( 10 )	/*�A�j���[�V�����Ԋu*/
#define TITLEGUI_ANIM_DIV_X	( 1 )	/*��������*/
#define TITLEGUI_ANIM_DIV_Y	( 1 )	/*����������*/

#define TITLEGUI_OUTPUT_SIZE	( 1 ) /*�����\���{��*/ 

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
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBufftitleGui = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTexturetitleGui;			/*�\���摜�̃e�N�X�`���̃|�C���^*/
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
/*******************************************************************************
* �֐���	:HRESULT InitTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�̏������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitTitleGui( void )
{
	g_nCounterAnim = 0;
	g_nPatternAnim = 0;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * TITLEGUI_VERTEX_MAX * USE_QUOTE , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBufftitleGui , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/titleGui.png" , &g_pTexturetitleGui );
	D3DXGetImageInfoFromFile( "data/TEXTURE/titleGui.png" , &info );
	
	/*�|�C���^�̃��b�N*/
	g_pVtxBufftitleGui->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	outputSizeX = ( int )( ( info.Width / TITLEGUI_ANIM_DIV_X ) * TITLEGUI_OUTPUT_SIZE );
	outputSizeY = ( int )( ( info.Height / TITLEGUI_ANIM_DIV_Y ) * TITLEGUI_OUTPUT_SIZE );

	polygonWidth = ( float )( outputSizeX );
	polygonHeight = ( float )( outputSizeY );
	
	vertexInitQuad( pVtx , USE_QUOTE );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBufftitleGui->Unlock();

	/*��]�p�X�e�[�^�X�ݒ�*/	
	g_rotPolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	g_fLengthPolygon = sqrtf( polygonWidth / 2 * polygonWidth / 2 + polygonHeight / 2 * polygonHeight / 2 );
	g_fAnglePolygon = atan2f( polygonWidth / 2 , polygonHeight / 2 );
	
	/*�g��p�X�e�[�^�X*/
	polygonMag = 1;

	/*�����x����p�X�e�[�^�X�ݒ�*/
	g_movePolygon = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�̏I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitTitleGui( void )
{
	if( g_pVtxBufftitleGui != NULL )
	{
		g_pVtxBufftitleGui->Release();
		g_pVtxBufftitleGui = NULL;
	}
	if( g_pTexturetitleGui != NULL )
	{
		g_pTexturetitleGui->Release();
		g_pTexturetitleGui = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�̍X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateTitleGui( void )
{
	int quoteCnt, hystericPointBuffer;
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	float texWidth , texHeight , nowPatternX , nowPatternY;	/*�A�j���[�V�����p�o�b�t�@*/
	
	float hystericPer;
	int outputSizeHalf = outputSizeY * 0.5;
	int outputPointPos = 0, outputPointColor = 0;

	/*�|�C���^�̃��b�N*/ 
	g_pVtxBufftitleGui->Lock( 0 , 0 , ( void** ) &pVtx , 0 );

				
	texWidth = 1.0f / TITLEGUI_ANIM_DIV_X;
	texHeight = 1.0f / TITLEGUI_ANIM_DIV_Y;
	nowPatternX = ( float )( g_nPatternAnim % TITLEGUI_ANIM_DIV_X );
	nowPatternY = ( float )( ( int )( g_nPatternAnim / TITLEGUI_ANIM_DIV_X ) ) + 0;

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
	
	pVtx[0].col = D3DCOLOR_RGBA( 155 , 155 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 155 , 155 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 155 , 155 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 155 , 155 , 255 , 255 );

	/*���_���W�ݒ�*/
	g_posPolygon.x = OUTPUT_GUI_POSX;
	g_posPolygon.y = OUTPUT_GUI_POSY;
	pVtx[0].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
							g_posPolygon.y + -cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	pVtx[1].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + -cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[2].pos = D3DXVECTOR3( g_posPolygon.x + -sinf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + -g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );

	pVtx[3].pos = D3DXVECTOR3( g_posPolygon.x + sinf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 
								g_posPolygon.y + cosf( g_fAnglePolygon + g_rotPolygon.z ) * ( g_fLengthPolygon * polygonMag ) , 0.0f );
	
	

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBufftitleGui->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawTitleGui( void )
*		�^�C�g���C���^�[�t�F�[�X�̕`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawTitleGui( void )
{
	/*�f�o�C�X�̎擾*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();
	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBufftitleGui , 0 , sizeof( VERTEX_2D ) );

	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTexturetitleGui );
	
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}