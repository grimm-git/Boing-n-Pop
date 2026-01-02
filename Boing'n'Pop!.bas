
OPTION BASE 0
OPTION EXPLICIT
OPTION DEFAULT NONE

#DEFINE "[NUN]",""  'enable extra Nunchuk functions, requires nunchuk.inc
#DEFINE "[DBG]",""  'enable standard debugging output
#DEFINE "[MAT]","'"  'enable Math debugging
#DEFINE "[PHY]","'"  'switch to physical view mode

CONST PWD$=getParent$(MM.Info(current))
CONST MAX_PLAYER=2

#Include "inc/game.inc"
#Include "inc/controls.inc"
#Include "inc/nunchuk.inc"
#Include "inc/cmap.inc"      'colormap functions
#Include "inc/blocks.inc"
#Include "inc/arena.inc"
#Include "inc/ball.inc"
#Include "inc/panel.inc"
#Include "inc/hiscore.inc"
#Include "inc/settings.inc"
#Include "inc/fruit.inc"
#Include "inc/player.inc"
#Include "inc/math.inc"
#Include "inc/utils.inc"
#Include "inc/sound.inc"

#Include "inc/page_intro.inc"
#Include "inc/page_config.inc"
#Include "inc/page_hiscore.inc"


DIM Integer Screen.W,Screen.H
DIM Integer VP.X,VP.Y,VP.W,VP.H

game.init()
game.loadAssets()

'**********************************************************
'*                     Game Main Loop                     *
'**********************************************************
DIM Integer key,one,player,rc
DIM Integer inpch,ctrl,range
DIM Float   x,y
DIM Float   tim

DIM Integer nun,no,n,block
DIM Float cx,cy
DIM Float ax,ay,az

do
  Game.clrScreen()

'********************> State Handling <********************
  select case Game.State
  case STATE_INTRO
    if one=0 then one=1 : Intro.setup
    if isESC() then exit do
  
    Intro.draw
    Game.NumPlayers=Intro.control()
    if Game.NumPlayers>0 then
      cls
      Player.reset
      Ball.R=16-Intro.Difficulty*4
      Config.config Game.NumPlayers
      changeState(STATE_CONFIG)
      one=1
    endif

  case STATE_CONFIG
    if one=0 then one=1 : Config.setup
    if isESC() then changeState(STATE_INTRO)

    Config.draw
    if Config.control()=1 then
      select case Game.numPLayers
      case 1
        Player.position(1,Screen.W/2,Screen.H-30)
        Panel.setSize(1,150,10)
        range=Screen.W-20
      case 2
        Player.position(1,Screen.W/4,Screen.H-30)   'center position
        Panel.setSize(1,100,10)
        Player.position(2,3*Screen.W/4,Screen.H-30) 'center position
        Panel.setSize(2,100,10)
        range=Screen.W/2-30
      end select

      for player=1 to Game.NumPlayers
        Panel.setColor(player,map(Player.getCLUT(player)))
        Panel.setVisible(player,1)

        inpch=Player.getInpch(player)
        nun=Controls.getNunchuk(inpch)
        Nunchuk.setScale(nun,NCHK_SCALE_JOYX,(range-Panel.getWidth(player))/2)
        Nunchuk.setScale(nun,NCHK_SCALE_GCOZ,40)
      next

      Blocks.init
      Arena.init
      changeState(STATE_GAME)
    endif

  case STATE_GAME
    if one=0 then one=1 : Game.start : Player.newgame()
    if isESC() then cls : changeState(STATE_CONFIG)

    Game.update

    for player=1 to Game.NumPLayers
      inpch=Player.getInpch(player)
      nun=Controls.getNunchuk(inpch)
      ax=Nunchuk.getAccX(nun)
      ay=Nunchuk.getAccY(nun)
      az=Nunchuk.getAccZ(nun)
      Nunchuk.compG(nun,ax,ay,az)    'sets phi and compensated x,y,z

      cx=Player.getX(player)+Nunchuk.getJoyX(nun)
      cy=Player.getY(player)-Nunchuk.getCompGZ(nun)
      Panel.move(player,cx,cy,Nunchuk.getRoll(nun))
    next player

    block=Blocks.bounce()
    Panel.bounce

    'boundary scan
    if Ball.Y>SCREEN.H+Ball.R then
      if Game.Balls>0 then set(Game.Requests,REQ_NEWBALL) else changeState(STATE_OVER)
    else
      Ball.move()
      Fruit.hit()
    endif

    Game.draw
    Game.drawDashboard

    key=controls.readKey()
    if key=32 then Ball.vY=-5/Ball.dt

  case STATE_OVER
    if isESC() then changeState(STATE_CONFIG)

    Game.update
    Game.draw
    Game.drawDashboard

    text Screen.W/2, Screen.H/2,"Game Over!","C",5,,map(220),-1

    'timer zum Weiterschalten einbauen

    key=controls.readKey()
    if key=32 then changeState(STATE_HISCORE)

  case STATE_HISCORE
    if one=0 then one=1 : Hiscore.setup
    if isESC() then changeState(STATE_CONFIG)

    Hiscore.draw
    if Hiscore.control() then changeState(STATE_INTRO)

  end select
  
  Game.swapPage()
loop

'***********************> Game Exit Handling <***********************
settings.save()
hiscore.save(HISCORE_NAME)

mode 1
page write 0
print "Good Bye..."


sub changeState(newstate%)
  Game.State=newstate%
  playSample 14,22050,1
  one=0
end sub

function isESC() as Integer
  STATIC Integer screenshot=0,oldKey=0
  LOCAL Integer key

  key=Controls.readKey()
  if oldKey<>Key then
    oldKey=Key
    if key=27 then isESC=1 : exit function
  endif
end function



