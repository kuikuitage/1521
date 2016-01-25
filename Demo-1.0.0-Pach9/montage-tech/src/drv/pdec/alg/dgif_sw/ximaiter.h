/*
 * File:	ImaIter.h
 * Purpose:	Declaration of the Platform Independent Image Base Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra(at)servidor(dot)unam(dot)mx>
 *
 * 07/08/2001 Davide Pizzolato - www.xdp.it
 * - removed slow loops
 * - added safe checks
 *
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due.
 *
 * COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
 * THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
 * CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
 * THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
 * SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
 * PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.
 *
 * Use at your own risk!
 * ==========================================================
 */

#if !defined(__ImaIter_h)
#define __ImaIter_h

BYTE GetByte(image_iter_ptr p_iter ) { return p_iter->IterImage[p_iter->Itx]; }
void SetByte(image_iter_ptr p_iter,BYTE b) { p_iter->IterImage[p_iter->Itx] = b; }
void SetSteps(image_iter_ptr p_iter,int x, int y /*=0*/) {  p_iter->Stepx = x; p_iter->Stepy = y; }
int  GetY(image_iter_ptr p_iter) {return p_iter->Ity;}

BOOL ItOK (image_iter_ptr p_iter)
{
	if (p_iter->ima) return IsInside(p_iter->ima,p_iter->Itx, p_iter->Ity);
	else	 return FALSE;
}

/////////////////////////////////////////////////////////////////////
void Reset(image_iter_ptr p_iter)
{
	if (p_iter->ima) p_iter->IterImage = GetBits_g(p_iter->ima, 0);
	else	 p_iter->IterImage=0;
	p_iter->Itx = p_iter->Ity = 0;
}
/////////////////////////////////////////////////////////////////////
void Upset(image_iter_ptr p_iter)
{
	p_iter->Itx = 0;
	p_iter->Ity = GetHeight(p_iter->ima)-1;
	p_iter->IterImage = GetBits_g(p_iter->ima,0) + GetEffWidth(p_iter->ima)*(GetHeight(p_iter->ima)-1);
}
/////////////////////////////////////////////////////////////////////
BOOL NextRow(image_iter_ptr p_iter)
{
	if (++(p_iter->Ity) >= (int)GetHeight(p_iter->ima)) return 0;
	p_iter->IterImage += GetEffWidth(p_iter->ima);
	return 1;
}
/////////////////////////////////////////////////////////////////////
BOOL PrevRow(image_iter_ptr p_iter)
{
	if (--(p_iter->Ity) < 0) return 0;
	p_iter->IterImage -= GetEffWidth(p_iter->ima);
	return 1;
}
/* AD - for interlace */
void SetY(image_iter_ptr p_iter,int y)
{
	if ((y < 0) || (y > (int)GetHeight(p_iter->ima))) return;
	p_iter->Ity = y;
	p_iter->IterImage = GetBits_g(p_iter->ima,0) + GetEffWidth(p_iter->ima)*y;
}
/////////////////////////////////////////////////////////////////////
void SetRow(image_iter_ptr p_iter,BYTE *buf, int n)
{
	if (n<0) n = (int)GetEffWidth(p_iter->ima);
	else n = min(n,(int)GetEffWidth(p_iter->ima));

	if ((p_iter->IterImage!=NULL)&&(buf!=NULL)&&(n>0))
		memcpy(p_iter->IterImage,buf,n);
}


void init_ImageIterator ( gif_dec_ptr image ,image_iter_ptr p_iter)
{
	p_iter->ima = image;
	if (p_iter->ima) p_iter->IterImage = GetBits_g(p_iter->ima,0);
	p_iter->Itx = p_iter->Ity = 0;
	p_iter->Stepx = p_iter->Stepy = 0;
  ///function asigned

	p_iter->ItOK = ItOK;
	p_iter->Reset = Reset;
	p_iter->Upset = Upset;
	p_iter->SetRow = SetRow;
	
	p_iter->SetByte = SetByte;

	p_iter->NextRow = NextRow;
	p_iter->PrevRow = PrevRow;

	p_iter->SetSteps = SetSteps;

	p_iter->SetY = SetY;	/* AD - for interlace */
	p_iter->GetY = GetY;
}


#endif
