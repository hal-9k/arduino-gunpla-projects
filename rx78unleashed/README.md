# PG RX-78 unleased

WIP

## Boards

This project consists of two boards using 3 microcontrollers.

| Board | Microcontroler | Type     | Function                                    | Project |
| ----- | -------------- | -------- | ------------------------------------------- | ------- |
| TORSO | MC1            | Attiny85 | led slave: shoulders and verniers sequences | mc1.ino |
| TORSO | MC2            | Attiny85 | led slave: eyes, chest, beam saber          | mc2.ino |
| BASE  | MC3            | Nano     | SFX, sequence master.                       | mc3.ino |

## Led drivers

ULN2803APG https://www.st.com/resource/en/datasheet/uln2801a.pdf

## Sound board

DFPlayer https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299

## Libraries used

- https://github.com/DFRobot/DFRobotDFPlayerMini
