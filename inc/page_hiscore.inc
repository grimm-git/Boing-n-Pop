
CONST HISTATE_EDIT=0
CONST HISTATE_SHOW=1

DIM Integer Hiscore.State
DIM Integer Hiscore.Cursor.X  'screen position
DIM Integer Hiscore.Cursor.Y  'character
DIM Integer Hiscore.Player
DIM Integer Hiscore.Pos
DIM Integer Hiscore.C(2)

sub Hiscore.setup()
  Hiscore.State=HISTATE_EDIT
  Hiscore.Player=1

  mode 8,16
  page write PAGE_BUFFER
'  playMOD "music"
end sub

sub Hiscore.draw()
  LOCAL Integer n
  LOCAL Float lf,cx,cy
  LOCAL String temp$

  blit 0,0,(Screen.W-364)/2,0,364,260,PAGE_TITLE,&B100
  text Screen.W/2+70,250,"(c)2025","L",7,,rgb(220,220,80),-1

  lf=14 : cx=Screen.W/2 : cy=270
  blit 396,64,cx-53,cy,106,32,PAGE_TITLE,&B100 : inc cy,30

  select case Hiscore.state
  case HISTATE_EDIT
    text cx,cy,"Player "+str$(Hiscore.Player),"C",7,,Player.getColor(Hiscore.Player))
    inc cy,12

    for n=0 to HISCORE_NUM-1
      if Hiscore.Pos=n then
        temp$=chr$(Hiscore.C(0))+" "+chr$(Hiscore.C(1))+" "+chr$(Hiscore.C(2))
        text cx,cy,str$(n+1,2)+" "+temp$+"  "+str$(Hiscore.score(n),7),"C",1,2,rgb(240,200,100),-1
        rbox cx-89+Hiscore.Cursor.X*32,cy-1,20,25,4,rgb(red)    
        inc cy,24
      else
        text cx,cy,str$(n+1,2)+" "+Hiscore.name$(n)+"  "+str$(Hiscore.score(n),7),"C",1,,rgb(240,200,100),-1
        inc cy,lf
      endif
    next

  case HISTATE_SHOW
    for n=0 to HISCORE_NUM-1
      text cx,cy,str$(n+1,2)+" "+Hiscore.name$(n)+"  "+str$(Hiscore.score(n),7),"C",1,,rgb(240,200,100),-1
      inc cy,lf
    next
    text cx,cy+4,"Press RETURN to continue","C",7,,rgb(220,200,80),-1

  end select
end sub

function Hiscore.control() as Integer
  STATIC Float tim,player=0
  STATIC Integer lock,one,lastkey,lastctrl
  LOCAL Integer key,n,ctrl,char,tmp
  LOCAL String name$

  if Hiscore.Player<>player then
    player=Hiscore.Player

    tmp=Hiscore.freePos(Player.getScore(player))
    if tmp>=0 then
      Hiscore.Pos=tmp

      name$=Hiscore.name$(tmp)
      for n=0 to 2 : Hiscore.C(n)=asc(mid$(name$,n+1,1)) : next

    else
      Hiscore.state=HISTATE_SHOW
    endif
  endif
  
  ctrl=0 : key=0
  if Controls.isNunchuk(2) then ctrl=ctrl or Controls.read(2)
  if Controls.isNunchuk(3) then ctrl=ctrl or Controls.read(3)
  if keydown(0)>0 or ctrl>0 then
    if ctrl=lastctrl then ctrl=0 else lastctrl=ctrl
    key=keydown(1) : if key=lastkey then key=0 else lastkey=key

    select case Hiscore.state
    case HISTATE_EDIT
      char=Hiscore.C(Hiscore.Cursor.X)      
      if (ctrl and 4)>0 or key=128 then
        inc char,-1
        if char<65 then char=90
      endif
      if (ctrl and 8)>0 or key=129 then
        inc char
        if char>90 then char=65
      endif
      Hiscore.C(Hiscore.Cursor.X)=char

      if (ctrl and 2)>0 or key=131 then
        inc Hiscore.Cursor.X
        if Hiscore.Cursor.X>2 then Hiscore.Cursor.X=0
      endif
      if (ctrl and 1)>0 or key=130 then
        inc Hiscore.Cursor.X,-1
        if Hiscore.Cursor.X<0 then Hiscore.Cursor.X=2
      endif
      if (ctrl and 16)>0 or key=32 then
        Hiscore.name$(Hiscore.Pos)=chr$(Hiscore.C(0))+chr$(Hiscore.C(1))+chr$(Hiscore.C(2))
        if player+1 > Game.numPlayers then
          Hiscore.State=HISTATE_SHOW
        else
          Hiscore.Player=player+1
        endif
      endif        

      if key=13 or key=10 then
        playSample 2,22050
        pause 100 'necessary to play sound fx
        Hiscore.control=1
      endif
      if ctrl>0 or key>0 then playSample 1,22050

    case HISTATE_SHOW
      if key=13 or key=10 then Hiscore.control=1

    end select

  else
    lastctrl=0
    lastkey=0
  endif
end function

