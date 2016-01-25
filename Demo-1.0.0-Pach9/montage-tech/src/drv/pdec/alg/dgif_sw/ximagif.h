/*
 * File:	ximagif.h
 * Purpose:	GIF Image Class Loader and Writer
 */
/* ==========================================================
 * CxImageGIF (c) 07/Aug/2001 Davide Pizzolato - www.xdp.it
 * For conditions of distribution and use, see copyright notice in ximage.h
 *
 * Special thanks to Troels Knakkergaard for new features, enhancements and bugfixes
 *
 * original CImageGIF  and CImageIterator implementation are:
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra(at)servidor(dot)unam(dot)mx>
 *
 * 6/15/97 Randy Spann: Added GIF87a writing support
 *         R.Spann@ConnRiver.net
 *
 * DECODE.C - An LZW decoder for GIF
 * Copyright (C) 1987, by Steven A. Bennett
 * Copyright (C) 1994, C++ version by Alejandro Aguilar Sierra
 *
 * In accordance with the above, I want to credit Steve Wilhite who wrote
 * the code which this is heavily inspired by...
 *
 * GIF and 'Graphics Interchange Format' are trademarks (tm) of
 * Compuserve, Incorporated, an H&R Block Company.
 *
 * Release Notes: This file contains a decoder routine for GIF images
 * which is similar, structurally, to the original routine by Steve Wilhite.
 * It is, however, somewhat noticably faster in most cases.
 *
 * ==========================================================
 */

#if !defined(__ximaGIF_h)
#define __ximaGIF_h


//#define UPSET_IMAGE
#define ALL_8_BPP
#define USE_SRC_DATA     //accelerate decoder

#define MAIN_FRE 324000



typedef short int       code_int;   

/* Various error codes used by decoder */
#define OUT_OF_MEMORY -10
#define BAD_CODE_SIZE -20
#define READ_ERROR -1
#define WRITE_ERROR -2
#define OPEN_ERROR -3
#define CREATE_ERROR -4
#define MAX_CODES   4095
#define GIFBUFTAM 16383
#define TRANSPARENCY_CODE 0xF9

//LZW GIF Image compression
#define MAXBITSCODES    12
#define HSIZE  5003     /* 80% occupancy */
#define MAXCODE(n_bits) (((code_int) 1 << (n_bits)) - 1)
#define HashTabOf(i)    htab[i]
#define CodeTabOf(i)    codetab[i]



#define CXIMAGE_DEFAULT_DPI 96
#define	CXIMAGE_MAX_MEMORY 268435456

typedef struct tagCxImageInfo {
	DWORD	dwEffWidth;			///< DWORD aligned scan line width
	BYTE*	pImage;				///< THE IMAGE BITS	

	long	nBkgndIndex;		///< used for GIF, PNG, MNG
	RGBQUAD nBkgndColor;		///< used for RGB transparency
	
	long	nFrame;				///< used for TIF, GIF, MNG : actual frame
	long	nNumFrames;			///< used for TIF, GIF, MNG : total number of frames
	DWORD	dwFrameDelay;		///< used for GIF, MNG
	long	xDPI;				///< horizontal resolution
	long	yDPI;				///< vertical resolution
	
	BOOL	last_c_isvalid;

	BYTE	dispmeth;
	BOOL	bGetAllFrames;
	BOOL	bLittleEndianHost;

} CXIMAGEINFO;

typedef struct rgb_color { unsigned char r,g,b; }rgb_color_t;


#pragma pack(1)

typedef struct tag_gifgce{
  BYTE flags;                   /*res:3|dispmeth:3|userinputflag:1|transpcolflag:1*/
  WORD delaytime;
  BYTE transpcolindex;
} struct_gifgce;

typedef struct tag_dscgif{		/* Logic Screen Descriptor  */
  unsigned char header[6];				/* Firma and version */
  WORD scrwidth;
  WORD scrheight;
  unsigned char pflds;
  unsigned char bcindx;
  unsigned char pxasrat;
} struct_dscgif;

typedef struct tag_image{      /* Image Descriptor */
  WORD l;
  WORD t;
  WORD w;
  WORD h;
  BYTE   pf;
} struct_image;

