/* Super Sales Acer! - for PCC - link with PCIO */

#include <stdio.h>
#include "mikes.c"

#define SOLID 0x100
#define ANIM_SP 3

char tp[768],gp[768];  /* temp palette and game palette */
char lev[12][21];      /* ascii version of level */

/* space for shape tables */

char  man[10300],  /* player character */
     enemy[4680],  /* enemies */
      tile[3640],  /* ground tiles */
      wide[1560],  /* 2x1 shapes */
      door[1030],  /* buffer for door */
     dbody[2310],  /* dragon body shape */
    dhead[10760],  /* dragon head */
      blank[260],  /* blank space */
        line[16],  /* black horizontal line */
    manbuf[1030],  /* buffer for character */
     enbuf[5200],  /* buffer for enemies */
   widebuf[1560];  /* buffer for wide shapes */

/* total= 40526 */

int anf,ans; /* animation frame, speed */
int a,px,py,x,y,z,quit,level,lives,s,xd,yd;
int cheat,CGA,GAME_SP,SLOW,JUMP;
int levcnt,levone,elev,score;
int textsp;

/* enemy x,y,type,xdir,ydir,shape/status */
int ex[20],ey[20],et[20],exd[20],eyd[20],es[20];

int numen,elevnum;

main(argc,argv) int argc; char *argv[];
{  int k;
puts("\nInitializing...\n");
CGA=0;
SLOW=0;

if (*argv[1]=='?')
{
  puts("\nUse ln (0-9) to start on level n\n");
  puts("\nUse 0n (0-?) for slowdown (hi=slower, 2=norm)\n");
  exit(0);
}
if (*argv[1]=='c')
{ CGA=1;
  puts("(CGA will take longer to initialize...)\n");
}
if (*argv[1]=='s')
{ CGA=1;
  SLOW=1;
  puts("SLOW MODE - graphics will appear distorted.\n");
}
levone=1;
if (*argv[1]=='l')
{ levone=(*(argv[1]+1)-48);
  printf("Starting level %d\n",levone);
}

mike_init();

GAME_SP=speed<<2;
if (*argv[1]=='0')
  GAME_SP=speed<<(atoi(argv[1]));
if (SLOW)
  GAME_SP=1;

if (CGA)
  set_cga();
else
  set_vga();
NO_BLACK=1;  /* don't allow black in CGA conversion */
load_bmp("grid.bmp");
for (x=0; x<768; x++) 
  gp[x]=tp[x];  /* save game palette */
getdat();
quit=1;
NO_BLACK=0;  /* allow black for CGA again */
while (quit)
{ if (SLOW)
    cls();
  else
    load_bmp("title.bmp");
  fade_in(2);

  while ((k=scr_csts())==0);
  if (k!=27)
  { fade_out(2);
    cls();   
    for (x=0; x<768; x++)
      tp[x]=gp[x];
    fade_in(10);
    game();
  } else quit=0;
}
set_txt();
printf("\nSuper Sales Acer - (c) 1995-99 by Mike Brent  [P]dec99\n");
printf("Game program, name, concepts and visuals are protected by\n");
printf("copyright. All rights not expressly licensed are retained\n");
printf("by the author.\n\n");
printf("*Yawn* Now that that's over....\n\n");
printf("Hi!! My first PC release, so it WILL get better. :) Read the\n");
printf("doc file for details, but, I gotta tell you that this game is\n");
printf("umm.... ???Ware! If you like it at all, send me something!\n");
printf("Be it money, gift certificates, coupons, notes, postcards,\n");
printf("written promises to buy me a Coca-Cola someday, dolphins\n");
printf("(I love dolphins!), bridges, cars, what-have-you! :) I am at:\n");
printf("\nM.Brent (Tursi)\n");
printf("Release - Dec 95, Patched Jan 99 for faster computers :)\n");
}

fail(x) char *x;
{ set_txt();
  puts(x);
  exit(5);
}

