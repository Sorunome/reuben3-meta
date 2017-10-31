#!/usr/bin/python3
import sys
import time

#File format:
#   header [outputArraySizeLSB,outputArraySizeMSB]
#   header [numCodewordsLSB,numCodewordsMSB]
#   Eh.. see returnLZCodes function notes for more details.
#

MAXCODELEN = 30
LITCODELEN = 126

SIZE_LEN = 8-3
BACK_LEN = 8+3

BACKREF_DIST = 1023
 
def readFile(file):
        a = []
        f = open(file,'rb')
        b = f.read(1)
        while b!=b'':
                a.append(ord(b))
                b = f.read(1)
        f.close()
        return a
 
def writeFile(file,a):
        f = open(file,'wb+')
        f.write(bytes(a))
        f.close()
 
#winsize: default -1 for SoF, else curpos-winsize bounded by SoF.
def resetSlidingWindow(curpos,winsize=None):
        if winsize is None:
            a=0
        else:
            if winsize>curpos:
                a=0
            else:
                a=curpos-winsize
        return a

#decomp v0.2
#[0zzz zzzz] = (z+1) size of following literal bytes
#[1zzz zzzz,xxxx xxxx] = (z+2) size of bytes at x bytes backref
#
def returnLZCodes(count,backrefptr,symbol):
#        global SIZE_LEN
#        global BACK_LEN
# hardcode a backlen of 11 and a size len of 5
        global NUMBER_OF_CODES_EMITTED
        NUMBER_OF_CODES_EMITTED += 1
        a = []
        if isinstance(symbol,list):
            b = symbol
        else:
            b = [symbol]
        if backrefptr == 0:
            a = [(count<<1)&0xFE] + b
          #  print("C-LTRL SIZE:"+str(count+1))
          #  time.sleep(0.5)
        else:
            # [0000 00bb bbbb bbbb] -> [bb00 0000] [bbbb bbbb]
            # (b>>2&0xC0)|(c<<1&0x3E)            [bbcc ccc1]
            #
            a = [(backrefptr>>2&0xC0)|(count<<1&0x3E)|1,backrefptr&0xFF]
          #  print("L-LZCO SIZE:"+str(count+2)+" BREF:"+str(backrefptr))
         #   time.sleep(0.5)
        return a
         

         
         
         
# do not allow "compression" of files less than 2 bytes long        
def compress(inArray):
    global MAXCODELEN
    global BACKREF_DIST
    global LITCODELEN
    global NUMBER_OF_CODES_EMITTED
    NUMBER_OF_CODES_EMITTED = 0
    lbhptr = 0    #filestart
    cptr = 1      #filestart+1
    matchfound = 0  #flag1
    emitnow =0
    foundcount = 0
    foundbackref = 0
    outArray = []
    lit_ptr = 0
    lit_size = 1
    EoA = len(inArray)  #not +1, cuz we need var at EoF to emit as literal
    while cptr < EoA:
        if inArray[lbhptr]==inArray[cptr] and (cptr!=lbhptr):
            #initial match found
            tmpcptr=cptr
            tmplbhptr=lbhptr
            if (len(inArray)-cptr)>MAXCODELEN:
                counter=MAXCODELEN
            else:
                counter=len(inArray)-cptr
            counterbase=counter
            while counter!=0:
                if inArray[tmplbhptr]!=inArray[tmpcptr]:
                    break
                tmpcptr+=1
                tmplbhptr+=1
                counter-=1
            curcount=counterbase-counter
            if (curcount>2) and (foundcount<curcount):
                matchfound=1
                if curcount==MAXCODELEN:
                    emitnow=1
                foundcount=curcount
                foundbackref=cptr-lbhptr
        lbhptr+=1
        if (cptr==lbhptr) or (emitnow==1):
            if matchfound==1:
                if lit_size>0:
                    outArray.extend(returnLZCodes(lit_size-1,0,inArray[lit_ptr:(lit_ptr+lit_size)]))
                    lit_size=0
                    #NUMBER_OF_CODES_EMITTED+=1
                #match found, emit literal and LZcodes
                outArray.extend(returnLZCodes(foundcount-2,1024-foundbackref,0))
                #NUMBER_OF_CODES_EMITTED+=1
                cptr+=foundcount
                foundcount=0
                foundbackref=0
                matchfound=0
                emitnow=0
            else:
                #increment literal or something
                if lit_size==0:
                    lit_ptr=cptr
                lit_size+=1
                cptr+=1
                if lit_size>127:
                    outArray.extend(returnLZCodes(lit_size-1,0,inArray[lit_ptr:(lit_ptr+lit_size)]))
                    lit_size=0
                    #NUMBER_OF_CODES_EMITTED+=1
            lbhptr=resetSlidingWindow(cptr,BACKREF_DIST)
    if lit_size>0:
        outArray.extend(returnLZCodes(lit_size-1,0,inArray[lit_ptr:(lit_ptr+lit_size)]))
        lit_size=0
        #NUMBER_OF_CODES_EMITTED+=1
    return outArray

