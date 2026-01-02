
CONST GRAVITY =980  'Gravity (px/s^2)
CONST REBOUNCE=0.8  'Restitution [0..1]
CONST FRICTION=0    'Friction [0..1]

'Public Vars
DIM Float   Ball.X
DIM Float   Ball.Y
DIM Float   Ball.vX  'Velocity, pixels/s
DIM Float   Ball.vY
DIM Integer Ball.R
DIM Integer Ball.Owner  'Player, who hit the ball last

DIM Float   Ball.dt

sub Ball.init()
  Ball.reset -32,0
  Ball.R=8
  Ball.dt=1/75
  Ball.Owner=0
end sub

sub Ball.draw()
  LOCAL Integer clut

  Ball.dt=cfr(1/75)
  clut=choice(Ball.Owner=0,7,Player.getCLUT(Ball.Owner))
  circle Ball.X,Ball.Y,Ball.R,0,1.0,map(clut),map(clut)
end sub

sub Ball.update()
end sub

sub Ball.move()
  ' 1) Gravitation
  inc Ball.vY,GRAVITY*Ball.dt

  ' 2) Vorhersage Position
  inc Ball.X,Ball.vX*Ball.dt
  inc Ball.Y,Ball.vY*Ball.dt
end sub

sub Ball.setPos(posX!,posY!)
  Ball.X=posX!
  Ball.Y=posY!
end sub

sub Ball.reset(posX!,posY!)
  Ball.setPos posX!,posY!
  Ball.vX=0
  Ball.vY=0
  Ball.Owner=0
end sub

function Ball.bounce(x1!,y1!,x2!,y2!,e!,mu!) as Integer
  LOCAL Float tx, ty, slen
  LOCAL Float nx, ny
  LOCAL Float wx, wy, proj, u
  LOCAL Float dx, dy, dist, penetration
  LOCAL Float vn, vtx, vty
  LOCAL float bx1,by1,bx2,by2,bnx,bny,bl

  tx=x2!-x1! : ty=y2!-y1! : slen=SQR(tx*tx+ty*ty)
  if slen=0 then exit function

  tx=tx/slen : ty=ty/slen  'Segmentvektor und Tangente
  nx=-ty     : ny=tx       'Standard-Normalenvektor (senkrecht zur Tangente)

  ' Naechsten Punkt auf dem Segment finden
  wx=Ball.X-x1! : wy=Ball.Y-y1!
  proj=wx*tx+wy*ty   'dot product, Projektion auf die Tangente
  u=proj/slen        'Parameter entlang des Segments [0..1]

  'Auf das Segment begrenzen
  if proj < 0 then proj = 0
  if proj > slen then proj = slen

  'Vektor vom naechsten Punkt zum Ballzentrum
  dx=Ball.X-x1!-tx*proj : dy=Ball.Y-y1!-ty*proj : dist=SQR(dx*dx+dy*dy)

  if (u <= 0) or (u >= 1) then
    'Falls der naechste Punkt ein Endpunkt ist (Eckkollision),
    'verwende die Richtung vom Eckpunkt zum Ball als Normale.
    if dist > 0 then nx=dx/dist : ny=dy/dist
  else
    'Fuer Treffpunkte auf dem Segment: Normalenrichtung so waehlen,
    'dass sie auf den Ball zeigt.
    if (dx*nx + dy*ny) < 0 then nx=-nx : ny=-ny
  endif

  'Ballvektor 
  bx1=Ball.X : bx2=bx1+Ball.vX*Ball.dt
  by1=Ball.Y : by2=by1+Ball.vY*Ball.dt
  bnx=bx2-bx1 : bny=by2-by1 : bl=SQR(bnx*bnx+bny*bny)
  if bl>0 then inc bx2,Ball.R*bnx/bl : inc by2,Ball.R*bny/bl

  'Kollision, wenn der Ball die Oberflaeche schneidet
  if dist<Ball.R or Math.intersect(bx1,by1,bx2,by2,x1!,y1!,x2!,y2!) then

    'Entpenetration (verschiebe den Ball auf Kontaktkreis)
    penetration = Ball.R - dist
    inc Ball.X,nx*penetration
    inc Ball.Y,ny*penetration

    'Normalkomponente der Geschwindigkeit
    vn=Ball.vX*nx+Ball.vY*ny   'dot product

    ' Nur reagieren, wenn wir in die Oberfaeche hineinfliegen
    if vn<0 then
      vtx=Ball.vX - vn*nx 'Tangentialkomponente
      vty=Ball.vY - vn*ny
      vn =-e!*vn          'Rueckprall auf die Normalkomponente

      'Reibung daempft die Tangentialkomponente
      'Neue Geschwindigkeit = Tangential + Normal
      Ball.vX = (1-mu!)*vtx + vn*nx
      Ball.vY = (1-mu!)*vty + vn*ny
      Ball.bounce=1
    endif
  endif
end function


