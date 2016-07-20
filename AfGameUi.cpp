/*******************************************************************************
* �^�C�g��	:�w�i.cpp
* �t�@�C����:AfGameUi.cpp
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
#include "AfGameUi.h"
#include "AfComboNumber.h"
#include "AfPlayer.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define AF_GAME_UI_VERTEX_MAX ( 8 )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPolygon2 = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureBack2;	/*�w�i�e�N�X�`���̃|�C���^*/
static float texPos = 0;	/*���W���Z�l*/
static int fade = 255;
/*******************************************************************************
* �֐���	:HRESULT InitAfGameUi( void )
*		�w�i�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitAfGameUi( void )
{
	VERTEX_2D *pVtxBack2;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * AF_GAME_UI_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffPolygon2 , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}

	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/AfGameUi.png" , &g_pTextureBack2 );
	
	/*�|�C���^�̃��b�N*/
	g_pVtxBuffPolygon2->Lock( 0 , 0 , ( void** ) &pVtxBack2 , 0 );
	
	ZeroMemory( pVtxBack2 , sizeof( VERTEX_2D ) * AF_GAME_UI_VERTEX_MAX );

	/*���_���W�ݒ�*/
	pVtxBack2[0].pos = D3DXVECTOR3( WINDOW_WIDTH / 2 , 0.0f , 0.0f );
	pVtxBack2[1].pos = D3DXVECTOR3( WINDOW_WIDTH , 0.0f , 0.0f );
	pVtxBack2[2].pos = D3DXVECTOR3( WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2 , 0.0f );
	pVtxBack2[3].pos = D3DXVECTOR3( WINDOW_WIDTH , WINDOW_HEIGHT / 2, 0.0f );

	pVtxBack2[4].pos = D3DXVECTOR3( WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2 , 0.0f );
	pVtxBack2[5].pos = D3DXVECTOR3( WINDOW_WIDTH , WINDOW_HEIGHT / 2, 0.0f );
	pVtxBack2[6].pos = D3DXVECTOR3( WINDOW_WIDTH / 2 , WINDOW_HEIGHT , 0.0f );
	pVtxBack2[7].pos = D3DXVECTOR3( WINDOW_WIDTH , WINDOW_HEIGHT, 0.0f );

	/*���W�ϊ��p�W���ݒ�*/
	pVtxBack2[0].rhw = 1.0f;
	pVtxBack2[1].rhw = 1.0f;
	pVtxBack2[2].rhw = 1.0f;
	pVtxBack2[3].rhw = 1.0f;
	
	pVtxBack2[4].rhw = 1.0f;
	pVtxBack2[5].rhw = 1.0f;
	pVtxBack2[6].rhw = 1.0f;
	pVtxBack2[7].rhw = 1.0f;
	
	/*���_�J���[�ݒ�*/
	pVtxBack2[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	pVtxBack2[4].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[5].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[6].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtxBack2[7].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );

	/*�e�N�X�`�����W�ݒ�*/
	pVtxBack2[0].tex = D3DXVECTOR2( 0.5 , 0.0 );
	pVtxBack2[1].tex = D3DXVECTOR2( 1.0 , 0.0 );
	pVtxBack2[2].tex = D3DXVECTOR2( 0.5 , 0.5 );
	pVtxBack2[3].tex = D3DXVECTOR2( 1.0 , 0.5 );

	pVtxBack2[4].tex = D3DXVECTOR2( 0.5 , 0.5 );
	pVtxBack2[5].tex = D3DXVECTOR2( 1.0 , 0.5 );
	pVtxBack2[6].tex = D3DXVECTOR2( 0.5 , 1.0 );
	pVtxBack2[7].tex = D3DXVECTOR2( 1.0 , 1.0 );
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffPolygon2->Unlock();

	fade = 255;

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitAfGameUi( void )
*		�w�i�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitAfGameUi( void )
{
	if( g_pVtxBuffPolygon2 != NULL )
	{
		g_pVtxBuffPolygon2->Release();
		g_pVtxBuffPolygon2 = NULL;
	}
	if( g_pTextureBack2 != NULL )
	{
		g_pTextureBack2->Release();
		g_pTextureBack2 = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateAfGameUi( void )
*		�w�i�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateAfGameUi( void )
{
	VERTEX_2D *pVtxBack2;	/*���_�o�b�t�@�ւ̃|�C���^*/

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffPolygon2->Lock( 0 , 0 , ( void** ) &pVtxBack2 , 0 );
	
	
	int fadeColor = GetFadeAfComboNum();
	/*���_�J���[�ݒ�*/
	pVtxBack2[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fadeColor );
	pVtxBack2[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fadeColor );
	pVtxBack2[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fadeColor );
	pVtxBack2[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fadeColor );

	if( GetPlayerState()->posStatus.DrawPosY >= AF_PLAYER_DOWN_LIMIT &&
		GetPlayerState()->posStatus.DrawPosX >= AF_PLAYER_RIGHT_LIMIT )
	{
		fade = ( fade - 5 > 0 ? fade - 5 : 0 );
		pVtxBack2[4].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtxBack2[5].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtxBack2[6].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtxBack2[7].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
	}
	else
	{
		fade = ( fade + 5 < 255 ? fade + 5 : 255 );
		pVtxBack2[4].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtxBack2[5].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtxBack2[6].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
		pVtxBack2[7].col = D3DCOLOR_RGBA( 255 , 255 , 255 , fade );
	}
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffPolygon2->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawAfGameUi( void )
*		�w�i�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawAfGameUi( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureBack2 );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffPolygon2 , 0 , sizeof( VERTEX_2D ) );

	/*�w�i�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 4 , 2 );
}
