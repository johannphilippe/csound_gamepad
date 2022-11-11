<CsoundSynthesizer>
<CsOptions>
-odac
--opcode-lib=/home/johann/Documents/GitHub/build-csound_gamepad-Desktop_Qt_5_15_0_Clang_64bit-Release/bin/libcsound_gamepad.so
</CsOptions>
; ==============================================
<CsInstruments>

sr	=	48000
ksmps	=	1
;nchnls	=	2
0dbfs	=	1

// Test gamepad opcode
instr 1	

	kdev,  kid, kval gamepad
	printk2 kdev
	printk2 kid, 30
	printk2 kval, 60

endin

// Test gamepad_track_device opcode
instr 2

	kidx, kval gamepad_track_device 0
	printk2 kidx
	printk2 kval, 30

endin


// Test gamepad_track_control opcode
instr 3

	kval gamepad_track_control 0, 60433
	printk2 kval

endin

</CsInstruments>
; ==============================================
<CsScore>
;i 1 0 100
;i 2 0 100
i 3 0 100

</CsScore>
</CsoundSynthesizer>

