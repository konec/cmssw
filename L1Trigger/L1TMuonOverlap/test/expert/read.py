#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys, re, os, commands

fileName = "a.out"
file = open(fileName, "r") #Opens the file in read-mode
text = file.read() #Reads the file and assigns the value to a variable

phiMax = [0,-999,-999,-999]
phiMin = [0,999,999,999]

phiDTMax = [0,-999,-999,-999]
phiDTMin = [0,999,999,999]

for aLine in text.split('\n'):
    if aLine.find("phi:")!=-1:
        if aLine.find("sector: 1"):
            phi =  int(aLine.split("phi:")[1].split("offset:")[0])
            phiDT =  int(aLine.split("phiDT:")[1].split("phi:")[0])
            sector =  int(aLine.split("sector:")[1].split("phiDT")[0])
            if phi<phiMin[sector]:
                phiMin[sector] = phi
            if phi>phiMax[sector]:
                phiMax[sector] = phi

            if phiDT<phiDTMin[sector]:
                phiDTMin[sector] = phiDT
            if phiDT>phiDTMax[sector]:
                phiDTMax[sector] = phiDT



print "phiMax: ",phiMax
print "phiMin: ",phiMin

print "phiDTMax: ",phiDTMax
print "phiDTMin: ",phiDTMin

for iSector in range(0,4):
    print "iSector: ",iSector,phiDTMin[iSector]/4096.0+(iSector-1)*3.141/6.0, phiDTMax[iSector]/4096.0+(iSector-1)*3.141/6.0




