# Terminal Text Editor #
#### By Steven ####

## Editor ##
This simplified text editor was implemented based on the Linux Vim Text Editor. This text editor supports the following commands:

- wasd: moves the internal text editor cursor
- q: terminates the text editor window
- i [string]: inserts the text given by @string at cursor location
- save: saves the current content to the file
- u(ndo)/r(edo): undo/redo edits made by user in the current session

### Features/Constraits ###
This text editor has the following features/constraits:
- Supports a maximum of 30 lines
- Each displayed line in the input file must have a maximum width of 20 characters
- Inputting a string that is longer than 20 characters overflows to the next line
- Support for undos/redos & "quitting without saving changes" prompting

### Bugs ###
- Undo/redo logic is flawed, "unsaved changes" prompt sometimes shows/doesn't show
- Some scrolling logic issues