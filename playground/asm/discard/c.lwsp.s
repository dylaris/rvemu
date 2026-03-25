.option rvc
.option nopic

# @ident: name=c.lwsp, copcode=0x9, cfunct3=0x9
# @expect: rd=1, rs1=2, imm=0
c.lwsp x8, 0(x9)

