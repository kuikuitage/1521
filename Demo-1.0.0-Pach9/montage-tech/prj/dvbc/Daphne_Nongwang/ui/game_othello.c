/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "game_othello.h"

u8   computer_side = CHESS_BLACK;
u8   max_depth = 4;

u8   cur_depth = 0;


u8   cur_step =0;
u16   step_array[64];

u8   g_iGameLevel = GMLEVEL_MID; //��Ϸ�Ѷȵȼ�
const  u8 depth1[]={5,6,7};//{6, 7, 8};
const  u8 depth2[]={4,5,6};//{5, 6, 7};

/*�ҳ�������ˮƽ�����ܱ�����obcolor�������ӣ����ۼƷ���*/
s16 scan_horiz_aixes(board_type *board_ptr, u8 obcolor)
{
   /*ɨ��8��ˮƽ����*/
	s16 score=0;
	u8 *cur_ptr, *stop_ptr;
	u8  piece[4][2];
	u8 count=0, tmpscore;
	u8 bFull;
	u8 row;
	u8 nums;
	for(row=1; row<9; row++)
	{
       	tmpscore = (1 == row ||8 == row) ? 10:2;
		cur_ptr = &board_ptr->board[row][1];
		stop_ptr= &board_ptr->board[row][9];
		bFull = TRUE;
		count=0;
		while(cur_ptr < stop_ptr)
		{
		   if(*cur_ptr == obcolor)
		   {
			 piece[count][0]  = cur_ptr - &board_ptr->board[row][0];
			 while(*cur_ptr == obcolor)
				 cur_ptr ++;
			 piece[count++][1] = cur_ptr - &board_ptr->board[row][0];
		   }
		   if(!*cur_ptr)
			   bFull = FALSE; 
		   cur_ptr++;
		}
		while(count--)
		{
			nums = (piece[count][1]-piece[count][0]);
			if(bFull || piece[count][0]==1 || piece[count][1] == 9)
				score += nums;
			if(piece[count][0]==1 || piece[count][1] == 9)
				score += tmpscore;
			else if(!bFull && (piece[count][0] == 2 || piece[count][1] == 8) && (row == 1 || row == 8))
				score -= tmpscore;
		}
	}

	 return score;
}

//arithmetic
/*�ҳ������ڴ�ֱ�����ܱ�����obcolor�������ӣ����ۼƷ���*/
s16 scan_vertical_aixes(board_type *board_ptr, u8 obcolor)
{
	s16 score=0;
	u8 *cur_ptr, *stop_ptr;
	u8  piece[4][2];
	u8 count=0, tmpscore;
	u8 bFull;
	u8 col;
	u8 nums;
	for(col=1; col<9; col++)
	{
        tmpscore = (col == 1 || col == 8) ? 10:2;
		cur_ptr = &board_ptr->board[1][col];
		stop_ptr= &board_ptr->board[9][col];
		bFull = TRUE;
		count=0;
		while(cur_ptr < stop_ptr)
		{
		   if(*cur_ptr == obcolor)
		   {
			 piece[count][0]  = (cur_ptr - &board_ptr->board[0][col])/10;
			 while(*cur_ptr == obcolor)
				 cur_ptr += 10;
			 piece[count++][1] = (cur_ptr - &board_ptr->board[0][col])/10;
		   }
		   if(!*cur_ptr)
			   bFull = FALSE;
		   cur_ptr += 10;
		}
		while(count--)
		{
			nums = (piece[count][1]-piece[count][0]);
			if(bFull || piece[count][0]==1 || piece[count][1] == 9)
				score += nums;
			if(piece[count][0]==1 || piece[count][1] == 9)
				score += tmpscore;
			else if(!bFull && (piece[count][0] == 2 || piece[count][1] == 8) && (col == 1 || col == 8))
				score -= (tmpscore<<1);
		}
	}
   	return score;
}

