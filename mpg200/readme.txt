Here are the midi cc mappings:

Potmeters (data value 0-127)
12 - dco2 fine tune
13 - dco2 tune
14 - dco env amt
15 - dco lfo amt
16 - vcf source mix
17 - vcf hpf
18 - vcf resonance
19 - vcf cutoff freq
20 - vcf env mod
21 - vcf lfo mod
22 - vcf pitch follow
23 - vca level
24 - lfo rate
25 - lfo delay time
26 - env attack
27 - env decay
28 - env sustain
29 - env release

64
42 84
32 64 96

Switches
72 - dco1 range 
   0 -  41: 16'
  42 -  83: 8'
  84 - 127: 4'
73 - dco wave
   0 -  41: saw
  42 -  83: pwm
  84 - 127: square
74 - dco2 range
   0 -  41: 16'
  42 -  83: 8'
  84 - 127: 4'
75 - dco2 wave
   0 -  31: saw
  32 -  63: pwm
  64 -  95: square
  96 - 127: noise
76 - cross mod
   0 -  41: off
  42 -  83: sync
  84 - 127: metal
77 - vcf env polarity
   0 -  63: normal
  64 - 127: inverse
78 - vca mode
   0 -  63: envelope
  64 - 127: gate
79 - dco2 freq mod env
   0 -  63: on
  64 - 127: off
80 - dco2 freq mod lfo
   0 -  63: on
  64 - 127: off
81 - dco1 freq mod env
   0 -  63: on
  64 - 127: off
82 - dco1 freq mod lfo
   0 -  63: on
  64 - 127: off
83 - lfo waveform
   0 -  41: sine
  42 -  83: square
  84 - 127: random
84 - dco env polarity
   0 -  63: on
  64 - 127: off
85 - chorus
   0 -  63: on
  64 - 127: off



MPG-200 switch settings:
pg200: 
------
Only PG-200 messages are let through. Note on/off etc is transmitted. No switching between midi and pg200 modes 
is done, so messages are sent almost instantly. This is the only mode that works without modifying the jx-3p. 

If the jx-3p has not been modified, the switch on the jx-3p must be in the "Programmer" position for things to work.

safe mix:
---------
In this mode, the MPG-200 switches between midi and pg200 messages. Whenever the mpg-200 receives a cc message,
it switches to PG-200 mode and waits 20ms for the jx-3p to register the change. The message is then sent, and a timer is
set to count down 50ms. If no new cc message arrives within this time, the MPG-200 switches back to midi mode to be ready 
for the next midi message, and waits 25ms for the jx-3p to register the change. If any note on/off message arrives
when the MPG-200 is in PG-200 mode, the midi message is ignored, which may lead to hanging notes. While in pg-200 mode,
the MPG-200 treats CC messages as fast as possible. It is still, however, possible to overload the jx-3p if the
messages arrives to fast, but it handles quite a lot of messages without any problem.

If the jx-3p has not been modified, this mode will not work. If it HAS been modified, the switch on the jx-3p must be 
in the "Midi bus" position for things to work.

mix: 
----
This works the same way as safe mix, but does not wait after switching mode. Without additional modifications of the
jx-3p firmware, this will result in the first midi message after each pg-200 message being lost as the jx-3p has not
noticed the mode change. The first pg-200 message after a switch may also confuse the jx-3p, sometimes leading to strange
behaviour. The 'mix' mode is currently of little use.

If the jx-3p has not been modified, this mode will not work. If it HAS been modified, the switch on the jx-3p must be
in the "Midi bus" position for things to work.

Mode led:
---------
This LED shows the current transmit mode. If it is on, the MPG-200 is in midi mode and will transmit midi 
messages, if it is off if will transmit PG-200 messages. Watch this when you play and change parameters at the same
time for your own private disco.

Status led: 
-----------
Shows what's happening (to a certain extent) within the MPG-200. When switching on the jx-3p while the
MPG-200 is connected, the status led will
- flash three times - this means that it is ready to start up
- flash another three times - this means that setting up ports, midi mappings etc is finished
- flash 0, 1, 2 or 3 times to indicate what mode (pg200/safe mix/mix) it is in. 1=pg200, 2=safe mix, 3=mix. 0=default 
mode, whatever was the default when the firmware was compiled. It doesn't really matter, as the MPG-200 will always read
the position of the switch and use this.

When moving the switch, the status led will flash again to indicate the new mode.
1 flash for pg200, 2 flashes for safe mix and 3 for mix.

Modifying your