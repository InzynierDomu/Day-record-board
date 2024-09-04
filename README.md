# Day-record-board

![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/InzynierDomu/PhECMeter?style=flat-square)
<a href="https://discord.gg/KmW6mHdg">![Discord](https://img.shields.io/discord/815929748882587688?logo=discord&logoColor=green&style=flat-square)</a>
![GitHub](https://img.shields.io/github/license/InzynierDomu/PhECMeter?style=flat-square)
<a href="https://tipo.live/p/inzynierdomu">![support](https://img.shields.io/badge/support-tipo.live-yellow?style=flat-square)</a>

- [Day-record-board](#Day-record-board)
  - [Description](#description)
  - [Manual](#manual)
    - [Resetting-counter](#Resetting-counter)
    - [Adjust-clock](#Adjust-clock)
  - [Scheme](#scheme)
  - [IDE](#ide)

## Description
Scoreboard design, from P10 LED matrices. 6 Day counters, and streak record. Each counter can be reset to zero. Every day all 6 counters are incremented by 1.

<div align="center">
<h2>💖 Support</h2>

<p>If any of my projects have helped you in your work, studies, or simply made your day better, please consider supporting my work. <strong><a href="https://tipo.live/p/inzynierdomu">LINK</a></strong></p>
</div>

![photo](https://github.com/InzynierDomu/Day-record-board/blob/main/foto.jpg)

![photo](https://github.com/InzynierDomu/Day-record-board/blob/main/foto2.jpg)

## Manual

Resetting the counter and adjust the clock requires an IR pilot.
After turn on the device starts in normal mode, shows counter values ​​and streak record.

### Resetting-counter

- To enter the counter reset mode, select the back button (reverse arrow) on the pilot in the normal mode.
- Use the keys 1 to 6 to select the counter to be reset.
- The value will appear on the board where the record streak is in normal mode.
- The play button accepts and resets the counter.
- The back button cancels the action and returns to normal mode.

### Adjust-clock

- Pressing the play button in normal mode takes to the clock adjust mode.
- The last screen displays the hour, the penultimate one the minutes.
- The value that can be changed is flashing.
- To change the hours to minutes and vice versa, press the next button.
- To enter a new value of the flashing value, enter the next digits using the buttons. Entering the next digits will overwrite it. The format is hh-mm, e.g. 14.05.
- Press the play button again to accept the time change.

## Scheme
## IDE
The project is prepared for the Platform IO environment. A video on how to install such an environment can be watched on this [video](https://youtu.be/Em9NuebT2Kc).
<br><br>
Formatting is done using clang-format. The description of the tool configuration is in the [video](https://youtu.be/xxuaOG0WjIE).

