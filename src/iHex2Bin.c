/***
 *    ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓████████▓▒░░▒▓██████▓▒░ ░▒▓█▓▒░
 *    ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░
 *    ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░       ░▒▓█▓▒░
 *    ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓██████▓▒░ ░▒▓█▓▒▒▓███▓▒░░▒▓█▓▒░
 *    ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░
 *    ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░
 *     ░▒▓█████████████▓▒░ ░▒▓████████▓▒░░▒▓██████▓▒░ ░▒▓█▓▒░
 */

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
 *	-d PRINT debug information and all stats = YES default NO
 *	-i ENABLE otput info print informations default DISABLE
 *	-w DISABLE write  data to output file plus enable debug info - default ENABLE
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
 *	      iHex2bin.exe inputfilename.hex -w 
 *	      Above option is only scanning mode of iHex file
 *
 ********************************************************************************/

 ///////////////////////////////////////////////////////////////////////////////
 //
 //  This program is free software: you can redistribute it and/or modify
 //  it under the terms of the GNU General Public License as published by
 //  the Free Software Foundation, either version 3 of the License, or
 //  (at your option) any later version.
 //
 //  This program is distributed in the hope that it will be useful,
 //  but WITHOUT ANY WARRANTY; without even the implied warranty of
 //  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 //  GNU General Public License for more details.
 //
 //  You should have received a copy of the GNU General Public License
 //  along with this program. If not, see <http://www.gnu.org/licenses/>.
 //
 ///////////////////////////////////////////////////////////////////////////////
 

//==================================
#define _CRT_SECURE_NO_WARNINGS
//==================================
#include <stdio.h>
#include <inttypes.h>
#include <sys/stat.h>
//==================================

//======================================
//= Many thanks to Proteus for this
//= console regarding procedure
//======================================
#ifdef _WIN32 // WIN compiler or other?

// for control console colors in Windows

#include <windows.h>

HANDLE hStdOut;
DWORD originalMode;
WORD m_currentConsoleAttr;
CONSOLE_SCREEN_BUFFER_INFO csbi;


void deInitConsole() // return console attributes
{
	SetConsoleMode(hStdOut, originalMode);
	SetConsoleTextAttribute(hStdOut, m_currentConsoleAttr);
}
void initConsole() // init console
{
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdOut, &csbi);
	m_currentConsoleAttr = csbi.wAttributes;
	DWORD mode = 0;
	GetConsoleMode(hStdOut, &mode);
	originalMode = mode;
	mode |= 0x0004;
	SetConsoleMode(hStdOut, mode);
	atexit((void*)deInitConsole);
}

#else
#include <unistd.h>
void initConsole()
{
}
void deInitConsole()
{
}
#endif
// a colors definitions
#define Clear   "\033[2J\033[H"
#define Black   "\033[30m"
#define Red     "\033[31m"
#define Green   "\033[32m"
#define Yellow  "\033[33m"
#define Blue    "\033[34m"
#define Magenta "\033[35m"
#define Cyan    "\033[36m"
#define White   "\033[38;2;204;204;204m"
#define Reset   "\033[0m"
#define BrightBlack  "\033[38;2;118;118;118m" // RGB mode color definition
#define BrightRed     "\033[38;2;255;0;0m"
#define BrightGreen   "\033[38;2;0;255;0m"
#define BrightYellow  "\033[38;2;255;255;0m"
#define BrightBlue    "\033[38;2;82;82;255m"
#define BrightMagenta "\033[95m"
#define BrightCyan    "\033[38;2;0;255;255m"
#define BrightWhite   "\033[38;2;255;255;255m"
#define verbosecolor BrightGreen
#define debugcolor   BrightBlack
#define consolecolor BrightGreen
#define mycolor "\033[38;2;255;0;255m"
//==========================================================================
uint8_t not_write = 0;         // without write to file - scan hexfile 0 = write != 0 - not write
uint8_t byte_padd = 0xFF;      // padding byte value
uint8_t dbg_prt = 0;           // if != 0 show max information at the screen 
uint8_t checksum;              // checksum of record
uint8_t opened_input = 0;      // is opened input file ? 1 = opened | 0 = closed
uint8_t opened_output = 0;     // is opened input file ? 1 = opened | 0 = closed
uint8_t many_bt;               // actual processed record length
uint16_t record_addr;          // record address variable
uint8_t record_type;           // type of record variable
uint32_t seg0_count = 0;       // for statistics counter segments
uint32_t seg2_count = 0;       // for statistics counter segments
uint32_t seg3_count = 0;       // for statistics counter segments
uint32_t seg4_count = 0;       // for statistics counter segments
uint32_t seg5_count = 0;       // for statistics counter segments
uint32_t padded_bytes = 0;     // for stats coun padded bytes
size_t filesize;               // size in bytes of input binary file to convert
size_t rest_file;              // input filesize for calculate how many unreaded data bytes still waiting
size_t filecounter;            // input filesize to count converted data bytes to ASCII - to EOF recognize
uint32_t calc_addr = 0;        // start address value = firs byte data address
uint32_t address = 0x00000000; // 32 bit record address - defauld address value = (0)
uint32_t EIP_value = 0;        // address from (#05) record
uint16_t CS_value = 0;         // CS value from (#03) record
uint16_t IP_value = 0;         // IP value from (#03) record
uint32_t addr_len;             // length of text address parameter (const char* param);
uint32_t addr_strt;            // offset in (const char* param); to start address text
uint32_t end_add;              // just for print end address of binary data
uint32_t addr_rec;             // actuall address of records processed and saving data
int arg_count;                 // argument count copy
char** argptr;                 // arguments pointers copy
const char* progname;          // a own executed filename
const char* pFilename = "out.bin"; // output filename 
const char* input_ihex = NULL; // input filename iHex file
uint8_t input_line[1024];      // ASCII data line copied from iput_buffer[]
int actual_line = 0;           // Actual line number processing
int many_lines = 0;            // processed lines counter
uint8_t output_line[300];      // conveter from ASCII to binary data from input_line[] buffer
uint8_t input_buffer[202000];  // a buffer for input portion if binary data readed from file
uint32_t inbuf_count;          // pointer possition actually used in buffer /increasse
uint32_t inbuf_rest = 0;       // count of a data to read in the input buffer /decreasse
uint8_t output_buffer[70000];  // output buffer for portions of iHEX data records
uint32_t outbuf_count = 0;     // actual first free possition in the output buffer
uint8_t converted_byte[2];     // representation of actually converted uint8_t byte value to ASCII
uint8_t taked_byte;            // actual byte taked from input buffer
uint8_t byte_from_ASCII;       // converted byte from 2 ASCII char from line_buffer
uint32_t out_data_adr;         // address of output data
char dec_buff1[20];            // first decimal buf to convert
char dec_buff2[20];            // 2nd decimal buf to convert
uint32_t write_start = 0;      // start address for write iHex data to binary
uint32_t write_end = 0xFFFFFFFF; // end address for write iHex data to binary
uint32_t writed_bytes = 0;     // counter of write bytes to output file
uint8_t  verbose = 0;          // indicator if != 0 then print info data
uint32_t taked_value = 0;      // value from hex or decimal parameters convert from ASCII to bin value
uint32_t record00_bytes = 0;   // counter of many bytes in DATA RECORD (#00) type should be = writed_bytes
//======================================
void check_prgname(void);
int open_input_file(const char* Filename);
int read_data(void);
int read_record(void);
int scan_file(void);
int take_one_byte(void);
int unexp_EOF(void);
int file_format_error(uint8_t data);
int ASCII_to_byte(uint32_t my_offset);
uint8_t nybble_check(uint8_t data);
int  error12_print(int data, uint8_t data2);
int  error14_print(void);
void close_input(void);
void close_output(void);
void close_files(void);
int  show_help(int data);
char U_Case(char code);
void show_info03(void);
void show_info05(void);
int regard0204(void);
int open_for_write(const char* Filename);
int copy_to_outbuff(void);
int fill_out_buffer(uint32_t how_many);
char* decimal_convert(uint32_t data);
int flush_buffer(void);
const char* test_prgname(const char* Filename);
int check_params(void);
int compare_names(const char* param);
void get_value(const char* param);
uint32_t get_any_value(const char* param);
void get_decimal_value(const char* param);
void get_hex_value(const char* param);
void debug_print(void);
int param_print_error(const char* param);
//===================================================
struct stat filestatus; // structure for check file exists and filelength st_nlink and st_size
//===================================================
FILE* infile;           // input read binary file to convert
FILE* outfile;          // output write iHEX converted data
//===================================================


