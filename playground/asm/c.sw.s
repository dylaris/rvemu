.option rvc
.option nopic

# @ident: name=c.sw, copcode=0x0, cfunct3=0x6
# @expect: rd=0, rs1=8, rs2=9, imm=0
c.sw x9, 0(x8)

