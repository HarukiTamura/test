/*******************************************************************************
* �^�C�g��	:�t�F�[�h.cpp
* �t�@�C����:Fade.cpp
* �쐬��	:�c���t��
* �쐬��	:2015/07/08
********************************************************************************
* �X�V����	ver 1.00
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "fade.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/
#define FADE_VERTEX_MAX	( 4 )	/*�ō����_��*/

#define FADE_IN_SPEED	( 0.050f )	//�t�F�[�h�C���E�A�E�g�̑��x
#define FADE_OUT_SPEED	( 0.050f )

/*******************************************************************************
* �\���̒�`
*******************************************************************************/

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFade = NULL;	/*���_�o�b�t�@*/
static LPDIRECT3DTEXTURE9 g_pTextureFade;	/*�t�F�[�h�e�N�X�`���̃|�C���^*/
static D3DXIMAGE_INFO info;		/*�摜���*/

//�t�F�[�h�Ɋւ���l
static D3DXCOLOR g_colorFade;	//�t�F�[�h�p�̃|���S���F
static FADE g_fade;				//�t�F�[�h�̏��
static MODE g_modeNext;			//�t�F�[�h�A�E�g��̑J�ڐ�
static float g_alphaFade;		//�|���S���̃A���t�@�l
static float fadeOutR , fadeOutG , fadeOutB;
/*******************************************************************************
* �֐���	:HRESULT InitFade( void )
*		�t�F�[�h�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:HRESULT �֐��̎��s����
* ����		:�ϐ����̏����������s����N�����Ɉ�x�̂ݓ��삷��֐��ł��B
*******************************************************************************/
HRESULT InitFade( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/
	
	/*���_�o�b�t�@�̐���*/
	if( FAILED( pD3DDevice->CreateVertexBuffer( sizeof( VERTEX_2D ) * FADE_VERTEX_MAX , D3DUSAGE_WRITEONLY , FVF_VERTEX_2D , D3DPOOL_MANAGED , &g_pVtxBuffFade , NULL ) ) )
	{
		Error("���_�o�b�t�@�̐����Ɏ��s���܂����B");
		return E_FAIL;
	}
	
	/*�e�N�X�`���̓ǂݍ���*/
	D3DXCreateTextureFromFile( pD3DDevice , "data/TEXTURE/fade.png" , &g_pTextureFade );
	D3DXGetImageInfoFromFile( "data/TEXTURE/fade.png" , &info );

	/*�|�C���^�̃��b�N*/
	g_pVtxBuffFade->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
	/*���_���W�ݒ�*/
	pVtx[0].pos = D3DXVECTOR3( SCREEN_LEFT , SCREEN_TOP , 0.0f );
	pVtx[1].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_TOP , 0.0f );
	pVtx[2].pos = D3DXVECTOR3( SCREEN_LEFT , SCREEN_BOTTOM , 0.0f );
	pVtx[3].pos = D3DXVECTOR3( SCREEN_RIGHT , SCREEN_BOTTOM , 0.0f );

	/*���W�ϊ��p�W���ݒ�*/
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	
	/*���_�J���[�ݒ�*/
	pVtx[0].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	pVtx[1].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	pVtx[2].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	pVtx[3].col = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	
	/*�e�N�X�`�����W�ݒ�*/
	pVtx[0].tex = D3DXVECTOR2( 0 , 0 );
	pVtx[1].tex = D3DXVECTOR2( 1 , 0 );
	pVtx[2].tex = D3DXVECTOR2( 0 , 1 );
	pVtx[3].tex = D3DXVECTOR2( 1 , 1 );

	/*�|�C���^�̃A�����b�N*/
	g_pVtxBuffFade->Unlock();

	//�t�F�[�h�p�ϐ�������
	g_colorFade = D3DXCOLOR( 1.0f , 1.0f , 1.0f , 0.0f );
	g_fade = FADE_IN;
	g_modeNext = MODE_TITLE;
	g_alphaFade = 1.0f;

	fadeOutR = 0.0f;
	fadeOutG = 0.0f;
	fadeOutB = 0.0f;
	return S_OK;
}

