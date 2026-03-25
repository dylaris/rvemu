.option rvc
.option nopic

# @ident: name=c.ldsp, copcode=0x9, cfunct3=0x3
# @expect: rd=1, rs1=2, imm=0
c.ldsp x8, 0(x9)