//========================================================================
// main function
//
// return values:
// 0 = OK
//
// 1  = to less parameters ... to make conversion
// 2  = input iHex file doesn't exists
// 3  = any other IO read ERROR at the check of input file infostructure
// 4  = Input and output filenames are this same
// 5  = open input  file failed 
// 6  = open output file failed 
// 7  = READ  binary  input file ERROR
// 8  = WRITE binary output file ERROR
// 9  = Wrong placed INPUT filename parameter...
// 10 = unexpected EOF input binary ascii file
// 11 = input ASCII text file format error
// 12 = nybble conversion ERROR - data corrupted
// 13 = checksum ASCII data error
// 14 = newline marker ERROR
// 15 = record type is out of range
// 16 = mess record order
// 17 = wrong record length on the overflow
// 18 = wrong segment (#04) address
// 19 = Segment (#00) wrong order arriving at line
// 20 = wrong segment (#02) address
// 21 = wrong address in segment (#00)
// 22 = unrecognized parameter from command line
// 23 = not typed input filename
// 24 = in first parameter a first char is wrong "-"
//========================================================================
int main(int argc, char* argv[]) {
	argptr = argv;    // pointer to argv - make copy
	arg_count = argc; //  argument count - make copy
	int result;

	initConsole();
	check_prgname();  // set the own exe program name in parameter[0] string (argv[0])
	
	result = check_params(); // test sended parameters
	if(result == 50) {
		return (show_help(0)); // dont sign exit error code for this help invoke
	}
	
	if (result != 0) { return (show_help(result)); }

	result = compare_names(pFilename); // check for "out.bin" input file name are this same as output filename
	if (result != 0) {	return show_help(result); }

	if (input_ihex == NULL) { // input file name was sended?
		fprintf(stderr, BrightYellow "\nInput file wasn't typed in a first parameter !!! ERROR #23\n");
		show_help(23);
		return 23;
	}

	result = open_input_file(input_ihex); // open input file
	if (result != 0) { return show_help(result); }

	result = open_for_write(pFilename);   // open file to write
	if (result != 0) { return show_help(result); }

	result = scan_file(); // read and scan iHex file, write data if needed
	if (result != 0) {    // if scan file error
		close_files();    // then close files and terminate
		return show_help(result);
	}

	close_files();         // close files after no error conversion
	if (dbg_prt != 0) { debug_print(); } // print debug informations

	if (not_write != 0) {  // if file wasn't write then tell about "-w" parameter
		printf(mycolor"\n%s: Scanning file only \"%s\" because \"-w\" parameter used\n" BrightGreen, progname, input_ihex);
	}

	// all OK, print the last information about conversion
	if (not_write == 0) {  // file was saved?
		if (verbose != 0) {// if we are in "not silent mode" then print about conversion file
			printf(Cyan"\n%s: Converted file \"%s\" -> \"%s\"\n" BrightGreen, progname, input_ihex, pFilename);
		}
	}
	// exit - ALL OK !
	return 0;
}
//===========================================================================================

