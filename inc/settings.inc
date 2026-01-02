

DIM String Settings.filename

sub Settings.load(name$)
  LOCAL String line$,key$,num$
  LOCAL Integer n,value,calcdiffs

  Settings.filename=name$

  on error skip 1
  open Settings.name for INPUT as #1
  if MM.ERRNO>0 then print MM.ERRMSG$ : exit sub

  do
    line input #1,line$
    if line$="" then exit do

    key$=field$(line$,1,",")

    select case key
    case "difficulty"
      num$=field$(line$,2,",")
      Intro.Difficulty=val(num$)

    case "cal_nunchuk1"
      calcdiffs=1
      for n=0 to 2
        num$=field$(line$,n+2,",")
        Nunchuk.cal.jox(1,n)=val(num$)
      next
      for n=0 to 2
        num$=field$(line$,n+5,",")
        Nunchuk.cal.joy(1,n)=val(num$)
      next
      for n=0 to 2
        num$=field$(line$,n+8,",")
        Nunchuk.cal.ag1(1,n)=val(num$)
      next

    case "cal_nunchuk2"
      calcdiffs=1
      for n=0 to 2
        num$=field$(line$,n+2,",")
        Nunchuk.cal.jox(2,n)=val(num$)
      next
      for n=0 to 2
        num$=field$(line$,n+5,",")
        Nunchuk.cal.joy(2,n)=val(num$)
      next
      for n=0 to 2
        num$=field$(line$,n+8,",")
        Nunchuk.cal.ag1(2,n)=val(num$)
      next

    case "cal_nunchuk3"
      calcdiffs=1
      for n=0 to 2
        num$=field$(line$,n+2,",")
        Nunchuk.cal.jox(3,n)=val(num$)
      next
      for n=0 to 2
        num$=field$(line$,n+5,",")
        Nunchuk.cal.joy(3,n)=val(num$)
      next
      for n=0 to 2
        num$=field$(line$,n+8,",")
        Nunchuk.cal.ag1(3,n)=val(num$)
      next

    end select
  loop
  close #1

  if calcdiffs then
    for n=1 to 3 : Nunchuk.calcdiffs(n) : next
  endif

end sub

sub Settings.save()
  LOCAL Integer n,m

  open Settings.filename for OUTPUT as #1

  print #1,"difficulty,"+str$(Intro.Difficulty)

  for m=1 to 3
    print #1,"cal_nunchuk"+str$(m);
    for n=0 to 2 : print #1,","+str$(Nunchuk.cal.jox(m,n)); : next
    for n=0 to 2 : print #1,","+str$(Nunchuk.cal.joy(m,n)); : next
    for n=0 to 2 : print #1,","+str$(Nunchuk.cal.ag1(m,n)); : next
    print #1
  next

  close #1
end sub


