' Contains code to use the extended functions of a Nunchuk like analog joysticks
' and the accelerometers

CONST NCHK_MAX=3

CONST NCHK_X=0
CONST NCHK_Y=1
CONST NCHK_Z=2

CONST NCHK_LOW=0
CONST NCHK_CENTER=1
CONST NCHK_HIGH=2
CONST NCHK_DIV=3
  
CONST NCHK_SCALE_JOYX=0
CONST NCHK_SCALE_JOYY=1
CONST NCHK_SCALE_GCOX=2
CONST NCHK_SCALE_GCOY=3
CONST NCHK_SCALE_GCOZ=4

'Nunchuck channels have the address 1..NUNCHUK_MAX.
'The channel number 0 is reserved

DIM Integer Nunchuk.cal.jox(NCHK_MAX,3)  'Analog min,center,max,dif for X
DIM Integer Nunchuk.cal.joy(NCHK_MAX,3)  'Analog min,center,max,dif for Y
DIM Integer Nunchuk.cal.ag0(NCHK_MAX,2)  'a=0g for X,Y,Z
DIM Integer Nunchuk.cal.ag1(NCHK_MAX,2)  'a=1g for X,Y,Z
DIM Integer Nunchuk.cal.dif(NCHK_MAX,2)  'ag1-ag0
DIM Float   Nunchuk.g_vector(NCHK_MAX,2) 'part of the acceration that is caused by g (X,Y,Z)
DIM Float   Nunchuk.vector(NCHK_MAX,2)   'acceleration vector (X,Y,Z)
DIM Float   Nunchuk.roll(NCHK_MAX)       'roll angle of acceleration in RAD
DIM Float   Nunchuk.pitch(NCHK_MAX)      'pitch angle of accelerationm in RAD
DIM Float   Nunchuk.scales(NCHK_MAX,NCHK_SCALE_GCOZ)

DIM Integer Nunchuk.stableTime
DIM Integer Nunchuk.cal_mask(NCHK_MAX)
DIM Integer Nunchuk.cal_x(NCHK_MAX,2)  'temp values during calibration
DIM Integer Nunchuk.cal_y(NCHK_MAX,2)


' Initialize extra functions, reading calibration data from Nunchuks
' expects that the Nunchuks in use have been opened already. This
' should have already been done by Controls.inc
sub Nunchuk.init()
  LOCAL Integer nun

  for nun=1 to NCHK_MAX
    ON ERROR SKIP
    Nunchuk.cal.ag0(nun,NCHK_X)=Nunchuk(AX0,nun)
    if MM.ERRNO=0 then
      Nunchuk.cal.ag1(nun,NCHK_X)=Nunchuk(AX1,nun)
      Nunchuk.cal.ag0(nun,NCHK_Y)=Nunchuk(AY0,nun)
      Nunchuk.cal.ag1(nun,NCHK_Y)=Nunchuk(AY1,nun)
      Nunchuk.cal.ag0(nun,NCHK_Z)=Nunchuk(AZ0,nun)
      Nunchuk.cal.ag1(nun,NCHK_Z)=Nunchuk(AZ1,nun)
      Nunchuk.cal.jox(nun,NCHK_LOW)=Nunchuk(JXL,nun)
      Nunchuk.cal.jox(nun,NCHK_CENTER)=Nunchuk(JXC,nun)
      Nunchuk.cal.jox(nun,NCHK_HIGH)=Nunchuk(JXR,nun)
      Nunchuk.cal.joy(nun,NCHK_HIGH)=Nunchuk(JYT,nun)
      Nunchuk.cal.joy(nun,NCHK_CENTER)=Nunchuk(JYC,nun)
      Nunchuk.cal.joy(nun,NCHK_LOW)=Nunchuk(JYB,nun)
    endif
    Nunchuk.resetScales nun
    Nunchuk.calcDiffs nun
  next nun
end sub

