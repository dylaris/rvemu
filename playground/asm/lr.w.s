.option norvc
.option nopic

# @ident: name=lr.w, opcode=0x2f, funct3=0x2, funct5=0x02
# @expect: rd=1, rs1=2
lr.w x1, (x2)