load_bmp(n) char *n;
{ /* load a 320x200x8 BMP file onto the screen. Load palette, but don't
     set it */
FILE *fp;
int x,y;
int r,g,b;

fp=fopen(n,"rb");

if (fp==NULL) fail("cannot open");

for (x=0; x<256; x++)
  colour(x,0,0,0);   /* black palette */

for (x=0; x<54; x++) 
  fgetc(fp);         /* skip over the header */

y=0;
for (x=0; x<256; x++)
{ /* read palette */
  b=fgetc(fp)>>2;
  g=fgetc(fp)>>2;  /* shifted for VGA constraints */
  r=fgetc(fp)>>2;
  fgetc(fp);
  tp[y++]=(char)r;
  tp[y++]=(char)g;
  tp[y++]=(char)b;
}

if (CGA)
{ for (y=0; y<768; y++)
    pal[y]=tp[y];
  pal[0]=0;
  pal[1]=0;
  pal[2]=0;  /* black background */
  /* need palette to dither pic */
  
  for (y=199; y>=0; y--)
    for (x=0; x<80; x++)
    { r=(char)((bits(fgetc(fp),y)<<6)|(bits(fgetc(fp),y)<<4)|
               (bits(fgetc(fp),y)<<2)|(bits(fgetc(fp),y)));
      _poke(r,line_table[y]+x,scrn_base);
    }
} else
{ for (y=199; y>=0; y--)
  { /* read lines */
    for (x=0; x<320; x++)
      /* read one line into screen buffer */
      _poke(fgetc(fp),line_table[y]+x,scrn_base);
  }
}

/* all done */
fclose(fp);
}

fade_in(x) int x;
{ /* fade palette in tp[] to pal[], speed x */
int y,z,r,g,b;

if (CGA) return;

for (y=63; y>=0; y--)
{ for (z=0; z<256; z++)
  { r=tp[z*3]-y;
    g=tp[z*3+1]-y;
    b=tp[z*3+2]-y;
    if (r<0) r=0;
    if (g<0) g=0;
    if (b<0) b=0;
    colour(z,r,g,b);
  }
  delay(speed/x);
}
}

fade_out(x) int x;
{ /* fade palette out... tp[] must be equal to pal[], speed x */
int y,z,r,g,b;

if (CGA) return;

for (y=1; y<64; y++)
{ for (z=0; z<256; z++)
  { r=tp[z*3]-y;
    g=tp[z*3+1]-y;
    b=tp[z*3+2]-y;
    if (r<0) r=0;
    if (g<0) g=0;
    if (b<0) b=0;
    colour(z,r,g,b);
  }
  delay(speed/x);
}
}

getdat()
{ /* read shape data from grid onscreen */
get(&man[0],0,0,32,32);
get(&man[1030],32,0,32,32);
get(&man[2060],64,0,32,32);
get(&man[3090],96,0,32,32);
get(&man[4120],0,32,32,32);
get(&man[5150],32,32,32,32);
get(&man[6180],0,64,32,32);
get(&man[7210],32,64,32,32);
get(&man[8240],64,64,32,32);
get(&man[9270],96,64,32,32);

/* get(&enemy[0],64,32,16,16);  (cat cage) */
get(&enemy[260],80,32,16,16);
get(&enemy[520],96,32,16,16);
get(&enemy[780],112,32,16,16);
get(&enemy[1040],128,32,16,16);
get(&enemy[1300],64,48,16,16);
get(&enemy[1560],80,48,16,16);
get(&enemy[1820],96,48,16,16);
get(&enemy[2080],112,48,16,16);
get(&enemy[2340],64,96,16,16);
get(&enemy[2600],80,96,16,16);
get(&enemy[2860],96,96,16,16);
get(&enemy[3120],112,96,16,16);
get(&enemy[3380],0,112,16,16);
get(&enemy[3640],16,112,16,16);
get(&enemy[3900],32,112,16,16);
get(&enemy[4160],48,112,16,16);
get(&enemy[4420],64,112,16,16);

get(&tile[0],128,0,16,16);
get(&tile[260],144,0,16,16);
get(&tile[520],160,0,16,16);
get(&tile[780],176,0,16,16);
get(&tile[1040],192,0,16,16);
get(&tile[1300],208,0,16,16);
get(&tile[1560],256,0,16,16);
get(&tile[1820],272,0,16,16);
get(&tile[2080],256,16,16,16);
get(&tile[2340],272,16,16,16);
get(&tile[2600],256,32,16,16);
get(&tile[2860],272,32,16,16);
/* get(&tile[3120],128,16,16,16); (cat meow... not used) */
get(&tile[3380],304,0,16,16);

get(&wide[0],224,0,16,32);
get(&wide[520],0,96,16,32);
get(&wide[1040],32,96,16,32);

get(dbody,144,96,48,48);
get(dhead,208,64,96,112);

get(door,0,128,32,32);

if (CGA)
{
  if (SLOW==0)
  { makemask(&man[0]);
    makemask(&man[1030]);
    makemask(&man[2060]);
    makemask(&man[3090]);
    makemask(&man[4120]);
    makemask(&man[5150]);
    makemask(&man[6180]);
    makemask(&man[7210]);
    makemask(&man[8240]);
    makemask(&man[9270]);

    /* makemask(&enemy[0]); (cat cage) */
    makemask(&enemy[260]);
    makemask(&enemy[780]);
    makemask(&enemy[1040]);
    makemask(&enemy[1300]);
    makemask(&enemy[1560]);
    makemask(&enemy[1820]);
    makemask(&enemy[2080]);
    makemask(&enemy[2340]);
    makemask(&enemy[2600]);
    makemask(&enemy[2860]);
    makemask(&enemy[3120]);
    makemask(&enemy[3380]);
    makemask(&enemy[3640]);
    makemask(&enemy[3900]);
    makemask(&enemy[4160]);
    makemask(&enemy[4420]);

    makemask(&wide[520]);
    makemask(&wide[1040]);
 
    makemask(dbody);
    makemask(dhead);
  }

  for (z=0; z<260; z++)
    blank[z]=0;
  for (z=0; z<14; z++)
    line[z]=0xff;
}
else
{ for (z=0; z<260; z++)
    blank[z]=0;
  for (z=0; z<14; z++)
    line[z]=1;
}

blank[0]=tile[0];
blank[1]=tile[1]; 
line[0]=1;
line[1]=8;
if (CGA) line[1]=2;
}