/*******************************************************************************
* �֐���	:void UninitFade( void )
*		�t�F�[�h�I�������֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�I�����Ɏ��s����֐�������֐��ŏI�����Ɉ�x�̂ݓ���B
*******************************************************************************/
void UninitFade( void )
{
	if( g_pVtxBuffFade != NULL )
	{
		g_pVtxBuffFade->Release();
		g_pVtxBuffFade = NULL;
	}
	if( g_pTextureFade != NULL )
	{
		g_pTextureFade->Release();
		g_pTextureFade = NULL;
	}
}

/*******************************************************************************
* �֐���	:void UpdateFade( void )
*		�t�F�[�h�X�V�����֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:�������񖈂̏������e���L�q���܂��B�����I�ȃ��C���֐��ł��B
*******************************************************************************/
void UpdateFade( void )
{
	VERTEX_2D *pVtx;	/*���_�o�b�t�@�ւ̃|�C���^*/
	

	if( g_fade == FADE_IN )
	{
		g_alphaFade -= FADE_IN_SPEED;
		if( g_alphaFade <= 0.0f )
		{
			g_alphaFade = 0.0f;
			g_fade = FADE_NONE;
		}
	}
	else if( g_fade == FADE_OUT )
	{
		g_alphaFade += FADE_OUT_SPEED;
		if( g_alphaFade >= 1.0f )
		{
			g_alphaFade = 1.0f;
			g_fade = FADE_IN;
			SetMode( g_modeNext );
		}
		fadeOutR = 0.0f;
		fadeOutG = 0.0f;
		fadeOutB = 0.0f;
	}
	else if( g_fade == FADE_OUT_WHITE )
	{
		g_alphaFade += FADE_OUT_SPEED;
		if( g_alphaFade >= 1.0f )
		{
			g_alphaFade = 1.0f;
			g_fade = FADE_IN;
			SetMode( g_modeNext );
		}
		fadeOutR = 1.0f;
		fadeOutG = 1.0f;
		fadeOutB = 1.0f;
	}

	/*�|�C���^�̃��b�N*/
	if( g_pVtxBuffFade != NULL )
	{
		g_pVtxBuffFade->Lock( 0 , 0 , ( void** ) &pVtx , 0 );
	
		pVtx[0].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );
		pVtx[1].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );
		pVtx[2].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );
		pVtx[3].col = D3DXCOLOR( fadeOutR , fadeOutG , fadeOutB , g_alphaFade );

		/*�|�C���^�̃A�����b�N*/
		g_pVtxBuffFade->Unlock();
	}
}

/*******************************************************************************
* �֐���	:void DrawFade( void )
*		�t�F�[�h�`�揈���֐��ł��B
* ����		:void �����Ȃ�
* �߂�l	:void �߂�l�Ȃ�
* ����		:��ʏo�͂Ɋւ�鏈�������܂��B
*******************************************************************************/
void DrawFade( void )
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetDevice();	/*�f�o�C�X�̎擾*/

	/*���_�t�H�[�}�b�g�̐ݒ�*/
	pD3DDevice->SetFVF( FVF_VERTEX_2D );
	
	/*�e�N�X�`���̐ݒ�*/
	pD3DDevice->SetTexture( 0 , g_pTextureFade );
	
	/*���_�o�b�t�@���X�g���[���Ƀo�C���h*/
	pD3DDevice->SetStreamSource( 0 , g_pVtxBuffFade , 0 , sizeof( VERTEX_2D ) );

	/*�t�F�[�h�̕`��*/
	pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0 , 2 );
}

/*******************************************************************************
* �֐���	:void SetFade( FADE fade , MODE modeNext )
*		�t�F�[�h�̏�Ԃ�ݒ肵�܂��B
* ����		:FADE fade		�t�F�[�h�̐ݒ�ł��B
*			 MODE modeNext	�t�F�[�h�A�E�g���̐؂�ւ���ł��B
* �߂�l	:void �߂�l�Ȃ�
* ����		:�t�F�[�h�̏�Ԃ�ݒ肵�܂��B
*******************************************************************************/
void SetFade( FADE fade , MODE modeNext )
{
	g_fade = fade;
	g_modeNext = modeNext;
}


/*******************************************************************************
* �֐���	:FADE GetFade( void )
*		�t�F�[�h�̏�Ԃ��擾���܂��B
* ����		:void �����Ȃ�
* �߂�l	:FADE	���݂̃t�F�[�h���
* ����		:�t�F�[�h�̏�Ԃ��擾���܂��B
*******************************************************************************/
FADE GetFade( void )
{
	return g_fade;
}