/*�ҳ����������ϵ����·����ܱ�����obcolor�������ӣ����ۼƷ���*/
s16 scan_fd_aixes(board_type *board_ptr, u8 obcolor)
{
	s16 score =0;
    u8 *cur_ptr, *stop_ptr, *base_ptr;
	u8  piece[4][2];
	u8 count=0, tmpscore;
	u8 bFull;
	s8 aixes;
	u8 nums;
	s8  toborder;
	for(aixes = -5; aixes <= 5; aixes++)
	{
        tmpscore = (aixes == 0) ? 10:2;
		if(aixes <=0)
		{
		 base_ptr = cur_ptr = &board_ptr->board[1][8+aixes];
		 stop_ptr = &board_ptr->board[9+aixes][0];
		}
		else
		{
         base_ptr = cur_ptr = &board_ptr->board[aixes+1][8];
		 stop_ptr= &board_ptr->board[9][aixes];
		}
		bFull = TRUE;
		count=0;
		while(cur_ptr < stop_ptr)
		{
		   if(*cur_ptr == obcolor)
		   {
			 piece[count][0]  = cur_ptr - board_ptr->board[0];
			 while(*cur_ptr == obcolor)
				 cur_ptr += 9;
			 piece[count++][1] = cur_ptr- board_ptr->board[0];
		   }
		   if(!*cur_ptr)
			   bFull = FALSE;
		   cur_ptr += 9;
		}
		while(count--)
		{
			nums = (piece[count][1]-piece[count][0])/9;
			toborder = (piece[count][0] == base_ptr - board_ptr->board[0] || 
				                  piece[count][1] == stop_ptr - board_ptr->board[0]);
			if(bFull || toborder)
				score += nums;

			if((aixes == 1 || aixes == -1) && toborder)
			    score -= tmpscore;
			/*���������嵽��߽�*/
			else if(toborder)
				score += tmpscore;
			/*��������ڽǱ��ϣ���۷�*/
			else if(!bFull && (piece[count][0] == 27 ||
				                   piece[count][1] == 81))
				score -= (tmpscore<<1);
		}
	}

	/*����Ǳ������ӣ��۷�*/
	if(board_ptr->board[1][1] == obcolor)
			score += 10;
	else 
	{
		if(board_ptr->board[1][2] == obcolor)
			 score -=2;
		if(board_ptr->board[2][1] == obcolor)
			 score -=2;
		if(board_ptr->board[2][2]== obcolor)
		 score -=2;
	}
     
	if(board_ptr->board[8][8] == obcolor)
		 score +=10;
	else
	{
		if(board_ptr->board[7][8]  == obcolor)
			 score -=2;
		if(board_ptr->board[8][7]== obcolor)
			 score -=2;
		if(board_ptr->board[7][7]== obcolor)
			score -= 2;
	}    
    return score;
}
/*�ҳ����������ϵ����·����ܱ�����obcolor�������ӣ����ۼƷ���*/
s16 scan_bd_aixes(board_type *board_ptr,  u8 obcolor)
{
	
    s16 score =0;
    u8 *cur_ptr, *stop_ptr, *base_ptr;
	u8  piece[4][2];
	u8 count=0, tmpscore;
	u8 bFull;
	s8 aixes;
	u8 nums;
	s8  toborder;
	for(aixes = -5; aixes <= 5; aixes++)
	{
        tmpscore = (aixes == 0) ? 10:2;
		if(aixes <=0)
		{
		 base_ptr = cur_ptr = &board_ptr->board[1-aixes][1];
		 stop_ptr = &board_ptr->board[9][9+aixes];
		}
		else
		{
         base_ptr = cur_ptr = &board_ptr->board[1][aixes+1];
		 stop_ptr= &board_ptr->board[9-aixes][9];
		}
		bFull = TRUE;
		count=0;
		while(cur_ptr < stop_ptr)
		{
		   if(*cur_ptr == obcolor)
		   {
			 piece[count][0]  = cur_ptr - board_ptr->board[0];
			 while(*cur_ptr == obcolor)
				 cur_ptr += 11;
			 piece[count++][1] = cur_ptr- board_ptr->board[0];
		   }
		   if(!*cur_ptr)
			   bFull = FALSE;
		   cur_ptr += 11;
		} 
		while(count--)
		{
			nums = (piece[count][1]-piece[count][0])/11;
			toborder = (piece[count][0] == base_ptr - board_ptr->board[0] || 
				                  piece[count][1] == stop_ptr - board_ptr->board[0]);
			if(bFull || toborder)
				score += nums;
			/*����Ǳ������ӣ��۷�*/
			if((aixes == 1 || aixes == -1) && toborder)
			    score -= tmpscore;
			/*���������嵽��߽�*/
			else if(toborder)
				score += tmpscore;
			/*��������ڽǱ��ϣ���۷�, ���Խ��߷���*/
			else if(!bFull && (piece[count][0] == 22 ||
				                   piece[count][1] == 88))
				score -= (tmpscore<<1);
		}
	}

	/*����Ǳ������ӣ��۷�*/
	if(board_ptr->board[1][8] == obcolor)
			score += 10;
	else 
	{
		if(board_ptr->board[1][7] == obcolor)
			 score -=2;
		if(board_ptr->board[2][8] == obcolor)
			 score -=2;
		if(board_ptr->board[2][7]== obcolor)
		 score -=2;
	}
     
	if(board_ptr->board[8][1] == obcolor)
		 score +=10;
	else
	{
		if(board_ptr->board[7][1]  == obcolor)
			 score -=2;
		if(board_ptr->board[8][2]== obcolor)
			 score -=2;
		if(board_ptr->board[7][2]== obcolor)
			score -= 2;
	} 
    return score;
}