//======================================================
// read record procedure
// read ASCII data to read_line[] array
// convert ASCII data, checking properly
// data values read and convert:
// - ":" colon value - just check
// - data length in record store to uint8_t many_bt
// - record address (16bit) store into record_addr
// - type of record into record_type variable 
// - readed checksum value to checksum variable
//
// - read and convert records data to input_line[]
// - calculate checksum value
// - check EOF
// 
// return values:
// 0  = ALL OK
// 7  = READ  binary  input file ERROR
// 10 = unexpected end of file (inputfile EOF)
// 11 = colod doesn't exists at start line ERROR
// 12 = incorrect ASCII data in nybble value
// 13 = calculated checksum compare with store ERROR
// 14 = newline marker ERROR
// 15 = type of record out of range
//======================================================
int read_record(void) {
	int result;
	actual_line++;                        // count actual line
	uint32_t i, i2, len_test;
	uint8_t tmpr;

	if (filecounter < 11) {               // data exist for record? worst case EOF record":00000001FF"
		return unexp_EOF();               // unexpected end of file
	}

	tmpr = input_buffer[inbuf_count];      // HERE IS TAKE A DATA FROM INPUT BUFFER

	if (tmpr != 0x3A) {
		
		return file_format_error(tmpr);
	}                                      // then exit with fileformatERROR code (#11)

	for (i = 0; i < 9; i++) {
		result = take_one_byte();
		if (result != 0) { return result; }
		input_line[i] = taked_byte;
	}

	result = ASCII_to_byte(1);          // convert length of data in record 
	if (result != 0) { return result; }

	tmpr = byte_from_ASCII;
	many_bt = tmpr;                     // store datalength in record to many_bt variable

	result = ASCII_to_byte(3);          // convert record address hi byte
	if (result != 0) { return result; }
	tmpr = tmpr + byte_from_ASCII;      // calculate checksum
	record_addr = byte_from_ASCII;
	record_addr = record_addr << 8;     // transfer to high 8 bits

	result = ASCII_to_byte(5);          // convert record address lo byte
	if (result != 0) { return result; }
	tmpr = tmpr + byte_from_ASCII;      // calculate checksum
	record_addr = record_addr + byte_from_ASCII;

	result = ASCII_to_byte(7);          // convert record type byte
	if (result != 0) { return result; }
	record_type = byte_from_ASCII;
	tmpr = tmpr + byte_from_ASCII;      // calculate checksum
	if (record_type == 0) {
		record00_bytes = record00_bytes + many_bt; // calc RAW data bytes in records (#00)
		seg0_count++;                   // count seg (#00) for statistics
	}                 

	if (record_type > 5) {
		fprintf(stderr, BrightYellow"\n\n%s: Input ASCII text file \"%s\" \ndata corruption in type of record (>5) at line %d !!!\n\n", progname, argptr[1], actual_line);
		close_files();
		return 15;
	}

	len_test = many_bt;                 // length of rest data in line
	len_test++;                         // plus checksum
	len_test = len_test << 1;           // multiple by 2 for ASCII representation

	if (filecounter <= len_test) {      // test of bytes to read from input file
		return unexp_EOF();
	}

	for (i = 0; i < len_test; i++) {    // take ASCII line data into record ASCII line
		result = take_one_byte();
		if (result != 0) { return result; }
		input_line[i + 9] = taked_byte;
	}

	i2 = 9;
	for (i = 0; i < many_bt; i++) {     // convert ASCII data in record line
		result = ASCII_to_byte(i2);     // and store into binary output line
		if (result != 0) { return result; }
		output_line[i] = byte_from_ASCII;
		tmpr = tmpr + byte_from_ASCII;  // continue calculate checksum
		i2 = i2 + 2;
	}
	result = ASCII_to_byte(i2);         // convert ASCII checksum
	if (result != 0) { return result; }
	checksum = byte_from_ASCII;

	// checksum = NEG checksum
	tmpr ^= 0xFF;
	tmpr++;                              // checksum = NEG checksum
	//========================
	if (tmpr != checksum) {
		fprintf(stderr, BrightYellow"\n\n%s: Input ASCII text file \"%s\" \n ASCII checksum ERROR at line %d ERROR #13 !!!\n\n", progname, argptr[1], actual_line);
		close_files();
		return 13;
	}

	if (record_type == 1) { // last record stop read, close input file, clear filecounter
		close_input();
		filecounter = 0;
		many_lines = actual_line;
		return 0;
	}

	if (filecounter == 0) { return unexp_EOF(); }
	result = take_one_byte();
	if (result != 0) { return result; }
	if (taked_byte == 0x0A) { return 0; } // end of line marker
	if (taked_byte != 0x0D) { return error14_print(); }

	if (filecounter == 0) { return unexp_EOF(); }
	result = take_one_byte();
	if (result != 0) { return result; }
	if (taked_byte == 0x0A) { return 0; } // end of line marker
	return error14_print();               // error eol marker
}
//======================================================================
//===

//===================================================
// print error data corrupted and 
// and return ERROR CODE (#14)
// new line marker error
//===================================================
int error14_print(void) {
	fprintf(stderr, BrightYellow"\n\n%s: Input ASCII text file \"%s\" \ndata corruption in newline marker at line %d !!! ERROR #14\n\n", progname, argptr[1], actual_line);
	close_files();
	return 14;
}
//===

//===================================================
// print error data corrupted and 
// and return ERROR CODE (#12)
//===================================================
int error12_print(int data, uint8_t data2) {
	fprintf(stderr, BrightYellow"\n\n%s: Input ASCII text file \"%s\" \nData corruption in nybble conversion at line %d at possition %d !!!\nERROR #12 expected data from range [0..9] = [0x30..0x39]\n or [A..F] = [0x41..0x46], arrived data 0x%02X \n\n", progname, argptr[1], actual_line, data, data2);
	close_files();
	return 12;
}
//===

//===================================================
// convert ASCII value nybble to half byte
// 
// return value if < 0x10 = data OK
// if return value > 0x0f than data are corrupted
//=====================================================
uint8_t nybble_check(uint8_t data) {
	uint8_t result;

	result = data;
	if ((result > 0x2F) && (result < 0x3A)) {  // Are data 0..9 digit ?
		result = result & 0x0F;
		return result;
	}

	if ((result > 0x40) && (result < 0x47)) {  // OR are data A..F ASCII ?
		result = result & 0x0F;
		result = result + 9;
		return result;
	}
	return 0xFF; // data are corrupted - send error > 0x0F
}
//===================================================
// convert ASCII data 2 bytes from ASCII line_buffer
// to uint8_t data byte
// function checking the properly of data are
// and invoce nybble value chec function
//
// return value = 0 -> ALL OK
//====================================================
int ASCII_to_byte(uint32_t my_offset) {
	int32_t x;
	uint8_t nybble , y;
	uint32_t i ;

	i = my_offset;                                   // pointer to ASCII high 4 bytes data
	x = (int32_t) i+1;                               // if error will occurre this value will be transfered
    y = input_line[i];                               // when error occurred take value to pass
	nybble = nybble_check(input_line[i]);            // test hi nybble ASCII are data corrected?
	if (nybble > 0x0F) { return error12_print(x,y);} // data corruption error
	converted_byte[0] = nybble << 4;                 // store first nybble binary value
	i++;                                             // pointer to next ASCII data of half byte
	y = input_line[i];
	x++;
	nybble = nybble_check(input_line[i]);            // test hi nybble ASCII are data corrected?
	if (nybble > 0x0F) { return error12_print(x,y);} // data corruption error
	converted_byte[1] = nybble;                      // store first nybble binary value
	byte_from_ASCII = converted_byte[0] + nybble;    // store final binary value converted from ASCII
	return 0;
}
//===

//================================
// input file format error
// print message and
// exit errorcode 11
//================================
int file_format_error(uint8_t data) {
	fprintf(stderr, BrightYellow"\n\n%s: Input ASCII text file \"%s\" format ERROR in line %d !!!\nERROR #11 - expected 0x3A = \":\" arrived 0x%02X \n\n", progname, argptr[1], actual_line, data);
	close_files();
	return 11;
}
//================================
// print error message about
// unexpected EOF
// and return
// error code 10
//================================
int unexp_EOF(void) {
	fprintf(stderr, BrightYellow"\n\n%s: ERROR #10 Unexpected EOF input file \"%s\" in line %d\n\n", progname, argptr[1], actual_line);
	close_files();
	return 10;
}
//===

//========================================
// take one byte from input buffer and
// actualize all needed counters...
// return value is a uint8_t [taked_byte]
// global variable
//========================================
int take_one_byte(void) {
	int result;

	if ((inbuf_rest == 0) || (inbuf_count > 199999)) { // if data in buffer are finished 
		result = read_data();                          // then get the new portion of data if exists
		if (result != 0) { return result; }            // if READ DATA ERROR exist then return
	}
	taked_byte = input_buffer[inbuf_count];            // HERE IS TAKE A DATA FROM INPUT BUFFER
	inbuf_count++;                                     // increasse pointer possition to data in buffer 
	inbuf_rest--;                                      // decreasse/count value of data bytes taked from buffer
	filecounter--;                                     // count the all bytes taked from binary file as well
	if (inbuf_rest == 0) { return read_data(); }       // if needed read next data portion
	return 0; // if don't need read data byte was teked so go back
}
//===

