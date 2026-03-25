.option rvc
.option nopic

# @ident: name=c.fldsp, copcode=0x2, cfunct3=0x1
# @expect: rd=1, rs1=2, imm=0
c.fldsp f8, 0(x9)