vprint(x,y,c,s) int x,y,c; char *s;
{ /* print a VGA string at x,y, color c */
  /* c|0x100 = solid background */
unsigned int q,i,j,f; unsigned char w,m;

if (CGA) 
{ x=x/4; 
  y=y/8;
  scr_rowcol(y,x);
  puts(s);
  return;
}

f=0;
if (c&0x100)
{ f=1;
  c=c&0xff;
}

while (*s)
{ q=(*s++)*8+0xe;
  for (i=0; i<8; i++)
  { w=_peek(q++,0xffa6);
    m=0x80;
    for (j=0; j<8; j++)
    { if (w&m) _poke(c,line_table[y+i]+x+j,scrn_base);
          else if (f) _poke(0,line_table[y+i]+x+j,scrn_base); 
      m=m>>1;
    }
  }
  x=x+8;
} /* while */
}

loadlev()
{ /* load the level 'level' */
FILE *fp;
int x,y,s;
char n[10],buf[40];
int eta[7];

eta[0]=780;
eta[1]=1300;
eta[2]=2860;
eta[3]=260;
eta[4]=1040;
eta[5]=1820;
eta[6]=0;

cls(); 
strcpy(n,"Level x");
n[6]=level+48;
vprint(132,96,2,n);
x=0;
while (x==0)
  x=scr_csts();
if (x=='?') cheat=(cheat==0);

strcpy(n,"levelx");
n[5]=level+48;
fp=fopen(n,"r");
if (fp==NULL) fail("Can't open level data");
numen=0;
elevnum=0;
levcnt=0;
for (y=0; y<20; y++)
  et[y]=' ';
for (y=0; y<11; y++)
{ fgets(buf,30,fp);
  strcpy(&lev[y][0],buf);
  for (x=0; x<20; x++)
  { put(blank,x<<4,(y<<4)+24);
    switch(buf[x])
    { case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G': /* background */
      case 'H': put(&tile[(buf[x]-65)*260],x<<4,(y<<4)+24);
                break;
      case 'I': /* target tile */
                levcnt++;
                put(&tile[3380],x<<4,(y<<4)+24);
                break;
      case 'a': /* rat */
      case 'b': /* crab */
      case 'c': /* bomb */          
                ex[numen]=x<<4;
                ey[numen]=(y<<4)+24;
                et[numen]=buf[x];
                exd[numen]=1;
                eyd[numen]=0;
                es[numen]=0;
                get(&enbuf[numen*260],x<<4,(y<<4)+24,16,16);
                numen++;
                s=eta[buf[x]-97];
                putm2(&enemy[s],x<<4,(y<<4)+24);
                lev[y][x]=' ';
                break;
      case 'd': /* trap */
      case 'e': /* crystal */
      case 'f': /* icicle */
                ex[numen]=x<<4;
                ey[numen]=(y<<4)+24;
                et[numen]=buf[x];
                exd[numen]=0;
                eyd[numen]=0;
                es[numen]=0;
                get(&enbuf[numen*260],x<<4,(y<<4)+24,16,16);
                numen++;
                s=eta[buf[x]-97];
                putm2(&enemy[s],x<<4,(y<<4)+24);
                lev[y][x]=' ';
                break;
      case 'g': /* door */
                put(door,(x<<4),(y<<4)+8);
                x++;
                break;
      case 'h':
      case 'i':
      case 'j': /* elevator start point */
      case 'k': ex[numen]=x<<4;
                ey[numen]=(y<<4)+24;
                et[numen]='h';
                exd[numen]=0;
                eyd[numen]=0;
                es[numen]=elevnum++;
                get(&widebuf[es[numen]*520],ex[numen],ey[numen],16,32);
                numen++;
                put(wide,x<<4,(y<<4)+24);
                lev[y][x]=buf[x]-55;
                x++;
                break;
      case '*': /* player */
                px=x<<4;
                py=(y<<4)+8;
                s=0;
                xd=0;
                yd=0;
                get(manbuf,px,py,32,32);
                putm2(man,px,py);
                lev[y][x]=' ';
                x++;
                break;
      case '~': /* dragon */
                for (a=0; a<3; a++)
                { ex[numen]=(x<<4)+(a*32);
                  ey[numen]=(y<<4)+24;
                  et[numen]='~';
                  exd[numen]=0;
                  eyd[numen]=0;
                  es[numen]=0;
                  if (a!=2)
                    putm2(dbody,ex[numen],ey[numen]);
                  else
                    putm2(dhead,ex[numen]-24,ey[numen]-32);
                  numen++;
                }
                break;
    } /* switch */
  } /* for */
} /* for */

for (x=0; x<320; x=x+8)
  put(line,x,23);

vprint(40,7,1,"Score:");
numprint(96,7,1,score);
vprint(200,7,1,"Lives:");
numprint(256,7,1,lives);

} /* loadlev */

