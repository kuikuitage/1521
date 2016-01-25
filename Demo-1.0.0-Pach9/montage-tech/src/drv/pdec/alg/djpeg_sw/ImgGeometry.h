#ifndef __IMGGEOMETRY_H__
#define __IMGGEOMETRY_H__




#define		PIE		3.14159265358979323846

////////////////////////////图像处理/////////////////////

/* 版本号 */
#define		IMAGE_PROCESS_VERSION_1_0			1

//工业颜色模型


#define		IMAGE_COLOR_RGB						0X00000000
#define		IMAGE_COLOR_RGBA					0X00000001
#define		IMAGE_COLOR_INDEX					0X00000002



//图像亮度处理
#define		IMAGE_BRIGHTNESS_PERCENTAGE			0X00000100
#define		IMAGE_BRIGHTNESS_REVERSE			0X00000101

//图像对比度调节
#define		IMAGE_CONTRAST_ADJUSTMENT			0X00000200

//图像灰度处理
#define		IMAGE_GRAY_COLOR_GRAYED_OUT			0X00000300
#define		IMAGE_GRAY_PERCENTAGE				0X00000301
#define		IMAGE_GRAY_LINEARIZE				0X00000302
#define		IMAGE_GRAY_LINEAR_ROUND_OFF			0X00000303
#define		IMAGE_GRAY_REVERSE					0X00000304
#define		IMAGE_GRAY_HISTOGRAM_BALANCE		0X00000305
#define		IMAGE_GRAY_HISTOGRAM_MATCH			0X00000306
#define		IMAGE_GRAY_STATISTIC_MATCH			0X00000307

//图像着色处理
#define		IMAGE_COLOR_RENDER					0X00000400

//图像曝光处理
#define		IMAGE_COLOR_SOLARIZE				0X00000500

//图像区域处理

//图像模糊,
//to blur or smooth image
// 3-order--------5-order
#define		IMAGE_BLUR_MEAN_SMOOTH_3			0X00010000
#define		IMAGE_BLUR_MEAN_SMOOTH_5			0X00010001

//中值模糊,非线性处理
#define		IMAGE_BLUR_MEDIAN_FILTER			0X00010002

//前两种方法为低通过滤,最后一种方法为中值过滤
#define		IMAGE_LOWPASS_NOISE_REMOVE_3		0X00010100
#define		IMAGE_LOWPASS_NOISE_REMOVE_5		0X00010101
#define		IMAGE_MEDIAN_NOISE_REMOVE			0X00010102

//图像高通滤波
// to sharpen image
#define		IMAGE_HIGHPASS_BASIC_SHARPEN		0X00010200
#define		IMAGE_HIGHPASS_MODERATE_SHARPEN		0X00010201
#define		IMAGE_HIGHPASS_EXCESSIVE_SHARPEN	0X00010202
#define		IMAGE_HIGHPASS_OUTLINE_SHARPEN		0X00010203


//边缘检测

//Laplacian 过滤
//to enhance discontinuities
#define		IMAGE_LAPLACIAN_BASIC_DETECT		0X00010300
#define		IMAGE_LAPLACIAN_MODERATE_DETECT		0X00010301
#define		IMAGE_LAPLACIAN_EXCESSIVE_DETECT	0X00010302

//梯度检测
#define		IMAGE_GRADIENT_EAST_DETECT			0X00010400
#define		IMAGE_GRADIENT_SOUTH_DETECT			0X00010401
#define		IMAGE_GRADIENT_WEST_DETECT			0X00010402
#define		IMAGE_GRADIENT_NORTH_DETECT			0X00010403
#define		IMAGE_GRADIENT_NORTHEAST_DETECT		0X00010404
#define		IMAGE_GRADIENT_SOUTHWEST_DETECT		0X00010405
#define		IMAGE_GRADIENT_NORTHWEST_DETECT		0X00010406
#define		IMAGE_GRADIENT_SOUTHEAST_DETECT		0X00010407

