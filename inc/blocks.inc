'Data and function for obstacle blocks

CONST BLOCKS_MAX=20

CONST BLOCK_EDGE=0
CONST BLOCK_RECT=1
CONST BLOCK_TRIANGLE=2
CONST BLOCK_CIRCLE=3

DIM Integer Blocks.active(BLOCKS_MAX)
DIM Integer Blocks.type(BLOCKS_MAX)
DIM Float   Blocks.X(BLOCKS_MAX)
DIM Float   Blocks.Y(BLOCKS_MAX)
DIM Float   Blocks.phi(BLOCKS_MAX)

DIM Integer Blocks.fcolor(BLOCKS_MAX) '-1=transparent
DIM Integer Blocks.bcolor(BLOCKS_MAX)
DIM Integer Blocks.cdoff(BLOCKS_MAX)   '1=collision detection disabled

DIM Float   Blocks.mesh(BLOCKS_MAX,9)    'base mesh definition
DIM Float   Blocks.mesh_rt(BLOCKS_MAX,9) 'rotated and tranlated mesh
DIM Integer Blocks.next

sub Blocks.init()
  LOCAL Integer n

  Blocks.next=0
  for n=0 to BLOCKS_MAX-1
    Blocks.active(n)=0
    Blocks.bcolor(n)=map(n+1 MOD 8)
    Blocks.fcolor(n)=map(n+1 MOD 8)
  next n
end sub

sub Blocks.lock(blk%)
  if blk%<0 or blk%>=BLOCKS_MAX then exit sub
  Blocks.cdoff(blk%)=1
end sub

sub Blocks.unlock(blk%)
  if blk%<0 or blk%>=BLOCKS_MAX then exit sub
  Blocks.cdoff(blk%)=0
end sub

'return block slot number
function Blocks.addEdge(w!) as Integer
  LOCAL Integer idx

  idx=Blocks.next
  Blocks.type(idx)=BLOCK_EDGE
  Blocks.mesh(idx,0)=-w!/2
  Blocks.mesh(idx,1)=0
  Blocks.mesh(idx,2)=w!/2
  Blocks.mesh(idx,3)=0

  Blocks.addEdge=idx
  inc idx : if idx<BLOCKS_MAX then Blocks.next=idx
end function

'return block slot number
function Blocks.addRect(w!,h!) as Integer
  LOCAL Integer idx

  idx=Blocks.next
  Blocks.type(idx)=BLOCK_RECT
  Blocks.mesh(idx,0)=-w!/2
  Blocks.mesh(idx,1)=-h!/2
  Blocks.mesh(idx,2)=w!/2
  Blocks.mesh(idx,3)=-h!/2
  Blocks.mesh(idx,4)=w!/2
  Blocks.mesh(idx,5)=h!/2
  Blocks.mesh(idx,6)=-w!/2
  Blocks.mesh(idx,7)=h!/2
  Blocks.mesh(idx,8)=Blocks.mesh(idx,0)
  Blocks.mesh(idx,9)=Blocks.mesh(idx,1)

  Blocks.addRect=idx
  inc idx : if idx<BLOCKS_MAX then Blocks.next=idx
end function

function Blocks.addTriangle(s1!,s2!,s3!) as Integer
  LOCAL Float xc,yc
  LOCAL Integer n,idx

  idx=Blocks.next
  xc=(s2!*s2!+s3!*s3!-s1!*s1!)/2/s3!
  yc=SQR(s2!*s2!-xc*xc)

  Blocks.type(idx)=BLOCK_TRIANGLE
  Blocks.mesh(idx,0)=-(s3!+xc)/3
  Blocks.mesh(idx,1)=-yc/3
  Blocks.mesh(idx,2)=(2*s3!-xc)/3
  Blocks.mesh(idx,3)=-yc/3
  Blocks.mesh(idx,4)=(2*xc-s3!)/3
  Blocks.mesh(idx,5)=2*yc/3
  Blocks.mesh(idx,6)=Blocks.mesh(idx,0)
  Blocks.mesh(idx,7)=Blocks.mesh(idx,1)

  Blocks.addTriangle=idx
  inc idx : if idx<BLOCKS_MAX then Blocks.next=idx
end function

'blk=0..BLOCKS_MAX
sub Blocks.setColor(blk%,fcolor%,bcolor%)
  Blocks.fcolor(blk%)=fcolor%
  Blocks.bcolor(blk%)=bcolor%
end sub