game()
{ /* enough messing around... time to begin */
int gquit;

level=levone;
lives=3;
score=0;
gquit=1;  /* quit current game. QUIT=0 quits program */
while ((gquit)&&(quit))
{ anf=0;
  ans=ANIM_SP;
  loadlev();
  JUMP=0;
  while(levcnt>0)
  { delay(GAME_SP);
    sound_off();
    bganim();
    badguy();
    goodguy();
    check();
  }
  if (levcnt==-2)
    quit=0;   /* ESC pressed */

  if (levcnt==-1)
  { die();    /* died */
    if (lives<0)
    { gameover();
      gquit=0;
    }
  }

  if (levcnt==0)
  { play(330,20);
    play(440,20);
    play(550,10);
    play(660,40);
    level=level+1;  /**** next level ****/
    if (level==10)
    { /* game won! */
      gquit=0;
      win();
    }
  }
}
}

bganim()
{ /* animate background bits */
int x,y;

ans--;
if (ans) return(0);

ans=ANIM_SP;
anf++;
if (anf==3) anf=0;

for (y=0; y<11; y++)
  for (x=0; x<20; x++)
  { if (lev[y][x]=='G')
      put(&tile[1560+(520*anf)],x<<4,(y<<4)+24);
    if (lev[y][x]=='H')
      put(&tile[1820+(520*anf)],x<<4,(y<<4)+24);
  }

/*  (animate MEOW for cat cage)
 * for (y=0; y<20; y++)
 *   if (et[y]=='g')
 *   { if (es[y]==0)
 *       put(&tile[3120],ex[y]+16,ey[y]);
 *     if (es[y])
 *       put(blank,ex[y]+16,ey[y]);
 *     es[y]=es[y]+1;
 *     if (es[y]==25) es[y]=0;
 *   }
 */

}

int abs(x) int x;
{ if (x>=0) return(x);
    else return(-1*x);
}

