.option norvc
.option nopic

# @ident: name=lr.d, opcode=0x2f, funct3=0x3, funct5=0x02

# @expect: rd=1, rs1=2
lr.d x1, (x2)

