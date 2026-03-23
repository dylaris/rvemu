.option norvc
.option nopic
# @ident: name=fmadd.d, opcode=0x43, funct2=0x1
# @expect: rd=0, rs1=1, rs2=2, rs3=3
fmadd.d f0, f1, f2, f3