badguy()
{ /* move the bad guys around */
int q,w,z,tx,ty;

for (q=0; q<20; q++)
{ switch (et[q])
  { case 'a': /* rat */
              exd[q]=exd[q]+(rand()%3-1);
              if (exd[q]>3) exd[q]=3;
              if (exd[q]<-3) exd[q]=-3;
              if (exd[q]<0) es[q]=2080;
              if (exd[q]>0) es[q]=780;
              tx=ex[q]+exd[q];
              ty=((ey[q]-24)/16)+1;
              tx=(tx+8)/16;
              put(&enbuf[q*260],ex[q],ey[q]);
              if (((lev[ty][tx]<='F')&&(lev[ty][tx]>='A'))||(lev[ty][tx]=='I'))
                ex[q]=ex[q]+exd[q];
              get(&enbuf[q*260],ex[q],ey[q],16,16);
              putm2(&enemy[es[q]],ex[q],ey[q]);
              break;
    case 'b': /* crab */
              tx=ex[q]+exd[q];
              ty=((ey[q]-24)/16)+1;
              tx=(tx+8)/16;
              es[q]=es[q]+1;
              if (es[q]==10) es[q]=0;
              put(&enbuf[q*260],ex[q],ey[q]);
              if (((lev[ty][tx]<='F')&&(lev[ty][tx]>='A'))||(lev[ty][tx]=='I'))
                ex[q]=ex[q]+exd[q];
              else 
                exd[q]=-1*exd[q];
              get(&enbuf[q*260],ex[q],ey[q],16,16);
              if (es[q]>4)
                putm2(&enemy[1300],ex[q],ey[q]);
              else
                putm2(&enemy[1560],ex[q],ey[q]);
              break;
    case 'c': /* bomb */
              if (es[q]==0)
              { if (px>ex[q]) exd[q]=1;
                       else   exd[q]=-1;
                tx=ex[q]+exd[q];
                ty=((ey[q]-24)/16)+1;
                tx=(tx+8)/16;
                put(&enbuf[q*260],ex[q],ey[q]);
                if ((((lev[ty][tx]<='F')&&(lev[ty][tx]>='A'))||(lev[ty][tx]=='I'))&&(anf==1))
                  ex[q]=ex[q]+exd[q];
                if ((ex[q]/2)*2==ex[q]) 
                  w=2340;
                else 
                  w=2600;
                if (exd[q]>0)
                  w=w+520;
                get(&enbuf[q*260],ex[q],ey[q],16,16);
                putm2(&enemy[w],ex[q],ey[q]);
                if ((ex[q]-px<48)&&(ex[q]>=px))
                  es[q]=50;
                if ((px-ex[q]<16)&&(ex[q]<px))
                  es[q]=50;
              }
              else
              { es[q]=es[q]-1;
                put(&enbuf[q*260],ex[q],ey[q]);
                if (es[q]>25)
                { w=3380;
                  if (es[q]%10==es[q]%5)
                    w=w+260;
                  putm2(&enemy[w],ex[q],ey[q]);
                }
                else
                { w=3900;
                  if (es[q]%10==es[q]%5)
                    w=w+260;
                  putm2(&enemy[w],ex[q],ey[q]);
                }
                if (es[q]==0)
                { /* boom */
                  sound_on(2169);
                  put(&enbuf[q*260],ex[q],ey[q]);
                  et[q]=' ';
                  for (z=0; z<3; z++)
                  { w=enblank();
                    if (w!=-1)
                    { ex[w]=ex[q];
                      ey[w]=ey[q];
                      et[w]='i';
                      if (z==0)
                        exd[w]=-2;
                      if (z==1)
                        exd[w]=0;
                      if (z==2)
                        exd[w]=2;
                      eyd[w]=-4;
                      get(&enbuf[w*260],ex[w],ey[w],16,16);
                    }
                  }
                  sound_off();
                }
              }
              break;
    case 'f': /* icicle */
              if (es[q]==0)
              { if ((ex[q]-px<48)&&(ex[q]>=px))
                  es[q]=1;
                if ((px-ex[q]<16)&&(ex[q]<px))
                  es[q]=1;
              }
              else
              { put(&enbuf[q*260],ex[q],ey[q]);
                eyd[q]=eyd[q]+1;
                if (eyd[q]>8) eyd[q]=8;
                ey[q]=ey[q]+eyd[q];
                if (onscreen(ex[q],ey[q]))
                { get(&enbuf[q*260],ex[q],ey[q],16,16);
                  putm2(&enemy[1820],ex[q],ey[q]);
                }
                else 
                  et[q]=' ';
              }
              break;
    case 'h': /* elevator */
              elev=0;
              put(blank,ex[q],ey[q]);
              put(blank,ex[q]+16,ey[q]);
              tx=ex[q]/16; 
              ty=((ey[q]-24)/16); 
              w=0;
              if ((py<=(ey[q]-26))&&(py>=(ey[q]-40))&&(px>=(ex[q]-16))&&(px<=(ex[q]+24)))
              { w=1;
                put(manbuf,px,py);
                if (yd>=0)
                { py=ey[q]-32;
                  if (yd>0)
                  { yd=0;
                    JUMP=0;
                  }
                  elev=1;
                  if (s==3090) s=0;
                  if (s==6180) s=9270;
                }
              }
              switch(lev[ty][tx])
              { case '1': ey[q]=ey[q]-1;
                          if (w) py=py-1;
                          break;
                case '2': ey[q]=ey[q]+1;
                          if (w) py=py+1;
                          break;
                case '3': ex[q]=ex[q]+1;
                          if (w) px=px+1;
                          break;
                case '4': ex[q]=ex[q]-1;
                          if (w) px=px-1;
                          break;
              }
              put(wide,ex[q],ey[q]);
              if (w)
              { get(manbuf,px,py,32,32);
                putm2(&man[s],px,py);
              }
              break;
    case 'i': /* spark */
              put(&enbuf[q*260],ex[q],ey[q]);
              eyd[q]=eyd[q]+1;
              if (eyd[q]>8) eyd[q]=8;
              ex[q]=ex[q]+exd[q];
              ey[q]=ey[q]+eyd[q];
              if (onscreen(ex[q],ey[q]))
              { get(&enbuf[q*260],ex[q],ey[q],16,16);
                putm2(&enemy[4420],ex[q],ey[q]);
              }
              else
                et[q]=' ';
              break;
    case '~': /* dragon (3 parts) */
              for (a=0; a<2; a++)
                erdrag(ex[q+a],ey[q+a],3,3);
              erdrag(ex[q+2]-24,ey[q+2]-32,7,6);
              for (a=0; a<3; a++)
              { eyd[q+a]=eyd[q+a]+((rand()%5)-2);
                if (eyd[q+a]<-5) eyd[q+a]=-5;
                if (eyd[q+a]>5) eyd[q+a]=5;
                for (w=a; w<3; w++)
                { ey[q+w]=ey[q+w]+eyd[q];
                  if (ey[q+w]>100) ey[q+w]=100;
                  if (ey[q+w]<48) ey[q+w]=48;
                }
                if (a!=2)
                  putm2(dbody,ex[q+a],ey[q+a]);
                else
                  putm2(dhead,ex[q+2]-24,ey[q+2]-32);
              }
              q=q+3;
              if (es[q-1]) es[q-1]=es[q-1]-1;
              if ((rand()%10==8)&&(es[q-1]==0))
              { w=0;
                for (a=19; a>16; a--)
                  if (et[a]==' ')
                    w=a;
                if (w)
                { ex[w]=ex[q-1]+80;
                  ey[w]=ey[q-1]+50;
                  et[w]='j';
                  exd[w]=2;
                  eyd[w]=0;
                  es[w]=0;
                  es[q-1]=25;
                  get(&widebuf[(w-17)*520],ex[w],ey[w],16,32);
                  putm2(&wide[520],ex[w],ey[w]);
                }
              }
              break;
    case 'j': /* dragon fireball */
              put(&widebuf[(q-17)*520],ex[q],ey[q]);
              ex[q]=ex[q]+exd[q];
              if (onscreen(ex[q]+32,ey[q]))
              { get(&widebuf[(q-17)*520],ex[q],ey[q],16,32);
                putm2(&wide[520+es[q]],ex[q],ey[q]);
                if (es[q]) es[q]=0;
                else es[q]=520;
              }
              else et[q]=' ';
              break;
  }
}
}