typedef struct tag_TabCol{		/* Tabla de colores */
  short colres;					/* color resolution */
  short sogct;					/* size of global color table */
  rgb_color_t paleta[256];		/* paleta */
} struct_TabCol;

#pragma pack()



typedef struct CxImageGIF
{
	void*				        m_pDib;                //contains the header, the palette, the pixels
	BITMAPINFOHEADER            m_head;                //standard header
	CXIMAGEINFO			        m_info;                //extended information

	struct_gifgce m_gifgce;

	int  m_interlaced;
	int  m_iypos;
	int  m_istep;
	int  m_iheight;
	int  m_ipass;
	int  m_ibf;
	int  m_ibfmax;

#ifdef USE_SRC_DATA
	unsigned char *m_pbuf;
#else
	unsigned char m_buf[GIFBUFTAM + 1];
#endif

	int m_curr_size;                     /* The current code size */
	int m_clear;                         /* Value for a clear code */
	int m_ending;                        /* Value for a ending code */
	int m_newcodes;                      /* First available code */
	int m_top_slot;                      /* Highest code for current size */
	int m_slot;                          /* Last read code */

	/* The following static variables are used* for seperating out codes */
	int m_navail_bytes;              /* # bytes left in block */
	int m_nbits_left;                /* # bits left in current BYTE */
	BYTE  m_b1;                           /* Current BYTE */
	BYTE  m_byte_buff[257];               /* Current block */
	BYTE *m_pbytes;                      /* Pointer to next BYTE in block */
	/* The reason we have these seperated like this instead of using
	* a structure like the original Wilhite code did, is because this
	* stuff generally produces significantly faster code when compiled...
	* This code is full of similar speedups...  (For a good book on writing
	* C for speed or for space optomisation, see Efficient C by Tom Plum,
	* published by Plum-Hall Associates...)
	*/
	BYTE m_stack[MAX_CODES + 1];            /* Stack for storing pixels */
	BYTE m_suffix[MAX_CODES + 1];           /* Suffix table */
	WORD m_prefix[MAX_CODES + 1];           /* Prefix linked list */

	char m_comment[256];
	int  m_loops;

    dec_mode_t  dec_mode;

	unsigned char * p_src_buf_start;
	unsigned char * p_src_buf_end;
	unsigned char * p_src_curpos;

	void * p_global_decline_reg;
	void * p_global_dec_param;


}CXIMAGEGIF, * gif_dec_ptr;



typedef struct stImageIterator
{
	int Itx, Ity;		// Counters
	int Stepx, Stepy;
	BYTE* IterImage;	//  Image pointer
	gif_dec_ptr ima;

	// Iterators
	BOOL (*ItOK)(struct stImageIterator * );
	void (*Reset) (struct stImageIterator *);
	void (*Upset) (struct stImageIterator *);
	void (*SetRow)(struct stImageIterator *,BYTE *buf, int n);

	BYTE (*GetByteY)(struct stImageIterator *);
	void (*SetByte)(struct stImageIterator *,BYTE b);

	BOOL (*NextRow)(struct stImageIterator *);
	BOOL (*PrevRow)(struct stImageIterator *);


	void (*SetSteps)(struct stImageIterator *,int x, int y);
	void (*GetSteps)(struct stImageIterator *,int *x, int *y);
	BOOL (*NextStep)(struct stImageIterator *);
	BOOL (*PrevStep)(struct stImageIterator *);

	void (*SetY)(struct stImageIterator *,int y);	/* AD - for interlace */
	int  (*GetY)(struct stImageIterator *);

}CIMAGEITERATOR, * image_iter_ptr;



///store decoder info
typedef struct gif_dec_struct
{
  u8 * p_linebuf ;
  u8 * p_bkgddib ;
  u8 * p_prevdib ;
  
  u32 g_firstfrm_pos;

  u32  g_iImage;
  u32  g_linesize;

  u32 g_dec_pos;
  u32 g_bTrueColor;
  u32 g_cur_dib_size;
  
  struct_TabCol g_TabCol;
  struct_dscgif g_dscgif;
  
}gif_dec_gparam_t;