s16 sample_calc_board_status(board_type *board_ptr, u8 obcolor)
{
	s16 score=0;
	u8 *ptr = &board_ptr->board[1][1];
	u8 *stop = &board_ptr->board[8][9]; 
	u8 tmpcol = ~obcolor &0x03;
	while(ptr<stop)
	{
		if(*ptr == obcolor)
			score++;
		else if(*ptr == tmpcol)
			score--;
		ptr++;
	}
	return score;
}

/*�������board_ptr��״̬��*/
s16 calc_board_status(board_type *board_ptr, u8 obcolor)
{
    s16 score=0;
	u8 tmpcol = ~obcolor & 0x03 ;
    score += scan_horiz_aixes(board_ptr,  obcolor);
    score += scan_vertical_aixes(board_ptr, obcolor);
	score += scan_bd_aixes(board_ptr, obcolor);
	score += scan_fd_aixes(board_ptr, obcolor);
	if(board_ptr->board[1][1] == tmpcol)
		score -= 44;
	if(board_ptr->board[8][8] == tmpcol)
		score -= 44;
	if(board_ptr->board[1][8] == tmpcol)
        score -= 44;
	if(board_ptr->board[8][1] == tmpcol)
		score -= 44;
	return score;
}

/*��start_pos�����ҵ�һ�������ӵĵ㣬������Ӱ����ӵĸ�����
                affected_list�����Ӱ�������ָ��,��һ��ָ��Ϊ���ӵĵ�*/
const s16 delta_array[8] = {-11, 11, -9, 9, -1, 1, -10, 10};
u16 find_move(board_type *board_ptr, u16 start_pos, 
			               u8 obcolor, u16 *affected_list)
{
  	u8 *cel_ptr = board_ptr->board[0] + start_pos;
  	u8 *stop_ptr = &board_ptr->board[8][9], *p;
  	u16 *aff_ptr = affected_list+1, *hold_aff;
  	u8 aixes;
  	u8 thithercolor = THITHER_COLOR(obcolor);
  
  	while(1)
  	{
		/*�ҵ�һ���ո���*/
	  	while(*cel_ptr) 
		  	if(++cel_ptr>=stop_ptr)
			  	return 0;
	  	/*�����8���������Ƿ��ܳԵ��Է������ӣ�����¼���Ե����ӵ��±�*/
      		for(aixes =0;aixes<8; aixes++)
		{
			hold_aff = aff_ptr;
			p = cel_ptr + delta_array[aixes];
         		while(*p == thithercolor)
			{
				*aff_ptr++ = p - board_ptr->board[0];
				p+= delta_array[aixes];
			}
			if(*p != obcolor)
				aff_ptr = hold_aff;
	  	}
     		/*���cel_ptr��Ӧ�ĵ���ԳԵ��Է�����*/
     		if(aff_ptr - affected_list > 1) 
	 	{
         		*affected_list = cel_ptr - board_ptr->board[0];
         		return (aff_ptr - affected_list);
	 	}
	 	cel_ptr++;
  	}
}
  