//==============================================================
// try to read portion of input data to input buffer in 65536
// bytes sequences or less if data to read are shortly
// if readed last data portion (EOF occurred) 
// input file will be closed
//
// return 0 = OK | != 0 = ERROR
//===============================================================
int read_data(void) {

	size_t length_data_bytes;
	size_t elements_read = 0;

	if (rest_file < 200000) {             // set the value of bytes to read 
		length_data_bytes = rest_file;    // data to read
		rest_file = 0;                    // actualize rest data value
	}
	else {
		length_data_bytes = 200000;       // data to read
		rest_file = rest_file - 200000;   // actualize rest data value
	}
	inbuf_count = 0;                      // pointer input buffer possition = start (0)

	elements_read = fread(&input_buffer, length_data_bytes, 1, infile); // read data into a buffer

	if (elements_read == 0) {             // read ERROR occurred?
		fprintf(stderr, BrightYellow"\n\n%s: READ binary INPUT iHex file \"%s\" ERROR #7 !!!\n\n", progname, argptr[1]);
		close_files();
		return 7;
	}
	inbuf_rest = (uint32_t)length_data_bytes; // set the count of a data to read in the input buffer 


	// all OK
	return 0;
}
//===

//==============================================
// function try to open input file
// input parameter: Filename
//
// output value:
// 0 = Success
// 2 = Fle doesn't exists
// 3 = Error of checking info file
// 5 = Open input file failed
//==============================================
int open_input_file(const char* Filename) {

	if (stat(Filename, &filestatus) < 0) // check file exists and file size
	{
		if (filestatus.st_nlink == 0) {       // is file exists??
			fprintf(stderr, BrightYellow"\n\nINPUT File: \"%s\" doesn't exists !!! ERROR #2\n\n", Filename);
			return 2;
		}
		else { // any other IO read ERROR
			fprintf(stderr, BrightYellow"\nAny I/O error ... of checking INPUT info file: %s !!! ERROR #3\n", Filename);
			return 3;
		}
	}
	//fopen_s(&infile, Filename, "rb");   // open input binary file to read
	infile = fopen(Filename, "rb");       // open input binary file to read
	if (infile == NULL) {                 // occurred error in open file?
		fprintf(stderr, BrightYellow"\n%s: open INPUT file \"%s\" failed !!! ERROR #5\n\n", progname, argptr[1]);
		return 5;
	}

	filesize = filestatus.st_size;           // get the file size
	rest_file = filesize;                    // for counting the rest of unreaded data from input file
	filecounter = filesize;                  // for counting all processed input data to ASCII and EOF check

	opened_input = 1;

	return read_data();
}
//===========================================
//===

//===========================================
// set pointer to program name
// to [progname] variable and if in string
// exists full path, pointer is set
// to filename withouth path 
// pointer called [progname] is used to
// print any information like usage
//===========================================
void check_prgname(void) {
	int offset = 0;
	while ((uint8_t)argptr[0][offset] != (uint8_t)0) { // get the full length
		offset++;
	}
	while (offset--) { // if exists full path, move pointer to filename
		if ((argptr[0][offset] == '/') || (argptr[0][offset] == '\\')) {
			break;
		}
	}
	offset++;
	progname = (&(argptr[0][offset])); // progname is a pointer to executed file name
}
//===
//=============================================
// return pointer to program name from
// called string in parameter [Fulename]
// if in string exists full path, the pointer
// is set to filename withouth path 
//=============================================
const char* test_prgname(const char* Filename) {
	int offset = 0;
	const char* result;
	while ((uint8_t)Filename[offset] != (uint8_t)0) { // get the full length
		offset++;
	}
	while (offset--) { // if exists full path, move pointer to filename
		if ((Filename[offset] == '/') || (Filename[offset] == '\\')) {
			break;
		}
	}
	offset++;
	result = (&(Filename[offset])); // progname is a pointer to file name
	return result;
}
//===

//=====================================================
// try to check address value parameter and convert
// it to address variable value trying to check and
// get value as hexadecimal or decimal fro ASCII
//=====================================================
void get_value(const char* param) {

	uint32_t chk_dec = 0; // input decimal value indicator
	uint32_t temp1, temp2;

	temp1 = 0;
	while ((uint8_t)param[temp1] != (uint8_t)0) { // check length of address parameter
		temp1++;
	}
	temp2 = 0;
	while ((uint8_t)param[temp2] == (uint8_t)0x20) { // delete leading spaces
		temp2++;
	}
	addr_len = temp1 - temp2;                            // length of address parameter
	addr_strt = temp2;                                   // offset in address string

	if (addr_len > 2) {                                  // check "0x" string for hex value
		if ((uint8_t)param[addr_strt] == 0x30) {
			if ((U_Case(param[addr_strt + 1])) == 0x58) {
				chk_dec++;
				get_hex_value(param);
			}
		}
	}
	if (chk_dec == 0) { get_decimal_value(param); }

}
//===


//=====================================================
// try to get data as hex input string of value
// and if successfull store converted string to
// uint32_t [taked_value] variable
//=====================================================
void get_hex_value(const char* param) {

	uint32_t start_string, end_string, i, new_add;
	uint8_t data;

	start_string = addr_strt + 2;                              // offset in address string (usually 0) + 2 ommited "0x"
	end_string = start_string + addr_len - 2;                  // end of hex address text string
	new_add = 0;                                               // initial value of new address

	if (addr_len > 10) { end_string = start_string + 10 - 2; } // if string is to long then trim it
	for (i = start_string; i < end_string; i++) {              // take the data to convert from string
		data = (uint8_t)U_Case(param[i]);                      // upper case
		if ((data > 0x2F) && (data < 0x3A)) {                  // is a digit value?
			data = data & 0x0f;
		}
		else {
			data = data & 0x0f;                                // it's not digit value, change to A..F (10..15) value
			data = data + 9;
		}
		new_add = new_add << 4;                                // next nyblle insight comming
		new_add = new_add + data;
	}
	taked_value = new_add;                                     // store new address value
}
//===

//=====================================================
// try to get the decimal value from parameter
// and if successfull store converted string to
// uint32_t [taked_value] variable
//=====================================================
void get_decimal_value(const char* param) {

	uint32_t start_string, end_string, i, new_add;
	uint8_t data;

	start_string = addr_strt;                 // offset in address string (usually 0)
	end_string = start_string + addr_len;     // length string of address param
	new_add = 0;                              // new address initial value

	for (i = start_string; i < end_string; i++) {
		data = (uint8_t)U_Case(param[i]);
		if (data == 0) { break; }

		if ((data > 0x2F) && (data < 0x3A)) { // ignore no digit value - isn't digit?
			data = data & 0x0f;               // take next digit
			new_add = new_add * 10;           // and calculate address
			new_add = new_add + data;
		}
	}
	taked_value = new_add;                    // store new address value
}
//===