typedef struct stDECLINEREG
{
	BYTE *sp;
	BYTE *bufptr;
	BYTE *buf;
	int code;
	int fc;
	int oc;
	int bufcnt;
	int c;
	int size;
	CIMAGEITERATOR iter ;
}DEC_LINE_REG_ST, *dec_linereg_ptr;



///////////////////////////////////////////////////////////////////////////////
//function declear
//////////////////////////////////////////////////////////////////////////////


void Startup(gif_dec_ptr p_gif_com);
void Init_Gif_Buf(gif_dec_ptr p_gif_com,unsigned char *,u32 size);

BOOL Decode(gif_dec_ptr p_gif_com, gif_output_t * p_dst_gif);

BOOL Get_gif_info(gif_dec_ptr p_gif_com);

BOOL Decode_line_before(gif_dec_ptr p_gif_com,gif_output_t * p_dst_gif);
int init_line_dec(gif_dec_ptr p_gif_com);
int scan_line(gif_dec_ptr p_gif_com, gif_output_t * p_dst_gif, u32 * p_dstline, u32 * p_line_num);

BOOL Save(gif_dec_ptr p_gif_com,const char* filename, DWORD imagetype);


BOOL DecodeExtension(gif_dec_ptr p_gif_com);

int init_exp(gif_dec_ptr p_gif_com,short size);

int get_next_code(gif_dec_ptr p_gif_com);

int decoder(gif_dec_ptr  p_gif_com,
		      image_iter_ptr iter,
			  u32 linewidth,
			  int *bad_code_count);

int   get_byte(gif_dec_ptr p_gif_com);

int   out_line(gif_dec_ptr p_gif_com,
		               image_iter_ptr iter,
					  unsigned char *pixels,
					  int linelen);

int   get_num_frames(gif_dec_ptr p_gif_com,
							struct_TabCol* TabColSrc,
							struct_dscgif* dscgif);

long  seek_next_image(gif_dec_ptr p_gif_com,long position);

void* Create(gif_dec_ptr p_gif_com,
					DWORD dwWidth, 
					DWORD dwHeight, 
					DWORD wBpp);

int Active_dib(gif_dec_ptr p_gif_com,DWORD dwWidth, DWORD dwHeight, DWORD wBpp,int bcur);

BOOL  Destroy(gif_dec_ptr p_gif_com);

u32	  GetSize(gif_dec_ptr p_gif_com);

BYTE* GetBits_g(gif_dec_ptr p_gif_com,DWORD row/* = 0*/);

DWORD	GetHeight(gif_dec_ptr p_gif_com) ;

DWORD	GetWidth(gif_dec_ptr p_gif_com) ;

DWORD	GetEffWidth(gif_dec_ptr p_gif_com) ;

DWORD	GetNumColors(gif_dec_ptr p_gif_com) ;

WORD	GetBpp(gif_dec_ptr p_gif_com) ;

void	SetXDPI(gif_dec_ptr p_gif_com,long dpi);
void	SetYDPI(gif_dec_ptr p_gif_com,long dpi);

long	GetTransIndex(gif_dec_ptr p_gif_com) ;
void	SetTransIndex(gif_dec_ptr p_gif_com,long idx);

void	SetDisposalMethod(gif_dec_ptr p_gif_com,BYTE dm);

DWORD	GetPaletteSize(gif_dec_ptr p_gif_com);
RGBQUAD* GetPalette(gif_dec_ptr p_gif_com);

void	SetPalettebyRGB(gif_dec_ptr p_gif_com,DWORD n, BYTE *r, BYTE *g, BYTE *b);
void	SetPalette(gif_dec_ptr p_gif_com,RGBQUAD* pPal,DWORD nColors/*=256*/);

void	SetStdPalette(gif_dec_ptr p_gif_com);

BOOL	IsInside(gif_dec_ptr p_gif_com,long x, long y);
BYTE	GetPixelIndex(gif_dec_ptr p_gif_com,long x,long y);
void	SetPixelIndex(gif_dec_ptr p_gif_com,long x,long y,BYTE i);
	
#endif
