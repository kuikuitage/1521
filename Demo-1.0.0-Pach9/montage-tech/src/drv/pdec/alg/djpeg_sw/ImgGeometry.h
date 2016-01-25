#ifndef __IMGGEOMETRY_H__
#define __IMGGEOMETRY_H__




#define		PIE		3.14159265358979323846

////////////////////////////ͼ����/////////////////////

/* �汾�� */
#define		IMAGE_PROCESS_VERSION_1_0			1

//��ҵ��ɫģ��


#define		IMAGE_COLOR_RGB						0X00000000
#define		IMAGE_COLOR_RGBA					0X00000001
#define		IMAGE_COLOR_INDEX					0X00000002



//ͼ�����ȴ���
#define		IMAGE_BRIGHTNESS_PERCENTAGE			0X00000100
#define		IMAGE_BRIGHTNESS_REVERSE			0X00000101

//ͼ��Աȶȵ���
#define		IMAGE_CONTRAST_ADJUSTMENT			0X00000200

//ͼ��Ҷȴ���
#define		IMAGE_GRAY_COLOR_GRAYED_OUT			0X00000300
#define		IMAGE_GRAY_PERCENTAGE				0X00000301
#define		IMAGE_GRAY_LINEARIZE				0X00000302
#define		IMAGE_GRAY_LINEAR_ROUND_OFF			0X00000303
#define		IMAGE_GRAY_REVERSE					0X00000304
#define		IMAGE_GRAY_HISTOGRAM_BALANCE		0X00000305
#define		IMAGE_GRAY_HISTOGRAM_MATCH			0X00000306
#define		IMAGE_GRAY_STATISTIC_MATCH			0X00000307

//ͼ����ɫ����
#define		IMAGE_COLOR_RENDER					0X00000400

//ͼ���ع⴦��
#define		IMAGE_COLOR_SOLARIZE				0X00000500

//ͼ��������

//ͼ��ģ��,
//to blur or smooth image
// 3-order--------5-order
#define		IMAGE_BLUR_MEAN_SMOOTH_3			0X00010000
#define		IMAGE_BLUR_MEAN_SMOOTH_5			0X00010001

//��ֵģ��,�����Դ���
#define		IMAGE_BLUR_MEDIAN_FILTER			0X00010002

//ǰ���ַ���Ϊ��ͨ����,���һ�ַ���Ϊ��ֵ����
#define		IMAGE_LOWPASS_NOISE_REMOVE_3		0X00010100
#define		IMAGE_LOWPASS_NOISE_REMOVE_5		0X00010101
#define		IMAGE_MEDIAN_NOISE_REMOVE			0X00010102

//ͼ���ͨ�˲�
// to sharpen image
#define		IMAGE_HIGHPASS_BASIC_SHARPEN		0X00010200
#define		IMAGE_HIGHPASS_MODERATE_SHARPEN		0X00010201
#define		IMAGE_HIGHPASS_EXCESSIVE_SHARPEN	0X00010202
#define		IMAGE_HIGHPASS_OUTLINE_SHARPEN		0X00010203


//��Ե���

//Laplacian ����
//to enhance discontinuities
#define		IMAGE_LAPLACIAN_BASIC_DETECT		0X00010300
#define		IMAGE_LAPLACIAN_MODERATE_DETECT		0X00010301
#define		IMAGE_LAPLACIAN_EXCESSIVE_DETECT	0X00010302

//�ݶȼ��
#define		IMAGE_GRADIENT_EAST_DETECT			0X00010400
#define		IMAGE_GRADIENT_SOUTH_DETECT			0X00010401
#define		IMAGE_GRADIENT_WEST_DETECT			0X00010402
#define		IMAGE_GRADIENT_NORTH_DETECT			0X00010403
#define		IMAGE_GRADIENT_NORTHEAST_DETECT		0X00010404
#define		IMAGE_GRADIENT_SOUTHWEST_DETECT		0X00010405
#define		IMAGE_GRADIENT_NORTHWEST_DETECT		0X00010406
#define		IMAGE_GRADIENT_SOUTHEAST_DETECT		0X00010407