//平移和差分边缘增强(检测)
#define		IMAGE_DIFFERENCE_EAST_DETECT		0X00010500
#define		IMAGE_DIFFERENCE_SOUTH_DETECT		0X00010501
#define		IMAGE_DIFFERENCE_WEST_DETECT		0X00010502
#define		IMAGE_DIFFERENCE_NORTH_DETECT		0X00010503
#define		IMAGE_DIFFERENCE_NORTHEAST_DETECT	0X00010504
#define		IMAGE_DIFFERENCE_SOUTHWEST_DETECT	0X00010505
#define		IMAGE_DIFFERENCE_NORTHWEST_DETECT	0X00010506
#define		IMAGE_DIFFERENCE_SOUTHEAST_DETECT	0X00010507

//Prewitt边缘增强(检测)
#define		IMAGE_PREWITT_EAST_DETECT			0X00010600
#define		IMAGE_PREWITT_SOUTH_DETECT			0X00010601
#define		IMAGE_PREWITT_WEST_DETECT			0X00010602
#define		IMAGE_PREWITT_NORTH_DETECT			0X00010603
#define		IMAGE_PREWITT_NORTHEAST_DETECT		0X00010604
#define		IMAGE_PREWITT_SOUTHWEST_DETECT		0X00010605
#define		IMAGE_PREWITT_NORTHWEST_DETECT		0X00010606
#define		IMAGE_PREWITT_SOUTHEAST_DETECT		0X00010607

//线检测
#define		IMAGE_LINE_HORIZONTAL_DETECT		0X00010700
#define		IMAGE_LINE_VERTICAL_DETECT			0X00010701
#define		IMAGE_LINE_LEFT_DIAGONAL_DETECT		0X00010702
#define		IMAGE_LINE_RIGHT_DIAGONAL_DETECT	0X00010703

//非线性边缘检测
#define		IMAGE_SOBEL_EDGE_DETECT				0X0001FE00
#define		IMAGE_KIRSCH_EDGE_DETECT			0X0001FE01

//.................,and so on.

//自定义卷积核
#define		IMAGE_GENERAL_CONVOLUTION_FILTER	0X0001FF00

//小块扩散过滤
#define		IMAGE_DIFFUSE_PLOT_FILTER			0X00020000

//颗粒粗化
#define		IMAGE_GRANUALATION_PLOT_FILTER		0X00020100

//图像合成

#define		IMAGE_TRANSPARENT_COLOR_COMPOSE		0X00030000

//图像间的代数运算
#define		IMAGE_ALGEBRA_ADD					0X00030100
#define		IMAGE_ALGEBRA_SUBTRACT				0X00030101
#define		IMAGE_ALGEBRA_MULTIPLY				0X00030102
#define		IMAGE_ALGEBRA_DIVIDE				0X00030103
#define		IMAGE_ALGEBRA_AVERAGE				0X00030104

//图像间的逻辑运算
#define		IMAGE_LOGIC_AND						0X00030200
#define		IMAGE_LOGIC_OR						0X00030201
#define		IMAGE_LOGIC_XOR						0X00030202

//图像融合处理

#define		IMAGE_BLEND							0X00030300

//图像融合源因子
//用于设置Alpha的计算线索
#define		IMAGE_BLEND_SFACTOR_255					0X00030400	
#define		IMAGE_BLEND_SFACTOR_ZERO				0X00030401
#define		IMAGE_BLEND_SFACTOR_DST_COLOR			0X00030402
#define		IMAGE_BLEND_SFACTOR_255_MINUS_DST_COLOR	0X00030403 
#define		IMAGE_BLEND_SFACTOR_SRC_ALPHA			0X00030404
#define		IMAGE_BLEND_SFACTOR_255_MINUS_SRC_ALPHA	0X00030405
#define		IMAGE_BLEND_SFACTOR_DST_ALPHA			0X00030406
#define		IMAGE_BLEND_SFACTOR_255_MINUS_DST_ALPHA	0X00030407
#define		IMAGE_BLEND_SFACTOR_SRC_ALPHA_SATURATE	0X00030408