//=============================
// close input and output file
// if needed
//=============================
void close_files(void) {
	close_input();
	close_output();
}
//===
//=============================
// close input file if needed
//=============================
void close_input(void) {
	if (opened_input == 1) {
		fclose(infile);
	}
	opened_input = 0;
}
//===
//=============================
// close output file if needed
//=============================
void close_output(void) {
	if (opened_output == 1) {
		fclose(outfile);
	}
	opened_output = 0;
}
//=============================



//============================================
// try to convert passed byte to UPPER CASE
//
// return uin8_t converted byte
//============================================
char U_Case(char code) {

	uint8_t data;

	data = (uint8_t)code;
	if (data > 0x60) {          // if data in [a..z] range
		if (data < 0x7B) {
			data = data - 0x20; // then convert to [A..Z]
		}
	}
	return (char)data;
}
//==

//==============================================================
// try to write iHEX data from output buffer
// to output iHEX file (argv[2])
//
// add return status value
// 0 = OK | != 0 = WRITE ERROR
// if WRITE ERRORS occurred then close opened files
//===============================================================
int flush_buffer(void) {
	if (outbuf_count == 0) { return 0; }
	uint8_t save_data = not_write;        // 0 = doing write ; != 0 = don't write

	uint32_t start_buf = 0;
	uint32_t end_buf = outbuf_count;

	uint32_t write_find;
	write_find = out_data_adr + outbuf_count - 1;


	if (write_start >= write_end) { save_data = 1; } // don't write

	// set start address to write
	if (write_start > write_find) { save_data = 1; }  // don't write
	if (write_start >= out_data_adr) {
		start_buf = write_start - out_data_adr;
	}

	// set end address to write
	if (write_end < out_data_adr) { save_data = 1; } // end was reached before
	if (write_end <= write_find) {
		end_buf = end_buf - (write_find - write_end) - 1;
	}

	size_t bytes_to_write = end_buf - start_buf; // bytes to write used bytes in iHEX buffer
	if (bytes_to_write == 0) { save_data = 1; }
	if (save_data == 0) { // if need doing write do it
		size_t elements_written = fwrite(&output_buffer[start_buf], bytes_to_write, 1, outfile); // write data
		if (elements_written == 0) {      // exists WRITE ERROR?
			fprintf(stderr, BrightYellow"\n\n%s: WRITE binary OUTPUT file \"%s\" ERROR #8 !!!\n\n", progname, pFilename);
			close_files();                // close opened files
			return 8;                     // ERRROR
		}
		writed_bytes = writed_bytes + bytes_to_write; // calculate saved bytes
	}
	// actualize output buffer data address
	out_data_adr = out_data_adr + outbuf_count;
	outbuf_count = 0;                 // rewind used buffer pointer possition
	return 0;                         // all OK
}
//===
// ============================
// show data from (#03) record
//=============================
void show_info03(void) {
	seg3_count++;         // count seg 3 for statistic
	CS_value = output_line[0];
	CS_value = CS_value << 8;
	CS_value = CS_value + output_line[1];

	IP_value = output_line[2];
	IP_value = IP_value << 8;
	IP_value = IP_value + output_line[3];

	if (verbose > 0) {
		printf(verbosecolor"\n\nRecord type (#03) with 16bit CS value = 0x%04X\n", CS_value);
		printf("Record type (#03) with 16bit IP value = 0x%04X\n\n", IP_value);
	}
}
//===

// ============================
// show data from (#05) record
//=============================
void show_info05(void) {
	seg5_count++;         // count seg 5 for statistic
	EIP_value = output_line[0];
	EIP_value = EIP_value << 8;
	EIP_value = EIP_value + output_line[1];
	EIP_value = EIP_value << 8;
	EIP_value = EIP_value + output_line[2];
	EIP_value = EIP_value << 8;
	EIP_value = EIP_value + output_line[3];
	if (verbose > 0) {
		printf(verbosecolor"\nRecord type (#05) with EIP value = 0x%08X\n\n", EIP_value);
	}
}
//===
//================================================
// regarding segment (#02) and (#04)
// 
// 
//================================================
int regard0204(void) {
	int result;
	uint32_t tmpr2, tmpr4;
	tmpr2 = address >> 16;
	//******************************************** */
	// check new data output addres fro record (#04)
	if (record_type == 4) {
		seg4_count++;         // count seg 4 for statistic
		if (dbg_prt != 0) { printf(debugcolor"Record 04 type at address: 0x%08X at line: %d\n", address, actual_line); }

		tmpr4 = output_line[0];
		tmpr4 = tmpr4 << 8;
		tmpr4 = tmpr4 + output_line[1];

		if (tmpr4 < tmpr2) {  // wrong continue segment address
			fprintf(stderr, BrightYellow"\n\nERROR #18 - Segment (#04) wrong hi address at line %d \n\n", actual_line);
			close_files();
			return 18;
		}
		if (tmpr4 > tmpr2) {
			tmpr4 = tmpr4 << 16;     // new address
			tmpr2 = tmpr4 - address; // tmpr2 = padding bytes length
			if (verbose > 0) {
				printf(verbosecolor"\nBetween segment (#04) at line: %d", actual_line);
				printf("\nPadding  0x%08X bytes from address: 0x%08X to 0x%08x\n\n", tmpr2, address, tmpr4);
			}
			result = fill_out_buffer(tmpr2);
			if (result != 0) { return result; }
			padded_bytes = padded_bytes + tmpr2; // do stats of padd bytes
			address = tmpr4;         // store new address value
			return   read_record();  // take new data record
		}
		else { return read_record(); } // take new data record

	}
	else { // now we gotta record 02 - no other possibility
		tmpr4 = output_line[0];
		tmpr4 = tmpr4 >> 4; // like and 0xF0
		tmpr4 = tmpr4 << 16;
		tmpr2 = address & 0x000FFFFF;
		seg2_count++;         // count seg 2 for statistic
		if (dbg_prt != 0) { printf(debugcolor"Record 02 type at address: 0x%08X at line: %d\n", address, actual_line); }

		if (tmpr4 < tmpr2) {  // wrong continue segment address
			fprintf(stderr, BrightYellow"\n\nERROR #20 Segment (#02) wrong hi address at line %d \n\n", actual_line);
			close_files();
			return 20;
		}
		if (tmpr4 > tmpr2) {
			tmpr4 = tmpr4 << 16;       // new address
			tmpr2 = tmpr4 - address;   // tmpr2 = padding bytes length
			if (verbose > 0) {
				printf(verbosecolor"\nBetween segment (#02) at line: %d", actual_line);
				printf("\nPadding  0x%08X bytes from address: 0x%08X to 0x%08x \n\n", tmpr2, address, tmpr4);
			}
			result = fill_out_buffer(tmpr2);
			if (result != 0) { return result; }
			padded_bytes = padded_bytes + tmpr2; // do stats of padd bytes
			address = tmpr4;           // store new address value
			return read_record();      // take new data record
		}
		else { return read_record(); } // take new data record
	}
}
//===