//ƽ�ƺͲ�ֱ�Ե��ǿ(���)
#define		IMAGE_DIFFERENCE_EAST_DETECT		0X00010500
#define		IMAGE_DIFFERENCE_SOUTH_DETECT		0X00010501
#define		IMAGE_DIFFERENCE_WEST_DETECT		0X00010502
#define		IMAGE_DIFFERENCE_NORTH_DETECT		0X00010503
#define		IMAGE_DIFFERENCE_NORTHEAST_DETECT	0X00010504
#define		IMAGE_DIFFERENCE_SOUTHWEST_DETECT	0X00010505
#define		IMAGE_DIFFERENCE_NORTHWEST_DETECT	0X00010506
#define		IMAGE_DIFFERENCE_SOUTHEAST_DETECT	0X00010507

//Prewitt��Ե��ǿ(���)
#define		IMAGE_PREWITT_EAST_DETECT			0X00010600
#define		IMAGE_PREWITT_SOUTH_DETECT			0X00010601
#define		IMAGE_PREWITT_WEST_DETECT			0X00010602
#define		IMAGE_PREWITT_NORTH_DETECT			0X00010603
#define		IMAGE_PREWITT_NORTHEAST_DETECT		0X00010604
#define		IMAGE_PREWITT_SOUTHWEST_DETECT		0X00010605
#define		IMAGE_PREWITT_NORTHWEST_DETECT		0X00010606
#define		IMAGE_PREWITT_SOUTHEAST_DETECT		0X00010607

//�߼��
#define		IMAGE_LINE_HORIZONTAL_DETECT		0X00010700
#define		IMAGE_LINE_VERTICAL_DETECT			0X00010701
#define		IMAGE_LINE_LEFT_DIAGONAL_DETECT		0X00010702
#define		IMAGE_LINE_RIGHT_DIAGONAL_DETECT	0X00010703

//�����Ա�Ե���
#define		IMAGE_SOBEL_EDGE_DETECT				0X0001FE00
#define		IMAGE_KIRSCH_EDGE_DETECT			0X0001FE01

//.................,and so on.

//�Զ�������
#define		IMAGE_GENERAL_CONVOLUTION_FILTER	0X0001FF00

//С����ɢ����
#define		IMAGE_DIFFUSE_PLOT_FILTER			0X00020000

//�����ֻ�
#define		IMAGE_GRANUALATION_PLOT_FILTER		0X00020100

//ͼ��ϳ�

#define		IMAGE_TRANSPARENT_COLOR_COMPOSE		0X00030000

//ͼ���Ĵ�������
#define		IMAGE_ALGEBRA_ADD					0X00030100
#define		IMAGE_ALGEBRA_SUBTRACT				0X00030101
#define		IMAGE_ALGEBRA_MULTIPLY				0X00030102
#define		IMAGE_ALGEBRA_DIVIDE				0X00030103
#define		IMAGE_ALGEBRA_AVERAGE				0X00030104

//ͼ�����߼�����
#define		IMAGE_LOGIC_AND						0X00030200
#define		IMAGE_LOGIC_OR						0X00030201
#define		IMAGE_LOGIC_XOR						0X00030202

//ͼ���ںϴ���

#define		IMAGE_BLEND							0X00030300

//ͼ���ں�Դ����
//��������Alpha�ļ�������
#define		IMAGE_BLEND_SFACTOR_255					0X00030400	
#define		IMAGE_BLEND_SFACTOR_ZERO				0X00030401
#define		IMAGE_BLEND_SFACTOR_DST_COLOR			0X00030402
#define		IMAGE_BLEND_SFACTOR_255_MINUS_DST_COLOR	0X00030403 
#define		IMAGE_BLEND_SFACTOR_SRC_ALPHA			0X00030404
#define		IMAGE_BLEND_SFACTOR_255_MINUS_SRC_ALPHA	0X00030405
#define		IMAGE_BLEND_SFACTOR_DST_ALPHA			0X00030406
#define		IMAGE_BLEND_SFACTOR_255_MINUS_DST_ALPHA	0X00030407
#define		IMAGE_BLEND_SFACTOR_SRC_ALPHA_SATURATE	0X00030408