void init_board(board_type *board_ptr, BOOL color)
{
	int i;
	memset(board_ptr, 0, sizeof(board_type));
	
	/*init boarder*/
	memset(board_ptr->board[0], 0xff, 10);
	memset(board_ptr->board[9], 0xff, 10);
	for(i=0; i<9; i++)
	{
		board_ptr->board[i][0] = board_ptr->board[i][9] =0xff;
		board_ptr->board[0][i] = board_ptr->board[9][i] =0xff;		
	}

	/*init chess*/
	board_ptr->board[4][4] = board_ptr->board[5][5] = CHESS_WHITE;
	board_ptr->board[4][5] = board_ptr->board[5][4] = CHESS_BLACK;
	cur_step = 0;
	
	computer_side = (color?CHESS_WHITE:CHESS_BLACK);
}   

/*�����̵�һ��״̬��������չ�˽�㣬�����ش˽��Ĳ��ֻ���ֵ*/
void extend_node_one(tree_node_type *node_ptr, tree_node_type *parent_ptr,u8 obcolor)
{
   tree_node_type childnode;
   u16 affected_list[MAX_AFFECTED_PIECES];
   u16 start_pos = 11, num;
   u8 depth;
   
   num = find_move(&node_ptr->board, start_pos, obcolor, affected_list);
   /*������վ�״̬���򷵻�״̬��ֵ������ֵ*/
   if(++cur_depth == max_depth || num==0 )
   {
	   /*����ѷ�PASS��û����ֽ���,Ҫ�۷�*/
	   node_ptr->value = calc_board_status(&node_ptr->board, computer_side);
	   if(!num)
	   {
		   /*���˫����û����*/
		   if(!find_move(&node_ptr->board, 11, (u8)(~obcolor&0x03), affected_list)) 
              return;

           if(obcolor == computer_side)
		   {
			   node_ptr->value -= 15;
			   return ;
		   }
		   node_ptr->value += 15;
	   }
      return;		   
   }
      /*��ʼ������ֵ*/ 
   node_ptr->value = (obcolor == computer_side)? -INITIAL_VALUE : INITIAL_VALUE;
   memcpy(&childnode.board, &node_ptr->board, sizeof(board_type));
   while(num)
   {
     while(num--)
		 childnode.board.board[0][affected_list[num]] = obcolor;
	 /*�ݹ���㲿�ֻ���ֵ*/
	 depth = cur_depth;
     extend_node_one(&childnode, node_ptr, (u8)((~obcolor)&0x03));
	 cur_depth = depth;
	 /*����˽��������һ�����򲿷ֻ���ֵ���ӽ��������һ��*/
	 if(obcolor == computer_side)
	 {
	   if(childnode.value > node_ptr->value)
	   {
		   node_ptr->value = childnode.value; 
		   node_ptr->movepos = affected_list[0];
	   }
	 }
	 /*����Ƕ���һ�������ֻ���ֵ���ӽ������С��һ��*/ 
	 else
	 {
		if(childnode.value < node_ptr->value)
		{
			node_ptr->value = childnode.value;
			node_ptr->movepos = affected_list[0];
		}
   	 }
	 /* ��-�²ü����ж�   �ڿ����ֵ����������һ���׽ڵ㼰�ֵ����������һ���ӽڵ�ʱ��
		  ������ӽڵ����ֵ�Ѿ�С�ڻ�������׽ڵ�Ļ���ֵ��
		  ��ô�Ͳ���Ҫ�Ըýڵ����������ڵ�������Ĵ����ˡ�
		  ����Ĺ��̿���ֱ�ӷ��ص��׽ڵ��ϡ�
	 */
	   /*�ڿ����ֵ����������һ���׽ڵ㼰�ֵ����������һ���ӽڵ�ʱ��
			  ������ӽڵ�Ĳ��ֻ���ֵ�Ѿ����ڻ�������׽ڵ�Ĳ��ֻ���ֵ��
			  ��ô�Ͳ���Ҫ�Ը��ӽڵ���������ڵ�������Ĵ����ˡ�
			  ������̿���ֱ�ӷ��ص��׽ڵ��ϡ�*/
	 if(parent_ptr)
	 {
		 if(obcolor != computer_side)
		 {
			 /*���ü�*/
			 if(node_ptr->value <= parent_ptr->value)
			  return;
		 }
		 else
		 {
			 /*�²ü�*/
			 if(node_ptr->value >= parent_ptr->value)
				return;
		 }
	 }
   	 /*�ҵ���һ�������ӵĵ�*/
	 start_pos = affected_list[0]+1;
	 memcpy(&childnode.board, &node_ptr->board, sizeof(board_type));
	 num = find_move(&childnode.board, start_pos, obcolor, affected_list);
   }
   return;
}


