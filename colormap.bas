
#Include "inc/cmap.inc"
#Include "inc/utils.inc"

CONST PWD$=getParent$(MM.INFO(current))

CONST FONTNO=7
CONST STARTX=48
CONST STARTY=20
CONST COLORSX=8
CONST COLORSY=256/COLORSX

DIM Integer fw,fh
DIM Integer row, col
DIM Integer model, color

mode 1,8
cmap.init()
cmap.load PWD$+"/img/colors.cmap"
cmap.activate

' loading assets
if cmap.getCMode()=8 then
  load data "img/sprites.raw",MM.INFO(PAGE ADDRESS 1)
else
  page write 1
  load png "img/helicopter.png"
endif

page write 0
do
  cls
  font FONTNO
  fw=MM.INFO(FONTWIDTH)
  fh=MM.INFO(FONTHEIGHT)+1

  colw=len("0x000000")*fw+fh+12
  tpos=len("0x000000")*fw+fh

  for col=0 to COLORSX-1
    text STARTX+col*colw+tpos,STARTY-fh,str$(col),"C",FONTNO,,getWhite()
  next

  for row=0 to COLORSY-1
    text STARTX-15,STARTY+row*fh,hex$(row*COLORSX),"R",FONTNO,,getWhite()
  next

  for col=0 to COLORSX-1
    for row=0 to COLORSY-1
      if cmap.getCMode()=8 then color=map(row*COLORSX+col) else color=cmap.get(row*COLORSX+col)
      box STARTX+col*colw,STARTY+row*fh,fw,fh,1,rgb(black),color
      text STARTX+col*colw+fw+4,STARTY+row*fh,"0x"+HEX$(cmap.get(row*COLORSX+col),6),"L",FONTNO,,getWhite()
    next
  next

  memoryMap model*50,100

  do while keydown(0)<>0:loop
  do while keydown(0)=0:loop
  key=keydown(1)
  if key=asc("q") then exit do
  if key=asc("c") then cmap.load PWD$+"/img/colors.cmap":cmap.activate
  if key=asc("r") then cmap.reset
  if key=asc("1") then model=0
  if key=asc("2") then model=1
  if key=asc("3") then model=2
  if key=asc("4") then model=3
  if key=asc("5") then model=4
  if key=asc("6") then model=5
loop

function getWhite() as Integer
  LOCAL Integer cmode=cmap.getCMode()

  getWhite=choice(cmode=8,map(7),rgb(white))  
end function

sub memoryMap(x%,y%)
  LOCAL Integer oW=50
  LOCAL Integer oH=50
  LOCAL Integer h=640
  LOCAL Integer addr%
  LOCAL Integer cmode=cmap.getCMode()

  LOCAL Integer ofs%=50

  addr%=MM.INFO(PAGE ADDRESS 1)
  if cmode=32 then
    for col%=0 to oW
      for row%=0 to oH
        adr%=addr%+(ofs%+row%)*4*h+0+(col%+13)*4
        c%=peek(BYTE adr%)+(peek(BYTE adr%+1)<<8)+(peek(BYTE adr%+2)<<16)
        text 40+col%*(fw*6+2),STARTY+(COLORSY+1)*fh+row%*fh, HEX$(c%,6),"R",FONTNO,,c%
      next
    next
  elseif cmode=16 then
    for col%=0 to oW
      for row%=0 to oH
        adr%=addr%+(ofs%+row%)*2*h+0+(col%+13)*2
        c%=peek(BYTE adr%) OR (peek(BYTE adr%+1)<<8)
        c1%=(c% AND &B0000000000011111)<<3
        c2%=(c% AND &B0000011111100000)<<5
        c3%=(c% AND &B1111100000000000)<<8
        c%=c1% OR c2% OR c3%
        text 40+col%*(fw*6+2),STARTY+(COLORSY+1)*fh+row%*fh, HEX$(c%,6),"R",FONTNO,,c%
      next
    next
  elseif cmode=8 then
    for col%=0 to oW
      for row%=0 to oH
        adr%=addr%+(y%+row%)*h+0+x%+col%
        c%=peek(BYTE adr%)
        text 85+col%*(fw*2+1),STARTY+(COLORSY+1)*fh+row%*fh, HEX$(c%,2),"R",FONTNO,,map(c%)
      next
    next
  endif
  blit x%,y%,MM.HRES-oW*2,STARTY+COLORSY*fh,oW,oH,1,&B100
end sub

