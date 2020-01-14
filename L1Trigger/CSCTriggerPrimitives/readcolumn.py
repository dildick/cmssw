import struct

def float_to_bin(num):
    return format(struct.unpack('!I', struct.pack('!f', num))[0], '032b')

with open('linearFits.lut.mod') as file:

    lst = []

    for line in file:
        lst.append([ float(x) for x in line.split()])

    column1 = [ x[0] for x in lst]
    column2 = [ x[1] for x in lst]
    column3 = [ x[2] for x in lst]
    column4 = [ x[3] for x in lst]

    ## print to file
    f1=open('CompCodeMap.txt', 'w+')
    index = 0
    for p,q,r,s in zip(column1, column2, column3, column4):
        if p == 100: p = 4
        if p == 90:  p = 3
        if p == 80:  p = 2
        if p == 70:  p = 1
        if p == 60:  p = 0

        ## to binary
        valuebin = float_to_bin(s)

        #        f1.write("compcode_lut[%d][%d] = (%f, %f);\n" %(p,q, r, s))
        f1.write("%d %d # iPat : iCC : %d : %d\n"%(index, int(valuebin,2), p, q))
        #compcode_lut[%d][%d] = (%f, %f);\n" %(p,q, r, s))
        index += 1
    f1.close()
