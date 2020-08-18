from __future__ import print_function
from optparse import OptionParser
import intelhex

def calc_crc(filename,outfilename):
	ih = intelhex.IntelHex()
	ih.fromfile(filename, "hex")
	flash_offset = 0x400000
	blob = ih.tobinarray(start=flash_offset)
	checksum=0
	img_length = len(blob)
	page_length = 512
	pages = img_length / page_length
	pages= int(pages)
	if img_length % page_length > 0:
		pages = pages +1
	for address in range(128*page_length,	len(blob)):
		checksum = checksum + blob[address]
		checksum = checksum & 0xffffffff;
	for address in range(len(blob),pages*page_length ):
		checksum = checksum + 0xff
		checksum = checksum & 0xffffffff;
		
	print("checksum ",format(checksum, '08x'))
	with open(outfilename, 'w') as out_file:
		out_file.write(':020000040040BA\n')
		out_file.write(':04000000')
		out_file.write(format(((checksum>>0) & 0xff), '02X'))
		out_file.write(format(((checksum>>8) & 0xff), '02X'))
		out_file.write(format(((checksum>>16) & 0xff), '02X'))
		out_file.write(format(((checksum>>24) & 0xff), '02X'))
		crc_line=(4+(checksum & 0xff)) & 0xff
		crc_line=(crc_line+((checksum>>8) & 0xff)) & 0xff
		crc_line=(crc_line+((checksum>>16) & 0xff)) & 0xff
		crc_line=(crc_line+((checksum>>24) & 0xff)) & 0xff
		out_file.write(format(((~crc_line)& 0xff)+1, '02X'))
		out_file.write('\n')
		file = open(filename, "r") 
		for line in file: 
			out_file.write(line) 		

		
def main():
       
	parser = OptionParser()
	parser.add_option(
		"-i",
		"--in",
		dest="in_file_name",
		default="Annapurna_4bl.hex",
		help="hex file to calculate checksum of",
		metavar="FILE")
	parser.add_option(
		"-o",
		"--out",
		dest="out_file_name",
		default="Annapurna_4bl_crc.hex",
		help="hex file to include checksum",
		metavar="FILE")
	
	(options, args) = parser.parse_args()
	calc_crc(options.in_file_name,options.out_file_name)
	
	
	
if __name__ == "__main__":
    main()
