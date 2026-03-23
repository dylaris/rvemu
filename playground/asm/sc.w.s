.option norvc
.option nopic

# @ident: name=sc.w, opcode=0x2f, funct3=0x2, funct5=0x03
# @expect: rd=1, rs1=2, rs2=3
sc.w x1, x3, (x2)
