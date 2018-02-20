import sys
import os
import subprocess
from shutil import copyfile

files = "errors.txt"

with open(files) as f:
    lines = f.read().splitlines()

failed = set()
for line in lines:
    if "root" not in line: continue
    newLine = ""
    segments = line.split("/")
    #remove first bit
    #split up last bit
    end = segments[-1].split(".")[0]
    if "key" not in end and "probably" in line:
        failed.add(end)

failed = sorted(failed)
for fail in failed:
    print fail