//ͼ���ں�Ŀ������
//��������Alpha�ļ�������
#define		IMAGE_BLEND_DFACTOR_255					0X00030500
#define		IMAGE_BLEND_DFACTOR_ZERO				0X00030501
#define		IMAGE_BLEND_DFACTOR_SRC_COLOR			0X00030502
#define		IMAGE_BLEND_DFACTOR_255_MINUS_SRC_COLOR	0X00030503
#define		IMAGE_BLEND_DFACTOR_SRC_ALPHA			0X00030504
#define		IMAGE_BLEND_DFACTOR_255_MINUS_SRC_ALPHA	0X00030505
#define		IMAGE_BLEND_DFACTOR_DST_ALPHA			0X00030506
#define		IMAGE_BLEND_DFACTOR_255_MINUS_DST_ALPHA	0X00030507

//��̬ѧ����

#define		IMAGE_MATH_MORPHOLOGY_DILATE			0X00040000
#define		IMAGE_MATH_MORPHOLOGY_ERODE				0X00040001
#define		IMAGE_MATH_MORPHOLOGY_OPEN				0X00040002
#define		IMAGE_MATH_MORPHOLOGY_CLOSE				0X00040003
#define		IMAGE_MATH_MORPHOLOGY_PICK_DETECT		0X00040004
#define		IMAGE_MATH_MORPHOLOGY_DENOISE			0X00040005
#define		IMAGE_MATH_MORPHOLOGY_OPEN_COMPOSE		0X00040006
#define		IMAGE_MATH_MORPHOLOGY_GRADIENTIZE		0X00040007
#define		IMAGE_MATH_MORPHOLOGY_TOP_HAT			0X00040008

//ͼ�񼸺α任 

#define		IMAGE_GEOMETRY_TRANSLATE					0X00050000
#define		IMAGE_GEOMETRY_SCALE						0X00050001
#define		IMAGE_GEOMETRY_ROTATE_CW_90					0X00050002
#define		IMAGE_GEOMETRY_ROTATE_CCW_90				0X00050003
#define		IMAGE_GEOMETRY_ROTATE_180					0X00050004
#define		IMAGE_GEOMETRY_ROTATE_0						0X00050005
#define		IMAGE_GEOMETRY_ROTATE						0X00050006
#define		IMAGE_GEOMETRY_MIRROR_X						0X00050007
#define		IMAGE_GEOMETRY_MIRROR_Y						0X00050008

//���α任������
#define		IMAGE_GEOMETRY_NEAREST_NEIGHBOR_INTERPOLATE	0X00050100
#define		IMAGE_GEOMETRY_BILINEAR_INTERPOLATE			0X00050101
#define		IMAGE_GEOMETRY_THREE_ORDER_INTERPOLATE		0X00050102

//ͼ��һ��任

#define		IMAGE_GEOMETRY_GENERAL_TRANSFORM			0X00050200

//Ť��
#define		IMAGE_GEOMETRY_WARP_X						0X00050300
#define		IMAGE_GEOMETRY_WARP_Y						0X00050301

////////////////////////////////////////////////////////////////////////////////

//*******************		�ṹ����       *********************

////////////////////////////////////////////////////////////////////////////////
typedef unsigned char       BYTE;
typedef BYTE             *LPBYTE;

typedef unsigned long       DWORD;

#ifndef  TRUE
#define TRUE (1)
#endif

#ifndef  FALSE
#define FALSE (0)
#endif

#ifndef  NULL
#define NULL (0)
#endif

//ͼ����ɫ:RGB
typedef struct tagPIXELCOLORRGB
{
	BYTE red;
	BYTE green;
	BYTE blue;
}PIXELCOLORRGB;

//ͼ����ɫ:RGBA
typedef struct tagPIXELCOLORRGBA
{
	BYTE red;
	BYTE green;
	BYTE blue;
	BYTE alpha;
}PIXELCOLORRGBA;

//��̬ѧ�ṹ

//�ṹԪ��
typedef struct tagMORPHOLOGYELEMENT
{
	int x;			//Ԫ�ص�X����
	int y;			//Ԫ�ص�Y����
	short red;		//��Ԫ�ص���ɫ:��ɫ����,����Ϊ��ֵ
	short green;	//��Ԫ�ص���ɫ:��ɫ����������Ϊ��ֵ
	short blue;		//��Ԫ�ص���ɫ:��ɫ����������Ϊ��ֵ
}MORPHOLOGYELEMENT;

