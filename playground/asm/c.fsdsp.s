.option rvc
.option nopic

# @ident: name=c.fsdsp, copcode=0x2, cfunct3=0x5
# @expect: rd=0, rs1=2, rs2=1, imm=0
c.fsdsp f1, 0(x2)