'blk=0..BLOCKS_MAX, phi in RADIANS
sub Blocks.position(blk%,posX!,posY!,phi!)
  LOCAL Integer n,points
  LOCAL Float x,y

  if blk%<0 or blk%>=BLOCKS_MAX then exit sub

  Blocks.active(blk%)=1
  Blocks.X(blk%)=posX!
  Blocks.Y(blk%)=posY!
  Blocks.phi(blk%)=phi!
  
  select case Blocks.type(blk%)
  case BLOCK_EDGE
    for n=0 to 4 step 2
      x=Blocks.mesh(blk%,n) : y=Blocks.mesh(blk%,n+1)
      Blocks.mesh_rt(blk%,n)  =posX! + x*cos(phi!) - y*sin(phi!)
      Blocks.mesh_rt(blk%,n+1)=posY! + x*sin(phi!) + y*cos(phi!)
    next n
  case BLOCK_RECT,BLOCK_TRIANGLE
    points=choice(Blocks.type(blk%)=BLOCK_RECT,8,6)
    for n=0 to points step 2
      x=Blocks.mesh(blk%,n) : y=Blocks.mesh(blk%,n+1)
      Blocks.mesh_rt(blk%,n)  =posX! + x*cos(phi!) - y*sin(phi!)
      Blocks.mesh_rt(blk%,n+1)=posY! + x*sin(phi!) + y*cos(phi!)
    next n
  case BLOCK_CIRCLE
  end select
end sub

sub Blocks.remove(blk%)
  LOCAL Integer n,last=Blocks.next-1

  if blk%<0 or blk%>=BLOCKS_MAX then exit sub
  Blocks.active(blk%)=0
end sub

'Draws all visible blocks on the screen
sub Blocks.draw()
  LOCAL Float x(4),y(4)
  LOCAL Integer blk,n,points

[PHY] exit sub

  for blk=0 to Blocks.next-1
    if Blocks.active(blk)=1 then
      if Blocks.fcolor(blk)>-1 then  'Block is transparent
        select case Blocks.type(blk)
        case BLOCK_EDGE
          x(0)=Blocks.mesh_rt(blk,0) : y(0)=Blocks.mesh_rt(blk,1)
          x(1)=Blocks.mesh_rt(blk,2) : y(1)=Blocks.mesh_rt(blk,3)
          line x(0),y(0),x(1),y(1),1,Blocks.fcolor(blk)

        case BLOCK_RECT,BLOCK_TRIANGLE
          points=choice(Blocks.type(blk)=BLOCK_RECT,4,3)
          for n=0 to points
            x(n)=Blocks.mesh_rt(blk,2*n)
            y(n)=Blocks.mesh_rt(blk,2*n+1)
          next n
          polygon points+1,x(),y(),Blocks.fcolor(blk),Blocks.bcolor(blk)

        case BLOCK_CIRCLE
        end select
      endif
    endif
  next
end sub

function Blocks.bounce() as Integer
  LOCAL Float x1,y1,x2,y2,sx1,sy1,sx2,sy2
  LOCAL Integer blk,s
  LOCAL Float d,dmin

  for blk=0 to Blocks.next-1
    if Blocks.cdoff(blk) then continue for

    dmin=9999
    select case Blocks.type(blk)
    case BLOCK_EDGE
      sx1=Blocks.mesh_rt(blk,0) : sy1=Blocks.mesh_rt(blk,1)
      sx2=Blocks.mesh_rt(blk,2) : sy2=Blocks.mesh_rt(blk,3)

    case BLOCK_TRIANGLE
      for s=0 to 4 step 2
        x1=Blocks.mesh_rt(blk,s+0) : y1=Blocks.mesh_rt(blk,s+1)
        x2=Blocks.mesh_rt(blk,s+2) : y2=Blocks.mesh_rt(blk,s+3)
        d=Math.distanceToLine(Ball.X,Ball.Y,x1,y1,x2,y2)
        if d<dmin then dmin=d : sx1=x1 : sy1=y1 : sx2=x2 : sy2=y2
      next

    case BLOCK_RECT
      for s=0 to 6 step 2
        x1=Blocks.mesh_rt(blk,s+0) : y1=Blocks.mesh_rt(blk,s+1)
        x2=Blocks.mesh_rt(blk,s+2) : y2=Blocks.mesh_rt(blk,s+3)
        d=Math.distanceToLine(Ball.X,Ball.Y,x1,y1,x2,y2)
        if d<dmin then dmin=d : sx1=x1 : sy1=y1 : sx2=x2 : sy2=y2
      next

    case BLOCK_CIRCLE
    end select

    if Ball.bounce(sx1,sy1,sx2,sy2,REBOUNCE,FRICTION) then
      Blocks.bounce=blk
      exit function
    endif
  next
  Blocks.bounce=-1
end function