sub Nunchuk.calcDiffs(nun%)
  Nunchuk.cal.dif(nun%,NCHK_X)=Nunchuk.cal.ag1(nun%,NCHK_X)-Nunchuk.cal.ag0(nun%,NCHK_X)
  Nunchuk.cal.dif(nun%,NCHK_Y)=Nunchuk.cal.ag1(nun%,NCHK_Y)-Nunchuk.cal.ag0(nun%,NCHK_Y)
  Nunchuk.cal.dif(nun%,NCHK_Z)=Nunchuk.cal.ag1(nun%,NCHK_Z)-Nunchuk.cal.ag0(nun%,NCHK_Z)
  Nunchuk.cal.jox(nun%,NCHK_DIV)=Nunchuk.cal.jox(nun%,NCHK_HIGH)-Nunchuk.cal.jox(nun%,NCHK_LOW)
  Nunchuk.cal.joy(nun%,NCHK_DIV)=Nunchuk.cal.joy(nun%,NCHK_HIGH)-Nunchuk.cal.joy(nun%,NCHK_LOW)
end sub

' recalibrate accelerometer 1g, factory settings migh be too inaccurate
' returns mask of learning 1g values/axles
' 0000 = x,y,z
' 0111 = all axles learning
' 1111 = calibration stable and completed
function Nunchuk.calibrateACC(nun%) as Float
  STATIC Integer init=1
  LOCAL Integer mask,dummy
  LOCAL Float value

  if nun%=0 then Nunchuk.calibrateACC=&B111 : exit function

  if init=1 then
    Nunchuk.stableTime=7000
    _resetCalibration nun%,0
    init=0
  endif
    
  mask=Nunchuk.cal_mask(nun%)

  value=Nunchuk(AX,nun%)
  value=_limitInt(value,Nunchuk(AX1,nun%)*1.1)
  mask=mask and &B011 or (_maxInt(value,Nunchuk.cal_x(nun%,NCHK_X),0) << 2)
  dummy=_minInt(value,Nunchuk.cal_y(nun%,NCHK_X),0)

  value=Nunchuk(AY,nun%)
  value=_limitInt(value,Nunchuk(AY1,nun%)*1.1)
  mask=mask and &B101 or (_maxInt(value,Nunchuk.cal_x(nun%,NCHK_Y),1) << 1)
  dummy=_minInt(value,Nunchuk.cal_y(nun%,NCHK_Y),1)

  value=Nunchuk(AZ,nun%)
  value=_limitInt(value,Nunchuk(AZ1,nun%)*1.1)
  mask=mask and &B110 or _maxInt(value,Nunchuk.cal_x(nun%,NCHK_Z),2)
  dummy=_minInt(value,Nunchuk.cal_y(nun%,NCHK_Z),2)

  if mask=&B111 then
    Nunchuk.cal.ag1(nun%,NCHK_X)=Nunchuk.cal_x(nun%,NCHK_X)
    Nunchuk.cal.dif(nun%,NCHK_X)=Nunchuk.cal.ag1(nun%,NCHK_X)-Nunchuk.cal.ag0(nun%,NCHK_X)
    Nunchuk.cal.ag1(nun%,NCHK_Y)=Nunchuk.cal_x(nun%,NCHK_Y)
    Nunchuk.cal.dif(nun%,NCHK_Y)=Nunchuk.cal.ag1(nun%,NCHK_Y)-Nunchuk.cal.ag0(nun%,NCHK_Y)
    Nunchuk.cal.ag1(nun%,NCHK_Z)=Nunchuk.cal_x(nun%,NCHK_Z)
    Nunchuk.cal.dif(nun%,NCHK_Z)=Nunchuk.cal.ag1(nun%,NCHK_Z)-Nunchuk.cal.ag0(nun%,NCHK_Z)
    init=1
  endif

  Nunchuk.cal_mask(nun%)=mask
  Nunchuk.calibrateACC=mask
end function

