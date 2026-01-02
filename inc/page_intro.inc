
CONST INTRO_ANIMSPD=3 '5
CONST INTRO_ANIMOBJ=12
CONST INTRO_PHASE=20000

CONST ISTATE_CREDITS=0
CONST ISTATE_CREDITS_OUT=1
CONST ISTATE_HISCORE=2
CONST ISTATE_HISCORE_OUT=3
CONST ISTATE_SETTINGS=4

CONST OBJSTATE_NONE=0
CONST OBJSTATE_INIT=1
CONST OBJSTATE_RUN=2
CONST OBJSTATE_COMPLETE=3

DIM Integer Intro.State
DIM Integer Intro.maxPlayers
DIM Integer Intro.numPlayers
DIM Integer Intro.Difficulty=1
DIM Integer Intro.Cursor.X
DIM Integer Intro.Cursor.Y

DIM Integer Intro.Obj.State(INTRO_ANIMOBJ)
DIM Float   Intro.Obj.X(INTRO_ANIMOBJ)      'current position
DIM Float   Intro.Obj.Y(INTRO_ANIMOBJ)
DIM Float   Intro.Obj.StartX(INTRO_ANIMOBJ) 'anim start position
DIM Float   Intro.Obj.StartY(INTRO_ANIMOBJ)
DIM Float   Intro.Obj.EndX(INTRO_ANIMOBJ)   'anim end position
DIM Float   Intro.Obj.EndY(INTRO_ANIMOBJ)
DIM Float   Intro.Obj.Event(INTRO_ANIMOBJ)  'percentage, when to start next anim

sub Intro.setup()
  LOCAL Integer ctrls=Controls.getNumControls()
  LOCAL Float cx,cy

  Intro.maxPlayers=choice(ctrls>2,2,ctrls)
  Intro.numPlayers=1
  Intro.State=ISTATE_CREDITS

  mode 8,16
  page write PAGE_BUFFER
'  playMOD "music"
end sub

sub Intro.draw()
  LOCAL Integer n
  LOCAL Float lf,cx,cy

  blit 0,0,(Screen.W-364)/2,0,364,260,PAGE_TITLE,&B100
  text Screen.W/2+70,250,"(c)2025","L",7,,rgb(220,220,80),-1

  lf=18 : cx=Screen.W/2 : cy=270
  select case Intro.state
  case ISTATE_CREDITS,ISTATE_CREDITS_OUT
    Intro.drawCredits Intro.Obj.X(0),Intro.Obj.Y(0)
    Intro.drawProg    Intro.Obj.X(1),Intro.Obj.Y(1)
    Intro.drawSoundFx Intro.Obj.X(2),Intro.Obj.Y(2)
    Intro.drawMusic   Intro.Obj.X(3),Intro.Obj.Y(3)
    Intro.drawIdea    Intro.Obj.X(4),Intro.Obj.Y(4)
    Intro.drawHelp    Intro.Obj.X(5),Intro.Obj.Y(5)

  case ISTATE_HISCORE,ISTATE_HISCORE_OUT
    Intro.drawHiscore Intro.Obj.X(0),Intro.Obj.Y(0)
    for n=0 to HISCORE_NUM-1
      Intro.drawScore Intro.Obj.X(n+1),Intro.Obj.Y(n+1),n
    next
    Intro.drawHelp    Intro.Obj.X(11),Intro.Obj.Y(11)

  case ISTATE_SETTINGS
    blit 396,128,cx-62,cy,124,32,PAGE_TITLE,&B100 : inc cy,48  'Settings

    blit 396,192,cx-140,cy,102,32,PAGE_TITLE,&B100  'Players
    n=choice(Intro.numPlayers=1,396,428)
    blit n,224,cx+90,cy,32,32,PAGE_TITLE,&B100
    inc cy,48

    blit 396,160,cx-140,cy  ,140,32,PAGE_TITLE,&B100  'Difficulty
    blit 364,128,cx+ 50,cy  , 32,32,PAGE_TITLE,&B100 
    blit 364,160,cx+ 94,cy+4, 24,24,PAGE_TITLE,&B100 
    blit 364,184,cx+138,cy+8, 16,16,PAGE_TITLE,&B100
    blit 364,200,cx+50+Intro.difficulty*40,cy,32,32,PAGE_TITLE,&B100
    inc cy,48
    text cx,cy,"Nunchuk Joystick to select, RETURN to accept and start the Game","C",7,,rgb(220,200,80),-1

    if Intro.Cursor.Y=0 then
      rbox cx+90,cy-96,32,32,4,rgb(red)    
    else
      rbox cx+50+Intro.Cursor.X*40,cy-48,32,32,4,rgb(red)    
    endif
  end select
end sub

