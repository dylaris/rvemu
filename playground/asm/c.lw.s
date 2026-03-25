.option rvc
.option nopic

# @ident: name=c.lw, copcode=0x0, cfunct3=0x2
# @expect: rd=8, rs1=9, imm=0
c.lw x8, 0(x9)

