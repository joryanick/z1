// GoodPrototyping z1 avrdude program fuses
// (c) 2013 Jory Anick, jory@goodprototyping.com

talk.cmd
Function: Uses avrdude to communicate with z1 via ISP programmer
          Verifies that everything is OK (z1 can be programmed)

prog.cmd
Function: Sets the micrcontroller fuses
          ONLY ADJUST FUSE VALUES IF YOU KNOW WHAT YOU ARE DOING
          IMPROPER SETTINGS CAN MAKE THE z1 UNUSABLE