int enblank()
{ /* return first blank enemy slot, or -1 if none */
int w,a;

w=-1;
for (a=0; a<20; a++)
  if ((et[a]==' ')&&(w==-1)) w=a;
return(w);
}

int onscreen(x,y) int x,y;
{ /* return true if co-ords are onscreen */
int w;

w=1;
if ((x<0)||(x>315)) w=0;
if ((y<0)||(y>195)) w=0;
return(w);
}

fixscreen(x,y) int *x,*y;
{ /* ensure co-ords are onscreen */
if (*x<0) *x=0;
if (*x>288) *x=288;
if (*y<0) *y=0;
if (*y>168) *y=168;
}

goodguy()
{ /* move the player around */
int k,zz,tx,ty,qw,fl;

fl=0;

if (JUMP)
{ yd=yd+2;
  qw='A';
  if (yd>=11)
  { yd=0;
    s=0;
    JUMP=0;
    fl=1;
    if (xd<0) s=9270;
  }
  k=scr_csts();
}
else
{ tx=(px+16)/16;
  ty=((py-8)/16)+1;
  qw=lev[ty][tx];

  k=scr_csts();
  if (k>90) k=k-32;  /* make uppercase */

  if (cheat)
  { if (k=='0') cheat=0;
    if (k=='1') levcnt=0;
  }

  if ((k=='E')&&((lev[ty-1][tx]=='F')||(lev[ty][tx]=='F')))
  { s=4120;      
    if (yd==-2) yd=0; else yd=-2;  
    xd=0;
  }
  if ((yd==-2)&&(lev[ty-1][tx]!='F')&&(lev[ty][tx]!='F'))
    yd=0;

  if ((k=='X')&&(qw=='F'))
  { s=4120;
    if (yd==2) yd=0; else yd=2;
    xd=0;
  }
  if ((yd==2)&&(qw!='F'))
    yd=0;

  if (k=='S')
  { s=7210;
    if (xd==-4) xd=0; else xd=-4;
    yd=0;
  }

  if (k=='D')
  { s=1030;
    if (xd==4) xd=0; else xd=4;
    yd=0;
  }
  if ((k==' ')&&(yd==0))
  { s=3090;
    if (xd<0) s=6180;
    yd=-9;
    JUMP=1;
  }

  if (k==27) levcnt=-2;  /*****quit with esc*****/

  if (((qw<'A')||(qw>'F'))&&(qw!='I')&&(elev==0)&&(JUMP==0))
    yd=4;
  else
    if ((yd==4)&&(JUMP==0)) yd=0;
}

if ((yd==0)&&(xd==0))
{ if ((s==1030)||(s==2060)) { s=0; fl=1; }
  if ((s==7210)||(s==8240)) { s=9270; fl=1; }
}

if ((xd)&&(s==0)) { s=1030; fl=1; }
if ((xd)&&(s==9270)) { s=7210; fl=1; }

if ((xd)||(yd)||(fl))
{ put(manbuf,px,py);
  if (qw=='I')
  { put(&tile[0],tx<<4,(ty<<4)+24);
    levcnt--;
    lev[ty][tx]='A';
    score=score+50;
    sound_on(3000+(levcnt*100));
    numprint(96,7,1|SOLID,score);
  }
  if ((qw=='G')||(qw=='H'))
    levcnt=-1;  /* player died */
  px=px+xd;
  py=py+yd;
  if ((py<0)&&(yd<0)) yd=yd*(-1);
  if ((elev==0)&&(yd==0)&&(xd))
  { if ((py-8)>>4)
      py=(((py-8)>>4)<<4)+8;
  }
  fixscreen(&px,&py);
  for (zz=0; zz<20; zz++)
    if (et[zz]=='h')
      erdrag(ex[zz],ey[zz],2,1);
  get(manbuf,px,py,32,32);
  for (zz=0; zz<20; zz++)
    if (et[zz]=='h')
      put(wide,ex[zz],ey[zz]);
  if ((JUMP==0)&&(s!=0)&&(s!=9270))
    putm2(&man[s+1030*(anf/2)],px,py);
  else
    putm2(&man[s],px,py);
}
}

