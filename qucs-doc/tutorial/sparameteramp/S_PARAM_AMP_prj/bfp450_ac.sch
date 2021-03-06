<Qucs Schematic 0.0.18>
<Properties>
  <View=-194,-120,888,877,1,167,0>
  <Grid=10,10,1>
  <DataSet=bfp450_ac.dat>
  <DataDisplay=bfp450_ac.dpl>
  <OpenDisplay=1>
  <Script=bfp450_ac.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 550 640 0 0 0 0>
  <L L2 1 550 550 -87 -26 0 3 "9.54728nH" 1 "" 0>
  <R R3 1 550 610 15 -26 0 1 "3Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 500 520 -26 -47 0 2 "3.5Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 610 190 -55 -26 0 3 "3 V" 1>
  <GND * 1 610 160 0 0 0 2>
  <.DC DC1 1 350 80 0 39 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 770 580 0 0 0 0>
  <R R10 1 770 550 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C5 1 640 520 -26 -49 0 2 "100n" 1 "" 0 "neutral" 0>
  <L L5 1 610 250 -87 -26 0 3 "1000nH" 1 "" 0>
  <.AC AC1 1 120 80 0 39 0 0 "lin" 1 "00MHz" 1 "2000MHz" 1 "2001" 1 "yes" 0>
  <L L1 1 160 520 -26 10 0 0 "3.39628nH" 1 "" 0>
  <C C2 1 30 590 17 -26 0 1 "5.68261pF" 1 "" 0 "neutral" 0>
  <GND * 1 30 680 0 0 0 0>
  <R R1 1 30 650 15 -26 0 1 "0.2Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 100 520 -26 15 0 0 "0.20 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 80 300 0 0 0 3>
  <GND * 1 190 240 0 0 0 2>
  <C C4 1 190 270 -62 -26 0 3 "220n" 1 "" 0 "neutral" 0>
  <L L4 1 190 450 -87 -26 0 3 "56nH" 1 "" 0>
  <Vac V2 1 -140 550 18 -26 0 1 "0.0001 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <R R9 1 -140 610 15 -26 0 1 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 -140 640 0 0 0 0>
  <R R7 0 110 300 -26 15 0 0 "89 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R8 0 270 300 -26 15 0 0 "211 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R6 1 260 410 -26 15 0 0 "4500 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 430 520 -52 10 1 2 "41.1255pF" 1 "" 0 "neutral" 0>
  <R R5 1 670 300 -26 -47 0 2 "50Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C6 1 730 300 -52 10 1 2 "1.6pF" 1 "" 0 "neutral" 0>
  <GND * 1 760 300 0 0 0 1>
  <L L3 1 610 330 -87 -26 0 3 "50nH" 1 "" 0>
  <SPICE X1 1 310 550 -26 -83 1 2 "BFP450_spice.cir" 1 "_net1,_net2,_net3" 0 "yes" 0 "none" 0>
  <GND * 1 340 580 0 0 0 1>
  <GND * 1 310 610 0 0 0 0>
  <Eqn Eqn1 1 580 60 -28 15 0 0 "G=dB(Vout.v/Vin.v)" 1 "yes" 0>
  <C C3 1 -10 520 -26 -49 0 2 "100n" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <530 520 550 520 "" 0 0 0 "">
  <550 520 610 520 "" 0 0 0 "">
  <670 520 770 520 "Vout" 750 490 45 "">
  <610 280 610 300 "" 0 0 0 "">
  <300 300 310 300 "" 0 0 0 "">
  <30 520 30 560 "" 0 0 0 "">
  <30 520 70 520 "" 0 0 0 "">
  <140 300 190 300 "" 0 0 0 "">
  <190 300 240 300 "" 0 0 0 "">
  <190 480 190 520 "" 0 0 0 "">
  <190 300 190 410 "" 0 0 0 "">
  <190 410 190 420 "" 0 0 0 "">
  <190 410 230 410 "" 0 0 0 "">
  <460 520 470 520 "" 0 0 0 "">
  <310 300 610 300 "" 0 0 0 "">
  <310 300 310 410 "" 0 0 0 "">
  <290 410 310 410 "" 0 0 0 "">
  <610 300 640 300 "" 0 0 0 "">
  <190 520 280 520 "" 0 0 0 "">
  <340 520 400 520 "" 0 0 0 "">
  <610 360 610 430 "" 0 0 0 "">
  <400 430 610 430 "" 0 0 0 "">
  <400 430 400 520 "" 0 0 0 "">
  <-140 520 -40 520 "Vin" -100 480 4 "">
  <20 520 30 520 "" 0 0 0 "">
  <190 520 190 520 "UBE" 220 490 0 "">
  <400 520 400 520 "UCE" 420 460 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 620 220 12 #000000 0 "This is the 3V supply">
  <Text 210 250 12 #000000 0 "Voltage divider to divide 3V to 0.87V">
  <Text 320 380 12 #000000 0 "Alternatively drop to 0.87V\nby passing 0.45mA">
  <Text 170 -10 14 #000000 0 "AC-Small Signal Simulation of Transistor Amplifier">
  <Text -20 360 12 #ff0000 0 "Use either Voltage Divider or\nSeries Resistor">
</Paintings>