#Ver 0.1: fixed len [size,backref,symbol(s)]
# If size=0, omit backref, use symbol.
# If backref=0, size+1= number of symbols (including trailing symbol)
#
def decompress(inArray):
    pass  #begin routine
    outArray = []
    cptr = 0
    while cptr < len(inArray):
        tempc = inArray[cptr]
        if (tempc&1)==0:
            count = (tempc>>1)+1
            cptr += 1
            for x in range(count):
                outArray.append(inArray[cptr])
                cptr += 1
        else:
            count = ((tempc&0x3E)>>1) + 2
            backref = 1024-(inArray[cptr+1] + ((tempc&0xC0)<<2))
       #     print("Dump count " + str(count) + ", backref " + str(backref))
            startptr = len(outArray)-backref
            for x in range(count):
                outArray.append(outArray[startptr+x])
          #  outArray.append(inArray[cptr+2])
            cptr += 2
    pass #end main loop
    return outArray

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
# Program start

NUMBER_OF_CODES_EMITTED = 0
timestart = time.clock()

if (len(sys.argv) < 2) or (sys.argv[1][0]=='-') or (sys.argv[1][0]=='/'):
    print("Usage: cmprss.ph <infile> <outfile> <options>")
    print("For help: cmprss.ph -h")
    sys.exit(1)
if "-h" in sys.argv:
    print("Usage: cmprss.ph <infile> <outfile> <options>")
    print("Options:")
    print("-t : Test mode (do not output to file)")
    print("-a : Add size header to file")
inFileName = sys.argv[1]
if (len(sys.argv) > 2) and (sys.argv[2][0]!='-'):
    outFileName = sys.argv[2]
else:
    outFileName = ""
if "-a" in sys.argv:
    addHeader = 1
else:
    addHeader = 0
if "-t" in sys.argv:
    testMode = 1
else:
    testMode = 0
     



inFileArray = readFile(inFileName)  #later read from cmdline
dataFreq = [0 for i in range(256)]  #table of num of occurances of bytes
#check  
count = 0
while count < len(inFileArray):
    dataFreq[inFileArray[count]] += 1
    count += 1

item_freq = 65535
item_code = 0
count = 0
filesum = 0

while count < 256:
    if dataFreq[count]<item_freq:
        item_freq = dataFreq[count]
        item_code = count
    filesum += dataFreq[count]
    count += 1
     
sixteensum = 0
for x in range(len(inFileArray)):
    sixteensum += inFileArray[x]
sixteensum = sixteensum & 0x00FFFF

print("Sanity check: fSize: " + str(len(inFileArray)) +
      ", summation " + str(filesum) + ", 16-bit chksum " + str(sixteensum))
#print("Item chosen: " + str(item_code) + " with " +
#      str(item_freq) + " occurrances.")

#escapecode = item_code
#escapelength = 0
#tmpvar = escapecode
#while 1:
#    escapelength += 1
#    tmpvar //= 2
#    if tmpvar == 0:
#        break
       
# print("Escape code chosen: " + str(escapecode) +
#      ", bit length: " + str(escapelength))

resultArray = compress(inFileArray)

print("Result array length :" + str(len(resultArray)))

for x in range(0,len(resultArray)):
    if resultArray[x]>255:
        print("Error: Array item " + str(x) + " outside bounds (" + str(resultArray[x]) + ")")
        print("Around: " + str(resultArray[x-4:x+4]))
#writeFile("testoutput",resultArray)

print("Decompression test start.")
decompTest = decompress(resultArray)
print("Length comparison: original: " + str(len(inFileArray)) + ", decompressed: " + str(len(decompTest)))

if str(len(inFileArray)) == str(len(decompTest)):
    print("Data integrity test")
    errcode = 0
    for x in range(len(resultArray)):
        if inFileArray[x] != decompTest[x]:
            errcode = 1
            print("Data mismatch at position " + str(x) + ": " + str(inFileArray[x]) + " vs " + str(decompTest[x]))
    if errcode == 0:
        print("Test successful. No discrepencies detected.")
    else:
        print("Error: Data mismatch found. File not written.")
        print("In: " + str(inFileArray[0:10]) + "\nOut: " + str(resultArray[0:10]) + "\nChk: " + str(decompTest[0:10]))
        sys.exit(2)
else:
    print("Error: Test length mismatch. File not written.")
    print("In: " + str(inFileArray[0:10]) + "\nOut: " + str(resultArray[0:10]) + "\nChk: " + str(decompTest[0:10]))
    sys.exit(2)
#

sizeLSB = NUMBER_OF_CODES_EMITTED%256
sizeMSB = (NUMBER_OF_CODES_EMITTED//256)%256
resultArray = [sizeLSB,sizeMSB] + resultArray

# Commented this chunk out if you're using the included z80 decompressor. Such fights. Many problems. Wow.
#sizeLSB = len(inFileArray)%256
#sizeMSB = (len(inFileArray)//256)%256
#resultArray = [sizeLSB,sizeMSB] + resultArray

print("Returned size ["+str(len(inFileArray))+"], numcodes ["+str(NUMBER_OF_CODES_EMITTED)+"]")

if addHeader == 1:
    print("Debug: resarraysample: " + str(resultArray[0:10]))

if testMode == 0:
    writeFile(outFileName,resultArray)
    print("File [" + outFileName + "] was output.")
else:
    print("Test mode running. No output.")
inSize = len(inFileArray)
outSize = len(resultArray)
print("Success! In: " + str(inSize) + ", Out: " + str(outSize) + " (" + str((((outSize/inSize)*100)//1)) + "% of original)")

timeend=time.clock()

print("Time elapsed: "+str(timeend-timestart))