void extend_node_two(tree_node_type *node_ptr, tree_node_type *parent_ptr,u8 obcolor)
{
   tree_node_type childnode;
   u16 affected_list[MAX_AFFECTED_PIECES];
   u16 start_pos = 11, num;
   u8 depth;
   
   num = find_move(&node_ptr->board, start_pos, obcolor, affected_list);
   /*������վ�״̬���򷵻�״̬��ֵ������ֵ*/
   if(!num)
   {
	   /*����ѷ�PASS��û����ֽ���,Ҫ�۷�*/
	   node_ptr->value = sample_calc_board_status(&node_ptr->board, computer_side);
	   /*���˫����û����*/
	    if(!find_move(&node_ptr->board, 11, (u8)(~obcolor&0x03), affected_list)) 
             return;

        if(obcolor == computer_side)
		{
			node_ptr->value -= 10;
	        return;
		}
    	node_ptr->value += 10;
	    return;
   }
      /*��ʼ������ֵ*/ 
   node_ptr->value = (obcolor == computer_side)? -INITIAL_VALUE : INITIAL_VALUE;
   memcpy(&childnode.board, &node_ptr->board, sizeof(board_type));
   while(num)
   {
     while(num--)
		 childnode.board.board[0][affected_list[num]] = obcolor;
	 /*�ݹ���㲿�ֻ���ֵ*/
	 depth = cur_depth;
     extend_node_two(&childnode, node_ptr, (u8)((~obcolor)&0x03));
	 cur_depth = depth;
	 /*����˽��������һ�����򲿷ֻ���ֵ���ӽ��������һ��*/
	 if(obcolor == computer_side)
	 {
	   if(childnode.value > node_ptr->value)
	   {
		   node_ptr->value = childnode.value; 
		   node_ptr->movepos = affected_list[0];
	   }
	 }
	 /*����Ƕ���һ�������ֻ���ֵ���ӽ������С��һ��*/ 
	 else
	 {
		if(childnode.value < node_ptr->value)
		{
			node_ptr->value = childnode.value;
			node_ptr->movepos = affected_list[0];
		}
   	 }
	 /* ��-�²ü����ж�   �ڿ����ֵ����������һ���׽ڵ㼰�ֵ����������һ���ӽڵ�ʱ��
		  ������ӽڵ����ֵ�Ѿ�С�ڻ�������׽ڵ�Ļ���ֵ��
		  ��ô�Ͳ���Ҫ�Ըýڵ����������ڵ�������Ĵ����ˡ�
		  ����Ĺ��̿���ֱ�ӷ��ص��׽ڵ��ϡ�
	 */
	   /*�ڿ����ֵ����������һ���׽ڵ㼰�ֵ����������һ���ӽڵ�ʱ��
			  ������ӽڵ�Ĳ��ֻ���ֵ�Ѿ����ڻ�������׽ڵ�Ĳ��ֻ���ֵ��
			  ��ô�Ͳ���Ҫ�Ը��ӽڵ���������ڵ�������Ĵ����ˡ�
			  ������̿���ֱ�ӷ��ص��׽ڵ��ϡ�*/
	 if(parent_ptr)
	 {
		 if(obcolor != computer_side)
		 {
			 /*���ü�*/
			 if(node_ptr->value <= parent_ptr->value)
			  return;
		 }
		 else
		 {
			 /*�²ü�*/
			 if(node_ptr->value >= parent_ptr->value)
				return ;
		 }
	 }
   	 /*�ҵ���һ�������ӵĵ�*/
	 start_pos = affected_list[0]+1;
	 memcpy(&childnode.board, &node_ptr->board, sizeof(board_type));
	 num = find_move(&childnode.board, start_pos, obcolor, affected_list);
   }
   return;
}

