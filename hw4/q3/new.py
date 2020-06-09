from capstone import *
import binascii as b
from pwn import *

r = remote("aup.zoolab.org", 2530)
r.recvuntil('========================================================')
r.readline()
#r.interactive()
for i in range(10):
    _in = r.readline()[4:]
    r.recvuntil('Your answer: ')
    _in = _in[:-1].decode('utf-8')

    #_in = input("enter what you want: ")
    _bin=b.a2b_hex(_in)

    md = Cs(CS_ARCH_X86, CS_MODE_64)
    res = ""
    for i in md.disasm(_bin,0):
        #print("0x%x:\t%s\t%s" %(i.address, i.mnemonic, i.op_str))
        #print("%s"%(i.op_str))
        res = res+i.mnemonic+' '+i.op_str
        if i.mnemonic:
            res = res + '\n'

    #print(res)
    out = b.b2a_hex(res.encode())
    #print(out)
    r.sendline(out)
    r.readline()
    r.readline()
r.interactive()