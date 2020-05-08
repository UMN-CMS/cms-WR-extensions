import os
import fileinput

massgrids = open("WRtoNLtoLLJJ.txt").readlines()
for massgrid in massgrids:
  this_sample = massgrid.strip()
#  newfile = open(this_sample+".txt","wr")
  for line in fileinput.input(this_sample+".txt",inplace=True):
	print line.replace("/xrd/sto","root://cms-xrdr.sdfarm.kr:1094//xrd/sto")


