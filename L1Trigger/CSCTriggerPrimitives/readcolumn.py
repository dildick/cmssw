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
    index = 0

    results4 = []
    results3 = []
    results2 = []
    results1 = []
    results0 = []
    for p,q,r,s in zip(column1, column2, column3, column4):
        valuebin = int(float_to_bin(s),2)
        if p == 100:
            p = 4
            line = "%d # iPat : iCC : %d : %d\n"%(valuebin, p, q)
            results4.append(line)
        if p == 90:
            p = 3
            line = "%d # iPat : iCC : %d : %d\n"%(valuebin, p, q)
            results3.append(line)
        if p == 80:
            p = 2
            line = "%d # iPat : iCC : %d : %d\n"%(valuebin, p, q)
            results2.append(line)
        if p == 70:
            p = 1
            line = "%d # iPat : iCC : %d : %d\n"%(valuebin, p, q)
            results1.append(line)
        if p == 60:
            p = 0
            line = "%d # iPat : iCC : %d : %d\n"%(valuebin, p, q)
            results0.append(line)

        index += 1

    f1=open('CompCodeMap.txt', 'w+')
    index = 0
    for p in results0:
        f1.write("%d "%index + p)
        index += 1
    for p in results1:
        f1.write("%d "%index + p)
        index += 1
    for p in results2:
        f1.write("%d "%index + p)
        index += 1
    for p in results3:
        f1.write("%d "%index + p)
        index += 1
    for p in results4:
        f1.write("%d "%index + p)
        index += 1

    f1.close()