'returns mask of learned axis
' 000000 = left, center, right, top, middle, bottom
' 010010 = center positions learned
' 111111 = calibration complete
function Nunchuk.calibrateJoy(nun%) as Integer
  STATIC Integer init=1
  LOCAL Float vx,vy
  LOCAL Integer mask

  if nun%=0 then Nunchuk.calibrateJoy=&B111111 : exit function

  if init=1 then
    Nunchuk.stableTime=2000
    _resetCalibration nun%,128
    init=0
  endif

  mask=Nunchuk.cal_mask(nun%)
  if mask=0 then  'learn center positions
    vx=Nunchuk(JX,nun%) : vy=Nunchuk(JY,nun%)
    if _isJoyCentered(vx) and _isJoyCentered(vy) then
      Nunchuk.cal_x(nun%,NCHK_CENTER)=vx
      Nunchuk.cal_y(nun%,NCHK_CENTER)=vy
      mask=mask or &B010010
    endif
  else
    if (mask and &B010000)>0 then
      vx=Nunchuk(JX,nun%)
      if not _isJoyCentered(vx) then
        mask=mask and &B110111 or _maxInt(vx,Nunchuk.cal_x(nun%,NCHK_HIGH)) << 3
        mask=mask and &B011111 or _minInt(vx,Nunchuk.cal_x(nun%,NCHK_LOW))  << 5
      endif
    endif
    if (mask and &B000010)>0 then
      vy=Nunchuk(JY,nun%)
      if not _isJoyCentered(vy) then
        mask=mask and &B111011 or _maxInt(vy,Nunchuk.cal_y(nun%,NCHK_HIGH),1) << 2
        mask=mask and &B111110 or _minInt(vy,Nunchuk.cal_y(nun%,NCHK_LOW),1)
      endif  
    endif
  endif

  if mask=&B111111 then
    Nunchuk.cal.jox(nun%,NCHK_LOW)   =Nunchuk.cal_x(nun%,NCHK_LOW)
    Nunchuk.cal.jox(nun%,NCHK_CENTER)=Nunchuk.cal_x(nun%,NCHK_CENTER)
    Nunchuk.cal.jox(nun%,NCHK_HIGH)  =Nunchuk.cal_x(nun%,NCHK_HIGH)
    Nunchuk.cal.jox(nun%,NCHK_DIV)   =Nunchuk.cal_x(nun%,NCHK_HIGH)-Nunchuk.cal_x(nun%,NCHK_LOW)
    Nunchuk.cal.joy(nun%,NCHK_LOW)   =Nunchuk.cal_y(nun%,NCHK_LOW)
    Nunchuk.cal.joy(nun%,NCHK_CENTER)=Nunchuk.cal_y(nun%,NCHK_CENTER)
    Nunchuk.cal.joy(nun%,NCHK_HIGH)  =Nunchuk.cal_y(nun%,NCHK_HIGH)
    Nunchuk.cal.joy(nun%,NCHK_DIV)   =Nunchuk.cal_y(nun%,NCHK_HIGH)-Nunchuk.cal_y(nun%,NCHK_LOW)
    init=1
  endif

  Nunchuk.cal_mask(nun%)=mask
  Nunchuk.calibrateJoy=mask
end function

sub Nunchuk.resetScales(nun%)
  LOCAL Integer id

  for id=0 to bound(Nunchuk.scales(),2)-1
    Nunchuk.scales(nun%,id)=1
  next
end sub

'nun%=1..3, id% = NUNCHUK_SCALE_*
sub Nunchuk.setScale(nun%,id%,scale!)
  Nunchuk.scales(nun%,id%)=scale!
end sub

'return: -1..1, 0=center, output scaled with user factor
function Nunchuk.getJoyX(nun%) as Float
  LOCAL Float value

  if nun%<=0 or nun%>NCHK_MAX then exit function

  value=Nunchuk(JX,nun%)-Nunchuk.cal.jox(nun%,NCHK_LOW)
  value=2*value/Nunchuk.cal.jox(nun%,NCHK_DIV)-1
  Nunchuk.getJoyX=value*Nunchuk.scales(nun%,NCHK_SCALE_JOYX)
end function

'return: -1..1, 0=center, output scaled with user factor
function Nunchuk.getJoyY(nun%) as Float
  LOCAL Float value

  if nun%<=0 or nun%>NCHK_MAX then exit function

  value=Nunchuk(JY,nun%)-Nunchuk.cal.joy(nun%,NCHK_LOW)
  value=2*value/Nunchuk.cal.joy(nun%,NCHK_DIV)-1
  Nunchuk.getJoyY=value*Nunchuk.scales(nun%,NCHK_SCALE_JOYY)
end function

'return: -1..1 = -1g..1g, 0=center, values >|1g| are possible
function Nunchuk.getAccX(nun%) as Float
  LOCAL Float value

  value=Nunchuk(AX,nun%)-Nunchuk.cal.ag0(nun%,NCHK_X)
  Nunchuk.getAccX=value/Nunchuk.cal.dif(nun%,NCHK_X)