check()
{ /* check for collisions */
  /* set levcnt to -1 for dead */
int q,w,xm,ym,xo,yo;

for (q=0; q<20; q++)
{ if (et[q]!=' ')
  { switch (et[q])
    { case 'a': xm=4; ym=9; xo=8; yo=8; break;
      case 'b': xm=8; ym=9; xo=8; yo=8; break;
      case 'c': xm=8; ym=9; xo=8; yo=8; break;
      case 'd': xm=8; ym=9; xo=8; yo=8; break;
      case 'e': xm=6; ym=9; xo=8; yo=8; break;
      case 'f': xm=8; ym=8; xo=8; yo=8; break;
      case 'i': xm=4; ym=10; xo=8; yo=8; break;
      case 'j': xm=16; ym=13; xo=16; yo=8; break;
      case '~': xm=40; ym=20; xo=50; yo=40; break;
      default: xm=0;
    }
    if ((xm)&&(cheat==0))
      if (abs((px+16)-(ex[q]+xo))<=xm)
        if (abs((py+16)-(ey[q]+yo))<=ym)
          levcnt=-1;
  }
}
}

erdrag(x,y,nx,ny)
{ /* erase dragon shape */
int a,b;

for (a=0; a<nx; a++)
  for (b=0; b<ny; b++)
    put(blank,x+(a*16),y+(b*16));

}