'returns number of selected players or 0 if the selection has not been made yet
function Intro.control() as Integer
  STATIC Float tim
  STATIC Integer lock,one,lastkey,lastctrl
  LOCAL Integer key,num,n,ctrl
  
  select case Intro.state
  case ISTATE_CREDITS
    if one=0 then one=1 : restore IntroCreditsIn : Intro.prepAnim(6) : tim=TIMER
    if Intro.anim(0) then
      if Intro.anim(2) then
        if Intro.anim(1) then
          if Intro.anim(4) then
            if Intro.anim(3) then
              if Intro.anim(5) then
                if TIMER-tim>INTRO_PHASE then one=0 : Intro.State=ISTATE_CREDITS_OUT
              endif
            endif
          endif
        endif
      endif
    endif

  case ISTATE_CREDITS_OUT
    if one=0 then one=1 : restore IntroCreditsOut : Intro.prepAnim(6)
    key=1
    for n=0 to 5
      key=key*Intro.anim(n)
    next
    if key then one=0 : Intro.State=ISTATE_HISCORE

  case ISTATE_HISCORE
    if one=0 then one=1 : restore IntroHiscoreIn : Intro.prepAnim(12) : tim=TIMER
    for n=0 to 11
      if Intro.anim(n)=0 then exit for
      if n=11 then
        if TIMER-tim>INTRO_PHASE then one=0 : Intro.State=ISTATE_HISCORE_OUT
      endif
    next

  case ISTATE_HISCORE_OUT
    if one=0 then one=1 : restore IntroHiscoreOut : Intro.prepAnim(12)
    for n=11 to 0 step -1
      if Intro.anim(n)=0 then exit for
      if n=0 then one=0 : Intro.State=ISTATE_CREDITS
    next

  case ISTATE_SETTINGS
  end select

  ctrl=0 : key=0
  if Controls.isNunchuk(2) then ctrl=ctrl or Controls.read(2)
  if Controls.isNunchuk(3) then ctrl=ctrl or Controls.read(3)
  if keydown(0)>0 or ctrl>0 then
    if ctrl=lastctrl then ctrl=0 else lastctrl=ctrl
    key=keydown(1) : if key=lastkey then key=0 else lastkey=key

    select case Intro.state
    case ISTATE_CREDITS,ISTATE_CREDITS_OUT,ISTATE_HISCORE,ISTATE_HISCORE_OUT
      if key=32 or (ctrl and 16)>0 then
        playMOD "keysound"
        playSample 1,22050
        Intro.state=ISTATE_SETTINGS
      endif

    case ISTATE_SETTINGS
      if (ctrl and 4)>0 or key=128 then Intro.Cursor.Y=(Intro.Cursor.Y-1) and 1
      if (ctrl and 8)>0 or key=129 then Intro.Cursor.Y=(Intro.Cursor.Y+1) and 1
      if Intro.Cursor.Y=0 then
        if (ctrl and 2)>0 or key=131 then inc Intro.numPLayers
        if (ctrl and 1)>0 or key=130 then inc Intro.numPLayers,-1
        if (ctrl and 16)>0 or key=32 then inc Intro.numPLayers
        if Intro.numPLayers<1 then Intro.NumPlayers=1
        Intro.numPlayers=min(Intro.numPlayers,Intro.maxPlayers)
        Intro.Cursor.X=1
      else
        if (ctrl and 2)>0 or key=131 then inc Intro.Cursor.X,1
        if (ctrl and 1)>0 or key=130 then inc Intro.Cursor.X,-1
        if Intro.Cursor.X>2 then Intro.Cursor.X=0
        if Intro.Cursor.X<0 then Intro.Cursor.X=2
        if (ctrl and 16)>0 or key=32 then Intro.Difficulty=Intro.Cursor.X
      endif

      if ctrl>0 or key>0 then playSample 1,22050
      if key=13 or key=10 then
        playSample 1,22050
        pause 100 'necessary to play sound fx
        Intro.control=Intro.NumPlayers
      endif
    end select

  else
    lastctrl=0
    lastkey=0
  endif
end function

function Intro.anim(n%) as Integer
  LOCAL Float x1,y1,x2,y2,e
  LOCAL Float dx,dy,dl,progress

  x1=Intro.Obj.StartX(n%) : y1=Intro.Obj.StartY(n%)
  x2=Intro.Obj.EndX(n%)   : y2=Intro.Obj.EndY(n%)

  if Intro.Obj.State(n%)=OBJSTATE_INIT then
    Intro.Obj.State(n%)=OBJSTATE_RUN
    Intro.Obj.X(n%)=x1
    Intro.Obj.Y(n%)=y1
  endif

  if abs(x2!-Intro.Obj.X(n%))<INTRO_ANIMSPD and abs(y2!-Intro.Obj.y(n%))<INTRO_ANIMSPD then
    Intro.Obj.State(n%)=OBJSTATE_COMPLETE
    Intro.Obj.X(n%)=x2
    Intro.Obj.Y(n%)=y2
    Intro.anim=1
    exit function
  endif

  dx=x2-x1 : dy=y2-y1 : dl=SQR(dx*dx+dy*dy)
  inc Intro.Obj.X(n%),INTRO_ANIMSPD*dx/dl
  inc Intro.Obj.Y(n%),INTRO_ANIMSPD*dy/dl

  e=Intro.Obj.Event(n%)
  dx=Intro.Obj.X(n%)-x1 : dy=Intro.Obj.Y(n%)-y1 : progress=SQR(dx*dx+dy*dy)/dl
  Intro.anim=choice(progress<e,0,1)
