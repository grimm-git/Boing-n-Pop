' Game objects
' Code by "Matthias Grimm"

CONST STATE_INTRO=0
CONST STATE_CONFIG=1
CONST STATE_DEMO=2
CONST STATE_GAME=3
CONST STATE_DEAD=4
CONST STATE_PLAYER=5
CONST STATE_LEVEL=6
CONST STATE_OVER=7
CONST STATE_CUTSCENE=8
CONST STATE_VICTORY=9
CONST STATE_HISCORE=10


'Memory Management      8Bit                16Bit
CONST PAGE_DISPLAY=0   '$24000000-$2404AFFF $D0000000-$D0095FFF
CONST PAGE_TITLE=2     '                    $D012C000-$D01C1FFF
CONST PAGE_BUFFER=3    '$D0098000-$D00E2FFF $D01C2000-$D0257FFF
CONST PAGE_PLAYFIELD=9 '$D0260000-$D02AAFFF   assembled background
CONST PAGE_SPRITES=10  '$ 
CONST PAGE_TILESET=11  '$ 
CONST PAGE_BACKGND=12  '$D0344000-$D038EFFF   clean background
CONST PAGE_DEVICES=7   '                    $D041A000-$D04AFFFF

CONST REQ_NEWBALL=0
CONST STS_GATEOPEN=32

DIM Integer Game.State      ' Default=STATE_INTRO
DIM Integer Game.Requests   ' bitfield
DIM Integer Game.NumPlayers
DIM Float   Game.frameTime=1/75
DIM Float   Game.frameTimer
DIM Integer Game.Balls

[DBG] DIM Float   DBG.updateTime

sub Game.init()
  mode 8,8

  Screen.W=MM.HRES
  Screen.H=MM.VRES
  VP.X    =0
  VP.Y    =80
  VP.W    =Screen.W-VP.X
  VP.H    =Screen.H-VP.Y

  Controls.init
  Nunchuk.init
  Blocks.init
  Arena.init
  Ball.init
  Panel.init
  Hiscore.init
  Fruit.init
  Player.init
end sub

sub Game.loadAssets()
  LOCAL Integer n
  
  load data "img/sprites.raw",MM.Info(page address PAGE_SPRITES)

  cmap.init
  cmap.load "img/colors.cmap"

  Hiscore.load(HISCORE_NAME)

  mode 8,16
  page write PAGE_TITLE
  load png "img/title.png"
  page write PAGE_DEVICES
  load png "img/devices.png"

  page write PAGE_BUFFER
  page display PAGE_DISPLAY

  Settings.load "Boing'n'Pop!.cfg"
end sub

sub Game.new
  Game.State=STATE_INTRO
end sub

sub Game.start
  mode 8,8
  Game.Balls=10

'  playMOD "soundfx"

  cmap.activate
  page write PAGE_PLAYFIELD
  blit 0,0,0,0,SCREEN.W,SCREEN.H,PAGE_BACKGND,0

  page write PAGE_BUFFER
  page display PAGE_DISPLAY

  set Game.Requests,REQ_NEWBALL
end sub

sub Game.clrScreen
  Game.frameTimer=TIMER
  cls
end sub

sub Game.update
  Fruit.update
  Ball.update
end sub

sub Game.draw
  STATIC Integer stage

  Arena.draw
  Blocks.draw
  Fruit.draw
  Panel.draw
  Ball.draw

  if tst(Game.Requests,REQ_NEWBALL) then
    if NOT tst(Game.Requests,STS_GATEOPEN) then
      if Arena.openGate()=1 then
        set Game.Requests,STS_GATEOPEN
        select case Arena.Gate.Side
        case 0
          Ball.reset(VP.X,Arena.Gate.Y-Arena.Gate.W/2)
          Ball.vX=300 : Ball.vY=-GRAVITY*Ball.dt/100
        case 1
          Ball.reset(Arena.Gate.X+Arena.Gate.W/2,VP.Y)
          Ball.vY=1
        case 2
          Ball.reset(VP.X+VP.W,Arena.Gate.Y-Arena.Gate.W/2)
          Ball.vX=-300 : Ball.vY=-GRAVITY*Ball.dt/100
        end select
        if Game.Balls>0 then inc Game.Balls,-1
        Ball.move()
      endif
    elseif Arena.closeGate()=0 then
      clr Game.Requests,STS_GATEOPEN
      clr Game.Requests,REQ_NEWBALL
    endif
  endif
end sub

sub Game.drawDashboard
  blit 500,350,Screen.W/2-32,14,64,64,PAGE_SPRITES,&B100
  text Screen.W/2,35,str$(Game.Balls),"C",,2,map(88),-1
  text 20,20,str$(Player.Score(0),6,0,"0"),"L",,5,Player.getColor(1),-1
  text Screen.W-20,20,str$(Player.Score(1),6,0,"0"),"R",,5,Player.getColor(2),-1
end sub

sub Game.swapPage
[DBG] DBG.updateTime=TIMER-Game.frameTimer
[DBG] Game.showFPS
  page copy PAGE_BUFFER to PAGE_DISPLAY,B
  Game.frameTime=TIMER-Game.frameTimer
end sub

Sub Game.showFPS
  text SCREEN.W-6,17,str$(int(1000/Game.frameTime)),"R",7,,,-1
  text SCREEN.W-6,27,str$(int(DBG.updateTime*10)/10,2,2)+"ms","R",7,,,-1
  text SCREEN.W-6,39,str$(cfr(1),2,2),"R",7,,,-1
end Sub




