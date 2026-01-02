'Panel data and function

CONST PANEL_DEFAULT_W=150
CONST PANEL_DEFAULT_H=10

CONST PANEL_ROTKICK=50
CONST PANEL_LATKICK=40

DIM Integer Panel.visible(MAX_PLAYER)   'visible=active
DIM Float   Panel.X(MAX_PLAYER,1)       'last, current position
DIM Float   Panel.Y(MAX_PLAYER,1)   
DIM Float   Panel.phi(MAX_PLAYER,1)     'last, current angle
DIM Float   Panel.H(MAX_PLAYER)
DIM Float   Panel.W(MAX_PLAYER)
DIM Integer Panel.color(MAX_PLAYER)

DIM Float   Panel.mesh(MAX_PLAYER,9)    'base mesh definition
DIM Float   Panel.mesh_rt(MAX_PLAYER,9) 'rotated and tranlated mesh


'Initializes the environment for [num_panels] panels
'it can only be called once. It is save to call it again but it will not set
' a new number of panels. It will just reset all existing ones.
sub Panel.init()
  LOCAL Integer idx

  for idx=1 to MAX_PLAYER
    Panel.reset(idx)
  next
end sub

'panel=1..n
sub Panel.reset(panel%)
  STATIC Integer colors(7)=(rgb(red),rgb(green),rgb(blue),rgb(yellow),rgb(cyan),rgb(magenta),rgb(brown),rgb(white))
  LOCAL Integer idx=panel%-1
 
  Panel.visible(idx)=0
  Panel.X(idx,0)    =0
  Panel.Y(idx,0)    =0
  Panel.phi(idx,0)  =0
  Panel.color(idx)  =colors(idx MOD 8)

  Panel.setSize panel%,PANEL_DEFAULT_W,PANEL_DEFAULT_H
end sub

'panel=1..n
sub Panel.setSize(panel%,w!,h!)
  LOCAL Float mesh(9)=(-w!/2,-h!/2,w!/2,-h!/2,w!/2,h!/2,-w!/2,h!/2,-w!/2,-h!/2)
  LOCAL Integer n,idx=panel%-1

  Panel.W(idx)=w!
  Panel.H(idx)=h!

  for n=0 to 9
    Panel.mesh(idx,n)=mesh(n)
  next n
end sub

'panel=1..n
function Panel.getWidth(panel%) as Float
  Panel.getWidth=Panel.W(panel%-1)
end function

'panel=1..n
sub Panel.setColor(panel%,color%)
  Panel.color(panel%-1)=color%
end sub

sub Panel.setVisible(panel%,visible%)
  Panel.visible(panel%-1)=choice(visible%>0,1,0)
end sub

function Panel.isVisibe(panel%) as Integer
  Panel.isVisible=Panel.visible(panel%-1)
end function

'panel=1..n, phi in RADIANS
'saves the old position
sub Panel.move(panel%,posX!,posY!,phi!)
  LOCAL Integer n,idx=panel%-1
  LOCAL FLOAT wlim=RAD(6), ylim=Ball.R
  LOCAL Float wn,wo,yn,yo
  LOCAL Float x,y

  wn=phi! : wo=Panel.phi(idx,1)
  if wn-wo > wlim then wn=wo+wlim   'Limit of w to 220 Grad/s
  if wo-wn > wlim then wn=wo-wlim

  yn=posY! : yo=Panel.Y(idx,1)
  if Panel.Y(idx,0)>0 then
    if yn-yo > ylim then yn=yo+wlim   'Limit to ball radius
    if yo-yn > ylim then yn=yo-wlim
  endif

  Panel.X(idx,0)=Panel.X(idx,1) : Panel.X(idx,1)=posX!
  Panel.Y(idx,0)=Panel.Y(idx,1) : Panel.Y(idx,1)=yn
  Panel.phi(idx,0)=Panel.phi(idx,1) : Panel.phi(idx,1)=wn

  for n=0 to 8 step 2
    x=Panel.mesh(idx,n) : y=Panel.mesh(idx,n+1)
    Panel.mesh_rt(idx,n)  =Panel.X(idx,1) + x*cos(wn) - y*sin(wn)
    Panel.mesh_rt(idx,n+1)=Panel.Y(idx,1) + x*sin(wn) + y*cos(wn)
  next n
end sub

'Draws all visible panels on the screen
sub Panel.draw()
  LOCAL Float x(4),y(4)
  LOCAL Integer panel,n
  LOCAL Float a,b

  for panel=0 to MAX_PLAYER-1
    if Panel.visible(panel)=1 then
      for n=0 to 4
        x(n)=Panel.mesh_rt(panel,2*n)
        y(n)=Panel.mesh_rt(panel,2*n+1)
      next n
      polygon 5,x(),y(),Panel.color(panel),Panel.color(panel)
    endif
  next panel
end sub

sub Panel.bounce()
  LOCAL Integer idx,s
  LOCAL Float x1,x2,y1,y2,sx1,sx2,sy1,sy2
  LOCAL Float d,dmin,lever
  LOCAL Float tx,ty,slen,nx,ny,u
  LOCAL Float wx,wy,proj,dx,dy,dist
  LOCAL Float omega,push

  for idx=0 to MAX_PLAYER-1
    if Panel.visible(idx)=0 then continue for

    dmin=99999
    for s=0 to 6 step 2
      x1=Panel.mesh_rt(idx,s+0) : y1=Panel.mesh_rt(idx,s+1)
      x2=Panel.mesh_rt(idx,s+2) : y2=Panel.mesh_rt(idx,s+3)
      d=Math.distanceToLine(Ball.X,Ball.Y,x1,y1,x2,y2)
      if d<dmin then dmin=d : sx1=x1 : sy1=y1 : sx2=x2 : sy2=y2
    next

    omega=Panel.phi(idx,1)-Panel.phi(idx,0) 'panel totation speed
    push=Panel.Y(idx,1)-Panel.Y(idx,0)      'panel vertical movement
    wx=Ball.X-sx1 : wy=Ball.Y-sy1
 
    if Ball.bounce(sx1,sy1,sx2,sy2,REBOUNCE,FRICTION) then
      Ball.Owner=idx+1

      tx=sx2-sx1 : ty=sy2-sy1 : slen=SQR(tx*tx+ty*ty)
      if slen=0 then exit sub

      tx=tx/slen : ty=ty/slen  'Segmentvektor und Tangente
      nx=-ty     : ny=tx       'Standard-Normalenvektor (senkrecht zur Tangente)

      'Naechsten Punkt auf dem Segment finden
      proj=wx*tx+wy*ty  'dot-product, Projektion auf die Tangente
      u=proj/slen       'Parameter entlang des Segments [0..1]

      'Auf das Segment begrenzen
      if proj < 0 then proj = 0
      if proj > slen then proj = slen
      dx=wx-tx*proj : dy=wy-ty*proj : dist=SQR(dx*dx+dy*dy)

      if (u <= 0) or (u >= 1) then
        if dist > 0 then nx=dx/dist : ny=dy/dist
      else
        if (dx*nx + dy*ny) < 0 then nx=-nx : ny=-ny
      endif
      
      if u<0 then u=0
      if u>1 then u=1
      lever=abs(2*u-1)
      
      inc Ball.vX,nx*lever*PANEL_ROTKICK
      inc Ball.vY,ny*lever*PANEL_ROTKICK+push*PANEL_LATKICK

      exit sub
    endif
  next
end sub