//================================================
// convert sended uint32_t value in parameter 
// to ASCII digit string
// 
// return value is a pointer to converted string
//================================================
char* decimal_convert(uint32_t data) {
	uint32_t i, i2, value, tmpr;
	uint8_t rest;
	value = data;

	for (i = 0; i < 20; i++) {
		dec_buff1[i] = (char)0;
		dec_buff2[i] = (char)0;
	}

	i = 0;
	while (value > 9) {
		tmpr = value / 10;
		tmpr = tmpr * 10;
		rest = value - tmpr;
		rest = rest + 48;
		value = value / 10;
		dec_buff1[i] = (char)rest;
		i++;
	}
	rest = value;
	rest = rest + 48;
	dec_buff1[i] = (char)rest;

	rest = i;
	rest++;
	i2 = 0;
	for (i2 = 0; i2 < rest; i2++) {
		dec_buff2[i2] = dec_buff1[i];
		i--;
	}
	return  &dec_buff2[0];
}
//===

//================================
// fill out buffer by PADD value
// and save to file if needed
//
// return 0 = OK or !=0 error
//================================
int fill_out_buffer(uint32_t how_many) {
	uint32_t i;
	int result;
	for (i = 0; i < how_many; i++) {
		output_buffer[outbuf_count] = byte_padd;
		outbuf_count++;

		if (outbuf_count > 0xFFFF) {
			result = flush_buffer();
			if (result != 0) { return result; }
		}
	}
	return 0;
}
//===

//=====================================
// copy converted from ASCII 
// to binary data from input_line[]
// to output_buffer 
// it's called for data from 
// (#00) record how many data 
// to copy is determinate
// in [many_bt] uint8_t variable
//=====================================

int copy_to_outbuff(void) {
	uint32_t i;
	for (i = 0; i < many_bt; i++) {
		output_buffer[outbuf_count] = output_line[i];
		outbuf_count++;
		if (outbuf_count > 0xFFFF) { return flush_buffer(); }
	}
	return 0;
}
//===

//============================
// open output file in write
// write mode (if needed)
//============================
int open_for_write(const char* Filename) {
	if (not_write > 0) { return 0; }      // if "-w" parameter activated then exit

	outfile = fopen(Filename, "wb");      // open input binary file to read
	if (outfile == NULL) {                // I/O ERROR occurred?
		fprintf(stderr, BrightYellow"\n%s: open output file \"%s\" failed !!! ERROR #6\n\n", progname, argptr[2]);
		close_files();
		return 6;
	}
	opened_output = 1;    // mark the output file is opened
	return 0;
}
//===

//==============================
// show short help information
//==============================
int show_help(int data)
{
	printf(
		"\n"BrightCyan
		"Usage:\n"BrightGreen
		"    %s {inputfilename} [parameters]\n\n"White
		"parameters are:\n\n"

		"inputfilename file name for input iHex ASCII file to binary convert \n"
		"     .. this parameter is MANDATORY\n\n"

		"-h for show this help\n"
		"-d PRINT debug information and all stats = YES default NO\n"
		"-i ENABLE otput info print informations default DISABLE\n"
		"-w DISABLE write  data to output file plus enable debug info - default ENABLE\n\n"
		"-o=outfilename output file name to write binary data\n"
		"     .. default: \"out.bin\"\n"
		"     .. example: %s -o=output.bin\n"
		"-p=8bit_value padding 8bit value for GAP FILLING\n"
		"     .. default = 0xFF - hex or decimal input value are accepted\n"
		"     .. example: \"-p=0x00\" or decimal \"-p=255\" \n"
		"-s=32bit_value start addres for write iHex to binary data\n"
		"     .. default 0x00000000 - hex or decimal input value are accepted\n"
		"     .. example \"-s=0x00000000\" or decimal \"-s=0\"\n"
		"-e=32bit_value end addres for write iHex to binary data\n"
		"     .. default 0xFFFFFFFF - hex or decimal input value are accepted\n"
		"     .. example \"-e=0xFFFFFFFF\" or decimal \"-e=4294967295\"\n\n"BrightCyan

		"     .. tip: use \n"BrightGreen
		"      %s inputfilename.hex -w \n"White
		"      Above option is only scanning mode of iHex file\n"consolecolor
		"\n",
		progname, progname, progname
	);
	if (dbg_prt != 0) { debug_print(); }
	return data;
}
//=============================