end function

'return: -1..1 = -1g..1g, 0=center, values >|1g| are possible
function Nunchuk.getAccY(nun%) as Float
  LOCAL Float value

  value=Nunchuk(AY,nun%)-Nunchuk.cal.ag0(nun%,NCHK_Y)
  Nunchuk.getAccY=value/Nunchuk.cal.dif(nun%,NCHK_Y)
end function

'return: -1..1 = -1g..1g, 0=center, values >|1g| are possible
function Nunchuk.getAccZ(nun%) as Float
  LOCAL Float value

  value=Nunchuk(AZ,nun%)-Nunchuk.cal.ag0(nun%,NCHK_Z)
  Nunchuk.getAccZ=value/Nunchuk.cal.dif(nun%,NCHK_Z)
end function

sub Nunchuk.compG(nun%,ax!,ay!,az!)
  LOCAL Float roll,pitch

  Nunchuk.vector(nun%,NCHK_X)=ax!  '-> -1..1
  Nunchuk.vector(nun%,NCHK_Y)=ay!
  Nunchuk.vector(nun%,NCHK_Z)=az!

  pitch=atan2(ay!,az!)
  Nunchuk.pitch(nun%)=pitch
  roll=atan2(ax!,SQR(ay!*ay! +az!*az!))
  Nunchuk.roll(nun%)=roll

  Nunchuk.g_vector(nun%,NCHK_X)=sin(roll)            'g is normalized to 1
  Nunchuk.g_vector(nun%,NCHK_Y)=sin(pitch)*cos(roll) 'therefore, it is removed from
  Nunchuk.g_vector(nun%,NCHK_Z)=cos(roll)*cos(pitch) 'these fomulas
end sub

'output scaled with user factor
function Nunchuk.getCompGX(nun%) as Float
  LOCAL Float value

  value=-Nunchuk.vector(nun%,NCHK_X)+Nunchuk.g_vector(nun%,NCHK_X)
  Nunchuk.getCompGX=value*Nunchuk.scales(nun%,NCHK_SCALE_GCOX)
end function

'output scaled with user factor
function Nunchuk.getCompGY(nun%) as Float
  LOCAL Float value

  value=Nunchuk.vector(nun%,NCHK_Y)-Nunchuk.g_vector(nun%,NCHK_Y)
  Nunchuk.getCompGY=value*Nunchuk.scales(nun%,NCHK_SCALE_GCOY)
end function

'output scaled with user factor
function Nunchuk.getCompGZ(nun%) as Float
  LOCAL Float value

  value=Nunchuk.vector(nun%,NCHK_Z)-Nunchuk.g_vector(nun%,NCHK_Z)
  Nunchuk.getCompGZ=value*Nunchuk.scales(nun%,NCHK_SCALE_GCOZ)
end function

function Nunchuk.getRoll(nun%) as Float
  Nunchuk.getRoll=Nunchuk.roll(nun%)
end function

function Nunchuk.getPitch(nun%) as Float
  Nunchuk.getPitch=Nunchuk.pitch(nun%)
end function

' value2% = max(value1%,value2%)
' return = max value stable
function _maxInt(value1%,value2%,slot%) as Integer
  STATIC Float stable(5)
  if value1%>value2% then value2%=value1% : stable(slot%)=TIMER
  if TIMER-stable(slot%)>Nunchuk.stableTime then _maxInt=1
end function

' value2% = min(value1%,value2%)
' return = min value stable
function _minInt(value1%,value2%,slot%) as Integer
  STATIC Float stable(5)
  if value1%<value2% then value2%=value1% : stable(slot%)=TIMER
  if TIMER-stable(slot%)>Nunchuk.stableTime then _minInt=1
end function

function _limitInt(value%,tolval%) as Integer
  _limitInt=choice(value%<tolval%,value%,0)
end function

function _isJoyCentered(value!) as Float
  _isJoyCentered=ABS(value!-128)<10
end function

sub _resetCalibration(nun%,init%)
  LOCAL Integer n

  Nunchuk.cal_mask(nun%)=0
  for n=0 to 2
    Nunchuk.cal_x(nun%,n)=init%
    Nunchuk.cal_y(nun%,n)=init%
  next
end sub