//图像融合目标因子
//用于设置Alpha的计算线索
#define		IMAGE_BLEND_DFACTOR_255					0X00030500
#define		IMAGE_BLEND_DFACTOR_ZERO				0X00030501
#define		IMAGE_BLEND_DFACTOR_SRC_COLOR			0X00030502
#define		IMAGE_BLEND_DFACTOR_255_MINUS_SRC_COLOR	0X00030503
#define		IMAGE_BLEND_DFACTOR_SRC_ALPHA			0X00030504
#define		IMAGE_BLEND_DFACTOR_255_MINUS_SRC_ALPHA	0X00030505
#define		IMAGE_BLEND_DFACTOR_DST_ALPHA			0X00030506
#define		IMAGE_BLEND_DFACTOR_255_MINUS_DST_ALPHA	0X00030507

//形态学操作

#define		IMAGE_MATH_MORPHOLOGY_DILATE			0X00040000
#define		IMAGE_MATH_MORPHOLOGY_ERODE				0X00040001
#define		IMAGE_MATH_MORPHOLOGY_OPEN				0X00040002
#define		IMAGE_MATH_MORPHOLOGY_CLOSE				0X00040003
#define		IMAGE_MATH_MORPHOLOGY_PICK_DETECT		0X00040004
#define		IMAGE_MATH_MORPHOLOGY_DENOISE			0X00040005
#define		IMAGE_MATH_MORPHOLOGY_OPEN_COMPOSE		0X00040006
#define		IMAGE_MATH_MORPHOLOGY_GRADIENTIZE		0X00040007
#define		IMAGE_MATH_MORPHOLOGY_TOP_HAT			0X00040008

//图像几何变换 

#define		IMAGE_GEOMETRY_TRANSLATE					0X00050000
#define		IMAGE_GEOMETRY_SCALE						0X00050001
#define		IMAGE_GEOMETRY_ROTATE_CW_90					0X00050002
#define		IMAGE_GEOMETRY_ROTATE_CCW_90				0X00050003
#define		IMAGE_GEOMETRY_ROTATE_180					0X00050004
#define		IMAGE_GEOMETRY_ROTATE_0						0X00050005
#define		IMAGE_GEOMETRY_ROTATE						0X00050006
#define		IMAGE_GEOMETRY_MIRROR_X						0X00050007
#define		IMAGE_GEOMETRY_MIRROR_Y						0X00050008

//几何变换的质量
#define		IMAGE_GEOMETRY_NEAREST_NEIGHBOR_INTERPOLATE	0X00050100
#define		IMAGE_GEOMETRY_BILINEAR_INTERPOLATE			0X00050101
#define		IMAGE_GEOMETRY_THREE_ORDER_INTERPOLATE		0X00050102

//图像一般变换

#define		IMAGE_GEOMETRY_GENERAL_TRANSFORM			0X00050200

//扭曲
#define		IMAGE_GEOMETRY_WARP_X						0X00050300
#define		IMAGE_GEOMETRY_WARP_Y						0X00050301

////////////////////////////////////////////////////////////////////////////////

//*******************		结构定义       *********************

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

//图像颜色:RGB
typedef struct tagPIXELCOLORRGB
{
	BYTE red;
	BYTE green;
	BYTE blue;
}PIXELCOLORRGB;

//图像颜色:RGBA
typedef struct tagPIXELCOLORRGBA
{
	BYTE red;
	BYTE green;
	BYTE blue;
	BYTE alpha;
}PIXELCOLORRGBA;

//形态学结构

