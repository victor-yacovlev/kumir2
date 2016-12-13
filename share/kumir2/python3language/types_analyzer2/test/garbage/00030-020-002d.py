def celfun(y):
    znach=y+1
    return znach

c1=int(10)
c2=int(3)
h1=float(1.2)
h2=float(0.5)
t1=str("master")
t2=str("12345")
s1=str("-")
s2=str("+")
lo=bool(0)
l2=bool(1)
cm1=[i for i in range(1,11)]
cm2=[[int(j) for j in range(1,4)] for i in range(1,4)]
cm3=[[[int (k) for k in range(1,4)] for j in range(1,4)] for i in range(1,3)]
ct=[i for i in range(1,6)]
ct2=[[int(j) for j in range(1,6)] for i in range( 1,6)]
cn=[i for i in range(-5,6)]
cn2=[[int(j) for j in range(-3,4)] for i in range(-4,5)]

n=(c1-c2)**2-40
n1=celfun(3)
n2=cm1[1]
n3=ct2[1][2]
n=(((c1-c2)**2-40+ct[1])-ct2[1][2])**2-47

for c in range(-5,6):
    cn[c]=c
c=cn[-1]
c=cn[0]
for c in range(-4,5):
    for c1 in range (-3,4):
        cn2[c][c1]=(c+4)*10+(c1+3)
c=cn2[-1][1]

c1=10
c2=3
ct[1]=1
cm1[1]=2
n1=celfun(3)
n=(((c1-c2)**2-40+ct[1])-ct2[1][2])**2-11*celfun(3)-3
cm1[1]=193
cm1[2]=192
cm1[5]=105
p=cm1[(((c1-c2)**2-40+ct[1])-ct2[1][2])**2-11*celfun(3)]
p=cm1[(((c1-c2)**2-40+ct[1])-ct2[1][2])**2-11*celfun(3)-3]*(cm1[(((c1-c2)**2-40+ct[1])-ct2[1][2])**2-11*celfun(3)-3]-183)
n=cm1[(((c1-c2)**2-40+ct[1])-ct2[1][2])**2-47]
cm1[(((c1-c2)**2-40+ct[1])-ct2[1][2])**2-2-11*celfun(3)]=193

cm1[1]=2
cm1[2]=102
cm1[3]=2
cm1[cm1[1]]=3
print(cm1[1]," ",cm1[2])
cm1[cm1[3]]=3
print(cm1[1]," ",cm1[2])
cm1[ct[1]]=3
print(cm1[1]," ",cm1[2])

cm1[2]=20
cm2[1][1]=2
c=cm1[cm2[1][1]]