//======================================================================
// make a scan input ihex file
// main task which work in loop
// reading ASCII text data from iHex file
// convert to binary and save if needed
// 
// return:
// 0 = OK and !=0 ERROR
//======================================================================
int scan_file(void) {
	int result = 0;
	uint32_t tmpr1, tmpr2, tmpr3, tmpr4;


	//********************************************** */
	// start read data and setup pointers
	result = read_record(); // read first record is segment record or data?
	if (result != 0) { return result; }
	if (record_type == 0) { printf("\n"); } // add newline character for first address print
	if (record_type == 4) {
		address = output_line[0];
		address = address << 8;
		address = address + output_line[1];
		address = address << 16;
		if (verbose > 0) {
			printf(verbosecolor"\nFirst record type (#04) with addres value = 0x%08X\n", address);
		}
		seg4_count++;
		result = read_record(); // read first record is segment record or data?
		if (result != 0) { return result; }
	}
	if (record_type == 2) {
		address = output_line[0];
		address = address >> 4;   // high 4 bits from 12 address of (#02) record
		address = address << 16;  // convert true address from 02 record
		if (verbose > 0) {
			printf(verbosecolor"\nFirst record type ($02) with addres value = 0x%08X\n", address);
		}
		seg2_count++;
		result = read_record(); // read first record is segment record or data?
		if (result != 0) { return result; }
	}

	if (record_type == 1) {
		close_files();
		fprintf(stderr, BrightYellow"\n\nNo data exist EOF record (#01) recognized at line: 1\n");
		return 0;
	}

	if ((record_type == 3) || (record_type == 5)) {
		if (record_type == 3) { show_info03(); }
		else { show_info05(); }
		result = read_record(); // read first record is segment record or data?
		if (result != 0) { return result; }
	}

	if (record_type != 0) {
		fprintf(stderr, BrightYellow"\n\nERROR #16 - Record (#00) expected, but it doesn't at line %d \n", actual_line);
		return 16; // mess record order
	}
	else {  // record type = (#00)
		address = address + record_addr;  // actualize  start address
		calc_addr = address;              // store first byte address
	}
	out_data_adr = address;               // store output data address writed in output buffer;
	if (verbose > 0) {
		printf(verbosecolor"Start data addres value = 0x%08X \n\n", address); // prin info about start address
	}
	//************************************************** */

	// now setup addres and etcetera
	while (1)
	{
		tmpr1 = record_addr;         // test of overflow 16bit data
		tmpr1 = tmpr1 + many_bt;     // now test overflow

		if (record_type == 1) {      // EOF data record?
			result = flush_buffer(); // yes! store unsaved data if exists
			close_files();
			if (result != 0) { return result; }
			//************
			// here can any flush buffer or statistics
			//**********************
			if (verbose > 0) {
				printf(mycolor"\n--------------------\n");
				printf("- STATISTICS INFO: -\n");
				printf("--------------------\n"White);

				printf(White"Start iHex  data address = "BrightGreen"0x%08X\n", calc_addr);
				printf(White"End   iHex  data address = "BrightGreen"0x%08X\n", address);
				printf(White"(#00) record  RAW  bytes = "BrightGreen"0x%08X"White" decimal = "BrightGreen"%s\n", record00_bytes, decimal_convert(record00_bytes));
				printf(White"Padded  of   data  bytes = "BrightGreen"0x%08X"White" decimal = "BrightGreen"%s\n", padded_bytes, decimal_convert(padded_bytes));
				uint32_t len_pd_ihex = record00_bytes + padded_bytes;
				printf(White"Padd + rec. (#00)  bytes = "BrightGreen"0x%08X"White" decimal = "BrightGreen"%s\n", len_pd_ihex, decimal_convert(len_pd_ihex));

				uint32_t len_ihex = address - calc_addr;
				printf(White"iHex StartADDR - EndADDR = "BrightGreen"0x%08X"White" decimal = "BrightGreen"%s\n", len_ihex, decimal_convert(len_ihex));
				printf(White"ASCII iHex size of  file = "BrightGreen"0x%08X"White" decimal = "BrightGreen"%s\n", (uint32_t)filesize, decimal_convert((uint32_t)filesize));

				printf(White"Total   ASCII  lines  of  iHex  data: "BrightGreen"%d\n", actual_line);
				printf(White"Records (#00) type ASCII lines count: "BrightGreen"%d\n", seg0_count);
				printf(White"Records (#02) type ASCII lines count: "BrightGreen"%d\n", seg2_count);
				printf(White"Records (#03) type ASCII lines count: "BrightGreen"%d\n", seg3_count);
				printf(White"Records (#04) type ASCII lines count: "BrightGreen"%d\n", seg4_count);
				printf(White"Records (#05) type ASCII lines count: "BrightGreen"%d\n", seg5_count);
				printf(White"----------------------------------------------------------\nWrited binary bytes = "BrightGreen"0x%08X "White"decimal = "BrightGreen"%s"White"\n----------------------------------------------------------\n", writed_bytes, decimal_convert(writed_bytes));
				printf(White"Write start address = "BrightGreen"0x%08X\n", write_start);
				printf(White"Write  end  address = "BrightGreen"0x%08X\n" White, write_end);
				printf(mycolor"--------------------------");
				printf("\n- END OF STATISTICS INFO -");
				printf("\n--------------------------\n");
			}

			return 0;
		}
		//****************************
		//* overflow test
		//****************************** */ */
		if (tmpr1 > 0xFFFF) {         // now we waitting for link data
			if (tmpr1 != 0x10000) {   // or EOF, or checing structure overflow error
				fprintf(stderr, BrightYellow"\n\nERROR #17 - Record (#00) wrong overflow data length %d \n\n", actual_line);
				close_files();
				return 17;
			}
			if (record_type == 0) {
				copy_to_outbuff();
				// here copy data to output buffer
				address = address + many_bt; // set the addres for next arriving bytes
			}
			result = read_record(); // take next record expexted (#04) or (#02)
			if (result != 0) { return result; }

			tmpr2 = address >> 16;   // prepare speculative new record (#04) address field
			tmpr3 = tmpr2 >> 12;     // prepare speculative new record (#02) address field
			if (record_type == 1) { return 0; } // end of file (EOF)
			if (record_type == 0) {
				fprintf(stderr, BrightYellow"\n\nERROR #19 - Segment (#00) wrong order arriving at line %d \n\n", actual_line);
				return 19;
			}

			if ((record_type == 3) || (record_type == 5)) {
				if (record_type == 3) { show_info03(); }
				else { show_info05(); }
				result = read_record(); // read next record is segment record or data?
				if (result != 0) { return result; }
			}
			if (record_type == 1) {
				flush_buffer();  // store the last data from buffer to output file
				close_files();
				return 0;
			} // end of file (EOF)

			result = regard0204();
			if (result != 0) { return result; }

		} // if(tmpr1 > 0xFFFF) { 

		if (record_type == 0) { // check if padding bytes need
			tmpr2 = address & 0xffff;


			if (record_addr > tmpr2) {
				tmpr3 = record_addr - tmpr2; // many padd
				tmpr4 = address + tmpr3;     // padd bytes between (#00) records
				if (verbose > 0) {
					printf(Cyan"\nBetween (#00) records at line: %d", actual_line);
					printf("\nPadding  0x%08X bytes from address: 0x%08X to 0x%08x \n\n", tmpr3, address, tmpr4);
				}
				result = fill_out_buffer(tmpr3);
				if (result != 0) { return result; }
				padded_bytes = padded_bytes + tmpr3; // do stats of padd bytes
				address = tmpr4;

			}
			else {
				if (record_addr < tmpr2) {
					printf("tmpr2 = 0x%08X ; record_addr = 0x%08X", tmpr2, record_addr);
					fprintf(stderr, BrightYellow"\n\nERROR #21 - Record (#00) wrong address at line %d \n\n", actual_line);
					close_files();
					return 21;
				}
				else {
					if (record_addr == tmpr2) { // copy data from record (#00) to output buffer
						// here copy data to output buffer
						copy_to_outbuff();
						address = address + many_bt; // set the addres for next arriving bytes
						result = read_record();
						if (result != 0) { return result; }
					}
				}
			}


		}
		else {
			if ((record_type == 3) || (record_type == 5)) {
				if (record_type == 3) { show_info03(); }
				else { show_info05(); }
				result = read_record(); // read next record is segment record or data?
				if (result != 0) { return result; }
			}
			else { // take regard for records 02 and 04
				result = regard0204();
				if (result != 0) { return result; }
			}
		}

	}
	return result;
}
//==============================================
// compare iHex filename with output filename
// and if are thes same output filename is
// setting to default value "out.bin"
//===============================================
int compare_names(const char* param) {
	int len1;      // length input filename
	int len2;      // length output filename
	int i;         // iterator

	uint8_t data1; // temp byte value
	uint8_t data2; // temp byte value
	const char* pTest;   // finally pointer to output filename parameter
	const char* pIhex;   // pointer to input filename parameter

	pTest = test_prgname(param);
	pIhex = test_prgname(argptr[1]);

	len1 = 0;
	while ((uint8_t)pIhex[len1] != (uint8_t)0) { // take the length of input filename
		len1++;
	}
	len2 = 0;
	while ((uint8_t)pTest[len2] != (uint8_t)0) { // take the length of output filename
		len2++;
	}

	if (len1 != len2) {
		pFilename = pTest; // filenames are not this same
		return 0;            // exit from procedure
	}

	if (len1 == len2) {
		for (i = 0; i < len1; i++) {
			data1 = (uint8_t)U_Case(pIhex[i]);
			data2 = (uint8_t)U_Case(pTest[i]);

			if (data1 != data2) {
				pFilename = pTest;// filenames are not this same
				return 0;           // exit from procedure
			}
		}
	}
	fprintf(stderr, BrightYellow"\nERROR #4 Input iHex filename " BrightRed "%s" BrightYellow " and output " BrightRed "%s " BrightYellow"are this same !!!\n" , pIhex , pTest);
	return 4;
}
//===
//==========================================
// get any hex or decimal value from string
// and store it in taked_value var and 
// returning it in return value
//==========================================
uint32_t get_any_value(const char* param) {
	if (param[2] == '=') {
		if ((uint8_t)param[3] != 0) {
			get_value(&param[3]);
		}
	}
	return taked_value;
}
//===