end function

sub Intro.drawCredits(x!,y!)
    blit 396,96,x!-53,y!,106,32,PAGE_TITLE,&B100
end sub

sub Intro.drawProg(x!,y!)
    blit 364,0,x!,y!,32,32,PAGE_TITLE,&B100   'programming
    text x!+40,y!+10,"Matthias Grimm","L",4,,rgb(80,200,08),-1
end sub

sub Intro.drawSoundFX(x!,y!)
    blit 364,32,x!,y!,32,32,PAGE_TITLE,&B100   'soundfx
    text x!+40,y!+10,"Matthias Grimm & Dominik Braun","L",4,,rgb(88,86,200),-1
end sub

sub Intro.drawMusic(x!,y!)
    blit 364,64,x!,y!,32,32,PAGE_TITLE,&B100   'music
    text x!+40,y!+10,"Unknown Artist","L",4,,rgb(230,80,80),-1
end sub

sub Intro.drawIdea(x!,y!)
    blit 364,96,x!,y!,32,32,PAGE_TITLE,&B100   'idea
    text x!+40,y!+10,"Matthias Grimm & Lukas Lee","L",4,,rgb(200,200,80),-1
end sub

sub Intro.drawHelp(x!,y!)
    if y!<200 then exit sub
    text x!,y!,"Press SPACE to start the Game","C",7,,rgb(green),-1
end sub

sub Intro.drawHiscore(x!,y!)
    blit 396,64,x!-53,y!,106,32,PAGE_TITLE,&B100
end sub

sub Intro.drawScore(x!,y!,idx%)
    if y!<200 then exit sub
    text x!,y!,str$(idx%+1,2)+" "+Hiscore.name$(idx%)+"  "+str$(Hiscore.score(idx%),7),"C",1,,rgb(240,200,100),-1
end sub

sub Intro.prepAnim(num%)
  LOCAL Integer n
  LOCAL Float cx,cy
  LOCAL Float d1,d2,d3,d4,d5

  cx=Screen.W/2 : cy=270  'virtual zero point of animations
  for n=0 to num%-1
    read d1,d2,d3,d4,d5
    Intro.Obj.State(n)=OBJSTATE_INIT
    Intro.Obj.X(n)     =cx+d1
    Intro.Obj.Y(n)     =cy+d2
    Intro.Obj.StartX(n)=cx+d1
    Intro.Obj.StartY(n)=cy+d2
    Intro.Obj.EndX(n)  =cx+d3
    Intro.Obj.EndY(n)  =cy+d4
    Intro.Obj.Event(n) =d5
  next
end sub

IntroCreditsIn:
'      Sx, Sy,  Ex, Ey, %
data    0,220,   0,  0,0.3 'Credits
data  320, 28, -90, 28,0.3 'Programming
data -680, 56,-160, 56,0.3 'SoundFx
data  320, 84, -90, 84,1.0 'Music
data -640,112,-150,112,0.3 'Idea
data    0,220,   0,160,1.0 'Help

IntroCreditsOut:
'      Sx, Sy,  Ex, Ey, %
data    0,  0,   0,220,1.0 'Credits
data  -90, 28, -90,220,1.0 'Programming
data -160, 56,-160,220,1.0 'SoundFx
data  -90, 84, -90,220,1.0 'Music
data -150,112,-150,220,1.0 'Idea
data    0,160,   0,220,1.0 'Help

IntroHiscoreIn:
'      Sx, Sy,  Ex, Ey, %
data    0,220,   0,  0,1.0 'Hiscore
data   -4,220,  -4, 30,1.0 'Score #1
data   -4,220,  -4, 44,1.0 'Score #2
data   -4,220,  -4, 58,1.0 'Score #3
data   -4,220,  -4, 72,1.0 'Score #4
data   -4,220,  -4, 86,1.0 'Score #5
data   -4,220,  -4,100,1.0 'Score #6
data   -4,220,  -4,114,1.0 'Score #7
data   -4,220,  -4,128,1.0 'Score #8
data   -4,220,  -4,142,1.0 'Score #9
data   -4,220,  -4,156,1.0 'Score #10
data    0,220,   0,180,1.0 'Help

IntroHiscoreOut:
'      Sx, Sy,  Ex, Ey, %
data    0,  0,   0,220,1.0 'Hiscore
data   -4, 30,  -4,220,0.5 'Score #1
data   -4, 44,  -4,220,0.5 'Score #2
data   -4, 58,  -4,220,0.5 'Score #3
data   -4, 72,  -4,220,0.5 'Score #4
data   -4, 86,  -4,220,0.5 'Score #5
data   -4,100,  -4,220,0.5 'Score #6
data   -4,114,  -4,220,0.5 'Score #7
data   -4,128,  -4,220,0.5 'Score #8
data   -4,142,  -4,220,0.5 'Score #9
data   -4,156,  -4,220,0.5 'Score #10
data    0,180,   0,220,0.5 'Help








