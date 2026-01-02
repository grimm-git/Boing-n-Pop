
CONST HISCORE_NUM=10
CONST HISCORE_NAME="hiscore.dat"

DIM String  Hiscore.name$(HISCORE_NUM)
DIM Integer Hiscore.score(HISCORE_NUM)

sub Hiscore.init()
  LOCAL Integer n,char,score

  char=65 : score=5000
  for n=0 to HISCORE_NUM-1
    Hiscore.name$(n)=chr$(char)+chr$(char)+chr$(char)
    Hiscore.score(n)=score
    inc char
    inc score,-500
  next
end sub

sub Hiscore.load(name$)
  LOCAL Integer idx
  LOCAL String line$,key$,value$

  on error skip 1
  open name$ for INPUT as #1
  if MM.ERRNO>0 then print MM.ERRMSG$ : exit sub

  do
    line input #1,line$
    if line$="" then exit do

    key$=field$(line$,1,",")
    value$=field$(line$,2,",")

    if len(key$)<>3 or val(value$)=0 then exit do
    Hiscore.name$(idx)=key$
    Hiscore.score(idx)=val(value$)
    inc idx : if idx>10 then exit do
  loop

  close #1
end sub

sub Hiscore.save(name$)
  LOCAL Integer idx

  open name$ for OUTPUT as #1

  for idx=0 to HISCORE_NUM-1
    print #1,Hiscore.name$(idx)+","+str$(Hiscore.score(idx))
  next

  close #1
end sub

function Hiscore.freePos(score%) as Integer
  LOCAL Integer n,idx

  if Hiscore.score(HISCORE_NUM-1) >= score% then
    Hiscore.freePos=-1
    exit function
  else
    idx=HISCORE_NUM-1
  endif

  for n=HISCORE_NUM-2 to 0 step -1
    if Hiscore.score(n)<score% then
      Hiscore.name$(n+1)=Hiscore.name$(n)
      Hiscore.score(n+1)=Hiscore.score(n)
      idx=n
    endif
  next

  Hiscore.name$(idx)="___"
  Hiscore.score(idx)=score%
  Hiscore.freePos=idx
end function


