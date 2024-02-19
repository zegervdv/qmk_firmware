# Non-Stock Customizations & Fixes

The following customizations/changes/fixes were applied to this firmware version. It includes various personal enhancements and bug fixes.

## Customizations

- `Fn + B` temporarily displays the current battery percentage on the F and number row.
The F row represents the 10s percentage and number row the ones. Example, 35% will light `F1` through `F5` and `1` through `5`.
- `Fn + \` turns on percent battery display as well as the stock side LED indicator.
- Battery indicator colour is a gradient flowing from green (full) through yellow, to red (low).
- `Fn + M + Z` to toggle the RF disconnect sleep timer between `5s` and `120s` (NuPhy default). Default is set to `5s`. This persist through restarts.
This sets how long the board tries to connect (left light blinking) before giving up.
- `Fn + M + S` toggles idle sleep timer between `30s` and default `360s`. This is temporary.
- `Fn + M + D` toggles QMK debugging. Don't turn this on when not connected to QMK toolbox.
The letter `D` will light up red when enabled.
- Side indicators will flash red for 0.5s when board enters sleep mode, as an indicator.
This is a deep sleep state. It only happens if the board is not charging, otherwise the board enters a light sleep
state with no indicators.
- Bluetooth connection indicators will be lit blue when establishing connection. This lights the corresponding
BT mode key. No indicator for RF as the sidelights are green already.
- Default startup LED brightness set to zero and side led set to lowest brightness. This is because I don't use LEDs so I don't need to toggle them off when resetting the board or flashing new firmware.


## Fixes

- Fix keyboard randomly crashing/freezing.
- Fix keyboard not sleeping properly and draining battery. This version sleeps the processor and uses almost no battery on sleep.
- Fix LED lights not powering down when not used. This increases battery life around 50-70% when LEDs aren't used.
- Fix keystrokes being lost on wake. Wake keystrokes will appear after a very short delay while board re-establishes connection.
- Enhance keyboard reports transmission logic to greatly reduce stuck/lost key strokes. It may still occasionally repeat keys but it's rare.
- Slightly enhanced sidelight refresh intervals for smoother animations.

## Author

@jincao1