//��ά��������
typedef struct tagFLOATPOINT
{
	float x;
	float y;
}FLOATPOINT;


////////////////////////////////////////////////////////////////////////////////

//*******************		��������       *********************

////////////////////////////////////////////////////////////////////////////////

//�ضϺ���
#define BOUND(x,a,b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

//����ֵ����
#define ABS(x)	(((x) < 0) ? -(x) : (((x) > 0) ? (x) : 0))

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif




















/////////////////////////////////////////////////////////////////////////////////
//
// ImageGeometry.cpp: implementation of the CImageGeometry class.
//
////////////////////////////////////////////////////////////////////////////////
// ��Ȩ����(2002)
// Copyright(2002)
// ��д��: ������
// Author: Xiang Shiming


//ͼ�񼸺α任

#include "ImgGeometry.h"
#include "math.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///lpbySrcXY----����Դ����(x, y)�ĵ�ַ, 
//x, y, ��������任��õ��Ķ�Ӧ��ԭͼ�����������
//nScanWidth,  int nScanHeight, Դɨ���Ⱥ�ɨ��߶�
PIXELCOLORRGB Interpolate(LPBYTE lpbySrcXY,  int x,  int y, 
						  float fu,  float fv,  int nScanWidth, 
						  int nScanHeight);


//lpbyBitsSrc32----����Դ����ֵ, 
//x, y, nWidth,  nHeight, ��������Ŀ��
//nScanWidth,  nScanHeight, ɨ���Ⱥ�ɨ��߶�
//lpbyBitsDst32----���������ź�Ľ��
//nWidthImgDst, nHeightImgDst���������

BOOL Scale(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
		   int nHeight,  int nScanWidth,  int nScanHeight, 
		   LPBYTE lpbyBitsDst32, int nWidthImgDst, int nHeightImgDst);


//˳ʱ����ת90��
//���� n * 360 - 90 �� 
//x, y, nWidth,  nHeight-----����������
//lpbyBitsDst32----��СΪnWidth * nHeight * 4;
//nScanWidth-------Դͼ��ɨ����
//nScanHeight------Դͼ��ɨ��߶�
BOOL RotateCW90(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
				int nHeight,  int nScanWidth,  int nScanHeight,
				LPBYTE lpbyBitsDst32);


//��ʱ����ת90��
//lpbyBitsDst32----��СΪnWidth * nHeight * 4;
BOOL RotateCCW90(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
				 int nHeight,  int nScanWidth,  int nScanHeight,  
				 LPBYTE lpbyBitsDst32);


BOOL Rotate180(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
			   int nHeight,  int nScanWidth,  int nScanHeight, 
			   LPBYTE lpbyBitsDst32);


//��ת..., -360, 0, 360, ...��
BOOL Rotate0(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
			 int nHeight,  int nScanWidth,  int nScanHeight, 
			 LPBYTE lpbyBitsDst32);



//lpbyBitsSrc32----����Դ����ֵ, 
//x, y, nWidth,  nHeight, ��������Ŀ��
//nScanWidth,  nScanHeight, ɨ���Ⱥ�ɨ��߶�
//lpbyBitsDst32----���������ź�Ľ��

//fTheta ---- ��ת�Ƕ�

//nWidthImgDst, nHeightDst��ת��ĳߴ�, ��ʵ�����ǿ���ͨ��nWidth,  nHeight��fTheta�������.

BOOL Rotate(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
			int nHeight,  int nScanWidth,  int nScanHeight,
			LPBYTE lpbyBitsDst32, float fTheta, int nWidthImgDst, 
			int nHeightImgDst);


//ˮƽ����,���Ǿ�����Ϊ Y ��
BOOL MirrorY(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
			 int nHeight,  int nScanWidth,  int nScanHeight,  
			 LPBYTE lpbyBitsDst32);


//��ֱ����,���Ǿ�����Ϊ X ��
BOOL MirrorX(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
			 int nHeight,  int nScanWidth,  int nScanHeight, 
			 LPBYTE lpbyBitsDst32);







#endif
