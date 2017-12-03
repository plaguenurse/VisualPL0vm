#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "flibble.h"

// Kate Barr
// ch986510
/* 	All work herein is solely my own work, except for the base() function 
 *	which is modified from the function provided in Appendix D of the 
 *  Programming assignment.
 */

typedef struct IR
{
	short op;
	short reg;
	short lex;
	int value;
} IR; 

typedef struct color
{
	unsigned short r;
	unsigned short g;
	unsigned short b;
}color;

int base(int lex,int bp,int* stack) // l stand for L in the instruction format
{  
  int b1; //find base L levels down
  b1 = bp; 
  while (lex > 0)
  {
    b1 = stack[b1 + 1];
    lex--;
  }
  return b1;
}

color * newColor(int r, int g, int b)
{
	color * retColor = malloc(sizeof(color));
	
	retColor->r = r;
	retColor->g = g;
	retColor->b = b;
	
	return retColor;
}

int main(int argc, char **argv)
{
	const int WIDTH = 1024;
	const int HEIGHT = 768;
	
	//color table
	color ** colorTable = malloc(sizeof(color*)*16);
	
	colorTable[0] = newColor(13,13,13);
	colorTable[1] = newColor(227,9,74);
	colorTable[2] = newColor(42,42,229);
	colorTable[3] = newColor(184,42,229);
	colorTable[4] = newColor(0,181,26);
	colorTable[5] = newColor(145,145,145);
	colorTable[6] = newColor(13,161,255);
	colorTable[7] = newColor(170,170,255);
	
	colorTable[8] = newColor(85,85,0);
	colorTable[9] = newColor(242,94,0);
	colorTable[10] = newColor(192,192,192);
	colorTable[11] = newColor(241,91,184);
	colorTable[12] = newColor(54,249,6);
	colorTable[13] = newColor(252,255,27);
	colorTable[14] = newColor(98,246,153);
	colorTable[15] = newColor(240,240,240);
	
	int run = 1;
	int i;
	int x=0,y=0;
	int registers[8] = {0};
	int sp = 0;
	int bp = 1;
	int go = 0;
	int step = 0;
	int stepSpd = 16;
	int pc = 0;
	char* codenametable[22]={"LIT","RTN","LOD","STO","CAL","INC","JMP","JPC","SIO","NEG","ADD","SUB","MUL","DIV","ODD","MOD","EQL","NEQ","LSS","LEQ","GTR","GEQ"};
	int codesize = 10;
	int printCap = 7;
	int printVal = 0;
	char ** bigBuff = malloc(sizeof(char*)*printCap);
	for(i = 0;i< 18;i++)
		bigBuff[i] = malloc(sizeof(char)*40);
	int buffPos;
	IR ir;
	IR* codestack = malloc(sizeof(IR)*codesize);
	int stacksize = 200;
	int* stack = calloc(sizeof(int),sizeof(int) * stacksize);
	FILE * codefile;

	int colorValue = 0;
	
	Game * game = setup(WIDTH,HEIGHT,WIDTH,HEIGHT);

	Font * font = newFont(loadImage("font.png",11,6,game));

	Image * screen = loadImage("screen.png",1,1,game);

	SDL_Texture * screenTex = SDL_CreateTexture(game->renderer,SDL_PIXELFORMAT_RGBA5551,SDL_TEXTUREACCESS_TARGET,341,246);
	SDL_SetRenderTarget(game->renderer,screenTex);
	SDL_SetRenderDrawColor(game->renderer,0,0,0,255);
	SDL_RenderClear(game->renderer);
	SDL_SetRenderTarget(game->renderer,NULL);
	SDL_Rect screenbox = {104,72,341,246};
	SDL_Rect power = {987,2,20,20};
	
	if(argc != 2)
	{
		fprintf(stderr,"ERROR - INCORRECT INPUT.\n Correct input is pm0vm [filename]\n");
		return 1;
	}
	else
	{
		codefile = fopen(argv[1],"r");
		do
		{
			if(pc+1>=codesize)
			{
				codesize *=2;
				codestack = realloc(codestack, sizeof(IR)*codesize);
			}
			fscanf(codefile,"%hd",&(codestack[pc].op));
			fscanf(codefile,"%hd",&(codestack[pc].reg));
			fscanf(codefile,"%hd",&(codestack[pc].lex));
			fscanf(codefile,"%d",&(codestack[pc].value));
			pc++;
		}
		while(!feof(codefile));
	}
	fclose(codefile);
	pc = 0;

while(!game->chain->close)
	{
		
		getEvents(game->chain);
		if(game->chain->key[' '] && go == 0)
		{
			go = stepSpd+1;
			game->chain->key[' '] = 0;
		}
		else if(game->chain->key[' '])
		{
			go = 0;
			game->chain->key[' '] = 0;
		}
		if(game->chain->key['v'])
		{
			step = !step;
			game->chain->key['v'] = 0;
		}
		if(game->chain->key['n'] && stepSpd > 1)
		{
			stepSpd /= 2;
			game->chain->key['n'] = 0;
		}
		if(game->chain->key['m'])
		{
			stepSpd *= 2;
			game->chain->key['m'] = 0;
		}
		if(((go >= 1)|| (step == 1)) && run)
		{

			if(go == 1)
				go = stepSpd+1;
			
			step = 0;
			do
			{
				//fetch portion
				ir= codestack[pc];
				pc++;
				//execute portion
				if(ir.op == 1)
				{
					registers[ir.reg] = ir.value;
				}
				else if(ir.op == 2)
				{
					sp = bp-1;
					bp = stack[sp+3];
					pc = stack[sp+4];
				}
				else if(ir.op == 3)
				{
					registers[ir.reg] = stack[base(ir.lex,bp,stack)+ir.value-1];
				}
				else if(ir.op == 4)
				{
					if((base(ir.lex,bp,stack)+ir.value-1)>stacksize)
					{
						stacksize*=2;
						stack = realloc(stack,sizeof(int) * stacksize);
					}
					stack[base(ir.lex,bp,stack)+ir.value-1] = registers[ir.reg];
				}
				else if(ir.op == 5)
				{
					if(sp+4>stacksize)
					{
						stacksize*=2;
						stack = realloc(stack,sizeof(int) * stacksize);
					}
					stack[sp+1] = 0;
					stack[sp+2] = base(ir.lex,bp,stack);
					stack[sp+3] = bp;
					stack[sp+4] = pc;
					bp = sp + 1;
					sp = sp + 4;
					pc = ir.value;
				}
				else if(ir.op == 6)
				{
					if(sp+ir.value>stacksize)
					{
						stacksize*=2;
						stack = realloc(stack,sizeof(int) * stacksize);
					}
					sp = sp + ir.value;
				}
				else if(ir.op == 7)
				{
					pc = ir.value;
				}
				else if(ir.op == 8)
				{
					if(registers[ir.reg] == 0)
						pc = ir.value;
				}
				else if(ir.op == 9)
				{
					if(ir.value == 1)
					{
						if(printVal < printCap)
						{
							sprintf(bigBuff[printVal],"%d\n",registers[ir.reg]);
							printVal++;
						}
						else
						{
							for(i = 1; i < printVal; i++)
							{
								strcpy(bigBuff[i-1],bigBuff[i]);
							}
							sprintf(bigBuff[printVal - 1],"%d\n",registers[ir.reg]);
						}
					}
					else if(ir.value == 2)
						scanf("%d\n",&registers[ir.reg]);
					else if(ir.value == 3)
						run = 0;
					else if(ir.value == 4)
						x = registers[ir.reg];
					else if(ir.value == 5)
						y = registers[ir.reg];
					else if(ir.value == 6)
						colorValue = registers[ir.reg]%16;
					else if(ir.value == 7)
					{
						SDL_SetRenderTarget(game->renderer,screenTex);

						SDL_SetRenderDrawColor(game->renderer,colorTable[colorValue]->r,colorTable[colorValue]->g,colorTable[colorValue]->b,SDL_ALPHA_OPAQUE);
						SDL_RenderDrawPoint(game->renderer,x,y);
						SDL_SetRenderTarget(game->renderer,NULL);
						SDL_SetRenderDrawColor(game->renderer,0,0,0,0);
					}
					
				}
				else if(ir.op == 10)
				{
					registers[ir.reg] = -registers[ir.lex];
				}
				else if(ir.op == 11)
				{
					registers[ir.reg] = registers[ir.lex] + registers[ir.value];
				}
				else if(ir.op == 12)
				{
					registers[ir.reg] = registers[ir.lex] - registers[ir.value];
				}
				else if(ir.op == 13)
				{
					registers[ir.reg] = registers[ir.lex] * registers[ir.value];
				}
				else if(ir.op == 14)
				{
					registers[ir.reg] = registers[ir.lex] / registers[ir.value];
				}
				else if(ir.op == 15)
				{
					registers[ir.reg] = registers[ir.lex] % 2;
				}
				else if(ir.op == 16)
				{
					registers[ir.reg] = registers[ir.lex] % registers[ir.value];
				}
				else if(ir.op == 17)
				{
					registers[ir.reg] = (registers[ir.lex] == registers[ir.value]);
				}
				else if(ir.op == 18)
				{
					registers[ir.reg] = registers[ir.lex] != registers[ir.value];
				}
				else if(ir.op == 19)
				{
					registers[ir.reg] = registers[ir.lex] < registers[ir.value];
				}
				else if(ir.op == 20)
				{
					registers[ir.reg] = registers[ir.lex] <= registers[ir.value];
				}
				else if(ir.op == 21)
				{
					registers[ir.reg] = registers[ir.lex] > registers[ir.value];
				}
				else if(ir.op == 22)
				{
					registers[ir.reg] = registers[ir.lex] >= registers[ir.value];
				}
				if(printVal < printCap)
				{
					sprintf(bigBuff[printVal],"%d Operation:%s %hd %hd %d",pc,codenametable[ir.op-1],ir.reg,ir.lex,ir.value);
					printVal++;
				}
				else
				{
					for(i = 1; i < printVal; i++)
					{
						strcpy(bigBuff[i-1],bigBuff[i]);
					}
					sprintf(bigBuff[printVal - 1],"%d Operation:%s %hd %hd %d",pc,codenametable[ir.op-1],ir.reg,ir.lex,ir.value);
				}
				go--;
				
			}
			while(go > 1 && run);
		}
		
		if(!run)
		{
			SDL_SetRenderDrawColor(game->renderer,colorTable[1]->r,colorTable[1]->g,colorTable[1]->b,SDL_ALPHA_OPAQUE);
		}
		else if(go >= 1)
		{
			SDL_SetRenderDrawColor(game->renderer,colorTable[12]->r,colorTable[12]->g,colorTable[12]->b,SDL_ALPHA_OPAQUE);			
		}
		else
		{
			SDL_SetRenderDrawColor(game->renderer,colorTable[9]->r,colorTable[9]->g,colorTable[9]->b,SDL_ALPHA_OPAQUE);
		}

		SDL_RenderFillRect(game->renderer,&power);
		SDL_SetRenderDrawColor(game->renderer,0,0,0,0);
		SDL_RenderCopy(game->renderer,screenTex,NULL,&screenbox);
		drawImage(screen,game);
		for(i=0;i<printVal;i++)
		{
			writeWords(bigBuff[i],font,13,game, 230,498 + 39*i);
		}
		if(game->chain->window)
		{
			SDL_RenderSetScale(game->renderer,(game->chain->winheight*1.0)/(HEIGHT*1.0),(game->chain->winheight*1.0)/(HEIGHT*1.0));
			game->height = HEIGHT;
			game->width = game->chain->winwidth * ((HEIGHT*1.0)/(game->chain->winheight*1.0));
		}
		SDL_RenderPresent(game->renderer);
		SDL_RenderClear(game->renderer);

	}
	free(codestack);
	free(stack);
}
