/*******************************************************************************
* �^�C�g��	:�J�[�\��.cpp
* �t�@�C����:Cursor.cpp
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
#include "cursor.h"
#include "input.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define POLYGON_VERTEX_MAX ( 4 )
#define CURSOR_SIZE	( 0.2f )	/*�J�[�\���̕\���{���ł�*/
/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffCursor = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureCursorAkr;	/*�J�[�\���e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

static float texPos = 0;	/*���W���Z�l*/

/*******************************************************************************
* �֐���	:HRESULT InitCursor( void )
*		�J�[�\���������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitCursor( void )
{
	int verCnt;

	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * POLYGON_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffCursor , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}

	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/cursor.png" , &g_pTextureCursorAkr );
	D3DXGetImageInfoFromFile( "data/TEXTURE/cursor.png" , &info );
	
	/*�|�C���^�̃��b�N*/
	g_pVtxBuffCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( 200 , 50 , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( 200 + info.Width * CURSOR_SIZE , 50 , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( 200 , 50 + info.Height * CURSOR_SIZE, 0.0f );
	pVtx[3].pos = D3DXVECTOR3( 200 + info.Width * CURSOR_SIZE , 50 + info.Height * CURSOR_SIZE , 0.0f );

	/*���W�ϊ��p�W���ݒ�*/
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	
	/*���_�J���[�ݒ�*/
	pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	
	/*�e�N�X�`�����W�ݒ�*/
	pVtx[0].tex = D3DXVECTOR2( 0 , 0 );
	pVtx[1].tex = D3DXVECTOR2( 1 , 0 );
	pVtx[2].tex = D3DXVECTOR2( 0 , 1 );
	pVtx[3].tex = D3DXVECTOR2( 1 , 1 );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffCursor->Unlock();

	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitCursor( void )
*		�J�[�\���I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitCursor( void )
{
	if( g_pVtxBuffCursor != NULL )
	{
		g_pVtxBuffCursor->Release();
		g_pVtxBuffCursor = NULL;
	}
	if( g_pTextureCursorAkr != NULL )
	{
		g_pTextureCursorAkr->Release();
		g_pTextureCursorAkr = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateCursor( void )
*		�J�[�\���X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateCursor( void )
{
	int verCnt;
	static float vector = 0;
	/*��]�p�ϐ�*/
	int x , x1 , x2 , x3 , x4;	/*�Ίp��XY���W�l*/
	int y , y1 , y2 , y3 , y4;
	int width , height;
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	POINT s_mouse;

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffCursor->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	GetCursorPos( &s_mouse );
	ScreenToClient( GetWnd() , &s_mouse );
	
	width = info.Width * CURSOR_SIZE;
	height = info.Height * CURSOR_SIZE;

	if( GetMousePress( 0 ) == true )
	{
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 12 , 12 , 255 );
	}
	else
	{
		pVtx[0].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[1].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[2].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
		pVtx[3].col = D3DCOLOR_RGBA( 255 , 255 , 255 , 255 );
	}

	pVtx[0].pos = D3DXVECTOR3( s_mouse.x , s_mouse.y , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( s_mouse.x + width , s_mouse.y , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( s_mouse.x , s_mouse.y + height , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( s_mouse.x + width , s_mouse.y + height , 0.0f );
	
	

	/*�����ŕό`�����Ƃ�*/
	
	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffCursor->Unlock();
}

/*******************************************************************************
* �֐���	:void DrawCursor( void )
*		�J�[�\���`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawCursor( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureCursorAkr );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffCursor , 0 , sizeof( VERTEX_2D ) );

	/*�J�[�\���̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );

}