//====================================================
// check parameters as:
// - to less parameters?
// - input  file exists?
// - input  file info read error
// - input  file name length
// - output file name length
// - comparison booth filename strings of inputfile and outputfile
//
// return 0 = OK | != 0 = ERROR
//=====================================================
int check_params(void) {

	int i, result;

	if (arg_count < 2) { // to less parameters !!!
		fprintf(stderr, BrightYellow"\n\n To less parameters ! ERROR #1\n\n");
		return 1;
	}

	for (i = 1; i < arg_count; i++) {
		if (argptr[i][0] == '-') {

			if (U_Case(argptr[i][1]) == 'H') { // help option?
				return 50; // yes go out for help print
			}
			else {
				if (i == 1) {
					fprintf(stderr, BrightYellow"\nThe first MANDATORY parameter as input iHex filename wasn't typed !!! ERROR #24\nWrong first parameter char \"-\"\n");
					return 24;
				}
				if (argptr[i][1] == 'd') {
					if(argptr[i][2] != 0) { return param_print_error(argptr[i]); }
					dbg_prt = 1; // enable debug info
				}
				if (argptr[i][1] == 'i') {
					if (argptr[i][2] != 0) { return param_print_error(argptr[i]); }
					verbose = 1;
				}
				if (argptr[i][1] == 'w') {
					if (argptr[i][2] != 0) { return param_print_error(argptr[i]); }
					not_write = 1;
					verbose = 1;
					dbg_prt = 1;
				}
				if (argptr[i][1] == 'o') {
					if (argptr[i][2] == '=') {
						if ((uint8_t)argptr[i][3] != 0) {
							result  = compare_names(&argptr[i][3]); // test for change output filename
							if(result != 0) { return result; }
						}
					}
				} // end of outputfilename check
				if (argptr[i][1] == 'p') { // padding byte value
					if (argptr[i][2] != '=') { return param_print_error(argptr[i]); }
					if ((uint8_t)argptr[i][3] == 0) { return param_print_error(argptr[i]); }
					taked_value = byte_padd;
					get_any_value(argptr[i]);
					byte_padd = taked_value & 0xFF;
				}
				if (argptr[i][1] == 's') { // padding byte value
					if (argptr[i][2] != '=') { return param_print_error(argptr[i]); }
					if ((uint8_t)argptr[i][3] == 0) { return param_print_error(argptr[i]); }
					taked_value = write_start;
					write_start = get_any_value(argptr[i]);
				}
				if (argptr[i][1] == 'e') { // padding byte value
					if (argptr[i][2] != '=') { return param_print_error(argptr[i]); }
					if ((uint8_t)argptr[i][3] == 0) { return param_print_error(argptr[i]); }
					taked_value = write_end;
					write_end = get_any_value(argptr[i]);
				}
			}
		}
		else
		{ // parameter without "-" marker it's input iHex name
			if (i == 1) {
				input_ihex = argptr[i];
			}
			else {
				if (i == 1) {
					fprintf(stderr, BrightYellow"\nThe first MANDATORY parameter as input iHex filename wasn't typed !!! ERROR #9\nWrong placed INPUT filename parameter...\n");
					return 9;
				}
				else {
					return param_print_error(argptr[i]);
				}
			}
		}

	}
	return 0;
}
//===

//================================
// if "-d" parameter was sended
// the this debug inf is printed
//================================
void debug_print(void) {
	printf(BrightCyan);
	printf("\n--------------");
	printf("\n- DEBUG INFO -");
	printf("\n--------------");
	printf(White);
	// Display each command-line argument.
	printf("\nCommand-line arguments:\n");
	printf(BrightCyan);
	printf("--------------\n");
	printf(White);
	for (int count = 0; count < arg_count; count++)
	{
		printf("  argv[%d]   "BrightGreen"%s\n" White, count, argptr[count]);
	}
	printf(BrightCyan);
	printf("--------------\n\n");
	printf(White);
	printf("Program filename: "BrightGreen"\"%s\"\n\n", progname);
	printf(White"Input  iHex file: "BrightGreen"\"%s\"\n", input_ihex);
	printf(White"Output bin  file: "BrightGreen"\"%s\"\n", pFilename);
	printf(White"-i option  info print is "BrightGreen"ENABLED\n"White);
	printf("-d option debug print is "BrightGreen"ENABLED\n"White);
	printf("-w option write conversion into binary file is "BrightGreen);
	if (not_write == 0) { printf("ENABLED\n"); }
	else { printf("DISABLED\n"); }
	printf(White"PADD byte value = "BrightGreen"0x%02X\n", byte_padd);
	printf(White"Write start address = "BrightGreen"0x%08X\n", write_start);
	printf(White"Write  end  address = "BrightGreen"0x%08X\n"BrightCyan, write_end);
	printf("---------------------");
	printf("\n- END OF DEBUG INFO -");
	printf("\n---------------------\n"BrightGreen);
}
//===

//=========================
// Print info ERROR about
// unrecognized parameter
//=========================
int param_print_error(const char* param) {
	fprintf(stderr, BrightYellow"\n\nERROR #22 Wrong parameter string - no match: " BrightRed "\"%s\"\n\n"BrightWhite"PLEASSE WAIT 3s ...\n", param);
#ifdef _WIN32
	Sleep(3000);  // for Windows
#else
	sleep(3);     // for Linux
#endif
	return 22;
}
//===

//================================================ 