
# iHex2Bin Intel HEX file converter to binary

 /***************************************************************************
 * iHex2bin program to convert iHex ASCII file to binary.
 * 
 * parameters:
 * 
 *
 * 
 *	Usage: iHex2bin.exe {inputfilename} [parameters]
 *	parameters are:
 *
 *	inputfilename file name for input iHex ASCII file to binary convert
 *	     .. this parameter is MANDATORY
 *
 *	-h show help
 *	-d print debug information and all stats = YES default NO
 *	-i enable otput info print informations default DISABLE
 *	-w disable write  data to output file - default ENABLE
 *	-o=outfilename output file name to write binary data
 *	     .. default: "out.bin"
 *	     .. example: iHex2bin.exe -o=output.bin
 *	-p=8bit_value padding 8bit value for GAP FILLING
 *	     .. default = 0xFF - hex or decimal input value are accepted
 *	     .. example: "-p=0x00" or decimal "-p=255" 
 *	-s=32bit_value start addres for write iHex to binary data
 *	     .. default 0x00000000 - hex or decimal input value are accepted
 *	     .. example "-s=0x00000000" or decimal "-s=0"
 *  -e=32bit_value end addres for write iHex to binary data
 *	     .. default 0xFFFFFFFF - hex or decimal input value are accepted
 *	     .. example "-e=0xFFFFFFFF" or decimal "-e=4294967295"
 *
 *	     .. tip: use 
 *	      iHex2bin.exe inputfilename.hex -w -d 
 *	      Above option is only scanning mode of iHex file
 *
 ********************************************************************************/

# Many thanks to ProteusPL   https://github.com/dkm1978  for add procedure to work with console colors

![Screenshot](/PICTURES/console01.jpg)
![Screenshot](/PICTURES/console02.jpg)
![Screenshot](/PICTURES/console03.jpg)