putm2(d,x,y) char *d; int x,y;
{ /* call the proper put routine checking SLOW */
if (SLOW)
  put(d,x,y);
else
  putm(d,x,y);
}

numprint(x,y,c,z) int x,y,c; unsigned z;
{ /* use vprint() to print a number, up to 5 digits (65535) */
char qw[40];
int i,f;

i=0;
f=0;

if ((z/10000)||(f))
{ f=1;
  qw[i++]=(z/10000)+48;
  z=z%10000;
}

if ((z/1000)||(f))
{ f=1;
  qw[i++]=(z/1000)+48;
  z=z%1000;
}

if ((z/100)||(f))
{ f=1;
  qw[i++]=(z/100)+48;
  z=z%100;
}

if ((z/10)||(f))
{ f=1;
  qw[i++]=(z/10)+48;
  z=z%10;
}

qw[i++]=z+48;
qw[i]=0;
vprint(x,y,c,qw);
}

die()
{ /* death routine */
int q;

for (q=5000; q>1000; q=q-100)
{  sound_on(q);
   delay(speed);
}
s=0;
delay(speed*6);
sound_off();
lives--;
}

gameover()
{ /* game over routine */
int q,w;

while (scr_csts());

for (q=0; q<10; q++)
  for (w=0; w<32; w++)
  {  vprint(124,96,w,"GAME OVER");
     delay(speed);
     if (scr_csts())
     { q=11; w=33;
     }
  }
}

win()
{ /* game win routine */
int q;

while (scr_csts());
load_bmp("g_win.bmp");
fade_in(2);
colour(255,63,63,63);
textsp=6;
csprint(5,"Finally past all the obstacles, you");
csprint(6,"knock on the door. Quickly slipping");
csprint(7,"inside, you present the customer with");
csprint(8,"your wares. Dazzling him with the system");
csprint(9,"specs of the amazing IBS System 7, such");
csprint(10,"as it's toggle-key keyboard, electronic");
csprint(11,"power indicator LED, dedicated cassette");
csprint(12,"I/O ports, and so on, he is soon very");
csprint(13,"anxious to buy. Yet another successful");
csprint(14,"sale for the amazing Super Sales Acer!!");
for (q=0; q<10; q++)
{ if (scr_csts()) q=11;
  delay(speed*60);
}
fade_out(1);
cls();
fade_in(5);
colour(255,63,63,63);
textsp=3;
csprint(0,"Super Sales Acer-created and written by");
csprint(1,"Mike Brent (c)1995 All Rights Reserved");
csprint(3,"Watch for the following upcoming games!");
csprint(5,"Super Shooting Acer");
csprint(6,"Super Sled Acer");
csprint(7,"Galactic Exploration (BBS door game)");
csprint(8,"Super Speed Acer");
csprint(9,"Super Space Acer");
csprint(10,"..and more!!");
csprint(13,"Cheats! Type 'SALEACER ?' for info.");
csprint(14,"At the 'LEVEL x' screen, press '?'");
csprint(15,"to toggle cheat mode. You are");
csprint(16,"invincicle in cheat mode. In cheat");
csprint(17,"mode, press '1' for level skip, '0'");
csprint(18,"to turn cheat off.");
csprint(23,"Hit <esc> to exit...");
while (scr_csts()!=27);
}

csprint(y,s) int y; char *s;
{ /* print a string slowly and centered at text row 'y' */
  /* uses 'textsp' */
int x,q,w,m,i,j;

y=y*8;
x=((40-strlen(s))/2)*8;
while (*s)
{ if (CGA)
  { scr_rowcol(y/8,x/4);
    putchar(*s);
    s++;
    x=x+8;
  }
  else
  { q=(*s++)*8+0xe;
    for (i=0; i<8; i++)
    { w=_peek(q++,0xffa6);
      m=0x80;
      for (j=0; j<8; j++)
      { if (w&m) _poke(255,line_table[y+i]+x+j,scrn_base);
        m=m>>1;
      }
    }
    x=x+8;
  }
delay(speed*textsp);
if (scr_csts()) textsp=0;

} /* while */
}