//结构元素
typedef struct tagMORPHOLOGYELEMENT
{
	int x;			//元素的X坐标
	int y;			//元素的Y坐标
	short red;		//该元素的颜色:红色分量,可以为负值
	short green;	//该元素的颜色:绿色分量，可以为负值
	short blue;		//该元素的颜色:蓝色分量，可以为负值
}MORPHOLOGYELEMENT;

//二维浮点向量
typedef struct tagFLOATPOINT
{
	float x;
	float y;
}FLOATPOINT;


////////////////////////////////////////////////////////////////////////////////

//*******************		基本函数       *********************

////////////////////////////////////////////////////////////////////////////////

//截断函数
#define BOUND(x,a,b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

//绝对值函数
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
// 版权所有(2002)
// Copyright(2002)
// 编写者: 向世明
// Author: Xiang Shiming


//图像几何变换

#include "ImgGeometry.h"
#include "math.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///lpbySrcXY----传递源像素(x, y)的地址, 
//x, y, 经过反向变换后得到的对应于原图像的象点的坐标
//nScanWidth,  int nScanHeight, 源扫描宽度和扫描高度
PIXELCOLORRGB Interpolate(LPBYTE lpbySrcXY,  int x,  int y, 
						  float fu,  float fv,  int nScanWidth, 
						  int nScanHeight);


//lpbyBitsSrc32----传递源像素值, 
//x, y, nWidth,  nHeight, 定义区域的宽度
//nScanWidth,  nScanHeight, 扫描宽度和扫描高度
//lpbyBitsDst32----存放最后缩放后的结果
//nWidthImgDst, nHeightImgDst缩放至多大

BOOL Scale(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
		   int nHeight,  int nScanWidth,  int nScanHeight, 
		   LPBYTE lpbyBitsDst32, int nWidthImgDst, int nHeightImgDst);


//顺时针旋转90度
//包括 n * 360 - 90 度 
//x, y, nWidth,  nHeight-----定义子区域
//lpbyBitsDst32----大小为nWidth * nHeight * 4;
//nScanWidth-------源图像扫描宽度
//nScanHeight------源图像扫描高度
BOOL RotateCW90(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
				int nHeight,  int nScanWidth,  int nScanHeight,
				LPBYTE lpbyBitsDst32);


//逆时针旋转90度
//lpbyBitsDst32----大小为nWidth * nHeight * 4;
BOOL RotateCCW90(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
				 int nHeight,  int nScanWidth,  int nScanHeight,  
				 LPBYTE lpbyBitsDst32);


BOOL Rotate180(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
			   int nHeight,  int nScanWidth,  int nScanHeight, 
			   LPBYTE lpbyBitsDst32);


//旋转..., -360, 0, 360, ...度
BOOL Rotate0(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
			 int nHeight,  int nScanWidth,  int nScanHeight, 
			 LPBYTE lpbyBitsDst32);



//lpbyBitsSrc32----传递源像素值, 
//x, y, nWidth,  nHeight, 定义区域的宽度
//nScanWidth,  nScanHeight, 扫描宽度和扫描高度
//lpbyBitsDst32----存放最后缩放后的结果

//fTheta ---- 旋转角度

//nWidthImgDst, nHeightDst旋转后的尺寸, 事实上它们可以通过nWidth,  nHeight和fTheta计算出来.

BOOL Rotate(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
			int nHeight,  int nScanWidth,  int nScanHeight,
			LPBYTE lpbyBitsDst32, float fTheta, int nWidthImgDst, 
			int nHeightImgDst);


//水平镜象,但是镜象轴为 Y 轴
BOOL MirrorY(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth, 
			 int nHeight,  int nScanWidth,  int nScanHeight,  
			 LPBYTE lpbyBitsDst32);


//垂直镜象,但是镜象轴为 X 轴
BOOL MirrorX(LPBYTE lpbyBitsSrc32,  int x,  int y,  int nWidth,  
			 int nHeight,  int nScanWidth,  int nScanHeight, 
			 LPBYTE lpbyBitsDst32);







#endif