void get_chess_score(board_type *board_ptr, u16 *iWscore, u16 *iBscore)
{
	u16 i;
	u16 j;
	*iWscore =0; *iBscore =0;
	for(i=1; i<=BOARD_ROWS; i++)
		for(j=1; j<=BOARD_COLS; j++)
		{
			if(board_ptr->board[i][j]  == CHESS_BLACK)
				(*iBscore)++;
			else  if(board_ptr->board[i][j] == CHESS_WHITE)
				(*iWscore)++;
		}
}

void game_over(board_type *board_ptr, othello_callback callback)
{
	  u16 wscore, bscore;
	  u8 winner;
	  get_chess_score(board_ptr, &wscore, &bscore);
	  if(computer_side == CHESS_WHITE)
	  {
		  if(wscore > bscore)
			winner = COMPUTER_WIN;
		  else if(wscore <bscore)
			winner = USER_WIN;
		  else winner = DOGFALL;
	  }
	  else
	  {
		  if(wscore > bscore)
			winner = USER_WIN;
		  else if(wscore <bscore)
		    winner = COMPUTER_WIN;
		  else winner = DOGFALL;
	  }
	  callback(GAME_OVER, (u32)winner, 0);
}
 
BOOL computer_play(board_type *board_ptr, othello_callback callback)
{
  tree_node_type node;
  u16 affected_list[MAX_AFFECTED_PIECES];

  cur_depth =0;
  
  start:
    
  memcpy(&node.board, board_ptr, sizeof(board_type));
  node.movepos =0;
  
  if(cur_step>= STEP_MONMENT2)
  {
    extend_node_two(&node, NULL, computer_side);
  }
  else if(cur_step > STEP_MONMENT1)
  {
    max_depth = depth2[g_iGameLevel];
    extend_node_one(&node, NULL, computer_side);
  }
  else 
  {
    max_depth = depth1[g_iGameLevel];
    extend_node_one(&node, NULL, computer_side);
  }

  if(!do_move_chess(board_ptr, node.movepos, computer_side, callback))
  {
    if(!find_move(board_ptr, 11, (u8)((~computer_side)&0x03), affected_list))
    {
      return FALSE;//game over
    }
    else
    {
      callback(COMPUTER_NO_STEP, 0, 0);
      return TRUE;
    }
  }
  else
  { 
    if(!find_move(board_ptr, 11, (u8)((~computer_side)&0x03), affected_list))
    {
      if(!find_move(board_ptr, 11, computer_side, affected_list))
      {
        return FALSE;//game over
      }
      else
      {
        callback(USER_NO_STEP, 0, 0);
             
        goto start;
      }
    }
  }

  return TRUE;
} 

u8 do_move_chess(board_type *board_ptr, u16 movepos, u8 obcolor, othello_callback callback)
{
  u16 affected_list[MAX_AFFECTED_PIECES];
  int i;
  u16 num;

  num = find_move(board_ptr, movepos, obcolor, affected_list);
  if(!num || affected_list[0] != movepos)
	  return 0; 
  for(i=0; i<num; i++)
  {
     board_ptr->board[0][affected_list[i]] = obcolor;
	 if(callback)
	 {
		 callback(TRANCHESS, (u32)(affected_list[i]), (u32)(i<<16|obcolor));
	 }
  }
  step_array[cur_step++] = movepos;

  return 1;
}


