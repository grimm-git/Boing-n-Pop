' Level class
' Code by "Matthias Grimm"

sub Level.init()
end sub

sub Level.prepare(level%)
  LOCAL Integer no

  Blocks.init
  Arena.init

'  Level.addSmallSquares(40,rgb(red))
  Level.addTestObjects
end sub

'Draw only dynamic parts of the level
sub Level.draw()
  Blocks.draw
end sub

sub Level.addSmallSquares(num%,color%)
  LOCAL Integer no,n

  for n=0 to num%
    no=Blocks.addRect(15,15)
    Blocks.setColor(no,color%,color%)
    Blocks.position(no,10+RND()*(Screen.W-20),10+RND()*SCREEN.H/3,RND()*PI)
  next
end sub

sub Level.addTestObjects()
  LOCAL Integer no,n

  no=Blocks.addRect(100,20)
  Blocks.setColor(no,rgb(brown),&HDD8822)
  Blocks.position(no,VP.Y+VP.H/4,350,pi/4)

  no=Blocks.addRect(100,20)
  Blocks.setColor(no,rgb(green),rgb(green))
  Blocks.position(no,VP.X+VP.W*3/4,350,-pi/4)

  no=Blocks.addTriangle(60,60,60)
  Blocks.setColor(no,rgb(yellow),rgb(blue))
  Blocks.position(no,VP.X+VP.W/2+10,200,pi/2)
end sub



