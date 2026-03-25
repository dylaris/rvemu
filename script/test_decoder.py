import ctypes
import re
import os
import argparse

lib = ctypes.CDLL("./librvemu.so")

class Inst(ctypes.Structure):
    _fields_ = [
        ("rd",  ctypes.c_int8),
        ("rs1", ctypes.c_int8),
        ("rs2", ctypes.c_int8),
        ("rs3", ctypes.c_int8),
        ("imm", ctypes.c_int32),
        ("kind", ctypes.c_int),
        ("rvc", ctypes.c_bool),
        ("brk", ctypes.c_bool),
    ]

class InstDef(ctypes.Structure):
    _fields_ = [
        ("name", ctypes.c_char_p),
        ("kind", ctypes.c_int),

        ("opcode", ctypes.c_uint16),
        ("funct2", ctypes.c_uint16),
        ("funct3", ctypes.c_uint16),
        ("funct5high", ctypes.c_uint16),
        ("funct5low", ctypes.c_uint16),
        ("funct7", ctypes.c_uint16),
        ("funct12", ctypes.c_uint16),

        ("copcode", ctypes.c_uint16),
        ("cfunct1", ctypes.c_uint16),
        ("cfunct2high", ctypes.c_uint16),
        ("cfunct2low", ctypes.c_uint16),
        ("cfunct3", ctypes.c_uint16),
        ("cfunct5high", ctypes.c_uint16),
        ("cfunct5low", ctypes.c_uint16),

        ("decode", ctypes.c_void_p),
    ]

lib.api_decode.argtypes = [ctypes.c_uint32, ctypes.POINTER(Inst)]
lib.api_decode.restype = ctypes.c_bool

lib.api_lookup.argtypes = [ctypes.c_uint32, ctypes.POINTER(InstDef)]
lib.api_lookup.restype = ctypes.c_bool

def parse_int(s):
    s = s.strip().lower()
    if s.startswith("0b"):
        return int(s, 2)
    elif s.startswith("0x"):
        return int(s, 16)
    else:
        return int(s)

def parse_test_annos(lines):
    annos = {
        "ident": None,
        "expect": []
    }
    for line in lines:
        line = line.strip()
        if line.startswith("# @ident:"):
            ident_str = line.replace("# @ident:", "").strip()
            annos["ident"] = dict(kv.split("=") for kv in ident_str.split(", "))
        if line.startswith("# @expect:"):
            exp_str = line.replace("# @expect:", "").strip()
            expect = dict(kv.split("=") for kv in exp_str.split(", "))
            annos["expect"].append(expect)
    return annos

def compare(index, annos, inst, inst_def):
    details = {"err": False, "id": index}
    ident = annos["ident"]
    expect = annos["expect"][index]

    if "name" in ident:
        exp = ident["name"]
        act = inst_def.name.decode()
        if act != exp:
            details["name"] = f"{exp} vs {act}"
            details["err"] = True

    if "opcode" in ident:
        exp = parse_int(ident["opcode"])
        act = inst_def.opcode
        if act != exp:
            details["opcode"] = f"{exp} vs {act}"
            details["err"] = True
    if "funct2" in ident:
        exp = parse_int(ident["funct2"])
        act = inst_def.funct2
        if act != exp:
            details["funct2"] = f"{exp} vs {act}"
            details["err"] = True
    if "funct3" in ident:
        exp = parse_int(ident["funct3"])
        act = inst_def.funct3
        if act != exp:
            details["funct3"] = f"{exp} vs {act}"
            details["err"] = True
    if "funct5high" in ident:
        exp = parse_int(ident["funct5high"])
        act = inst_def.funct5high
        if act != exp:
            details["funct5high"] = f"{exp} vs {act}"
            details["err"] = True
    if "funct5low" in ident:
        exp = parse_int(ident["funct5low"])
        act = inst_def.funct5low
        if act != exp:
            details["funct5low"] = f"{exp} vs {act}"
            details["err"] = True
    if "funct7" in ident:
        exp = parse_int(ident["funct7"])
        act = inst_def.funct7
        if act != exp:
            details["funct7"] = f"{exp} vs {act}"
            details["err"] = True
    if "funct12" in ident:
        exp = parse_int(ident["funct12"])
        act = inst_def.funct12
        if act != exp:
            details["funct12"] = f"{exp} vs {act}"
            details["err"] = True

    if "copcode" in ident:
        exp = parse_int(ident["copcode"])
        act = inst_def.copcode
        if act != exp:
            details["copcode"] = f"{exp} vs {act}"
            details["err"] = True
    if "cfunct1" in ident:
        exp = parse_int(ident["cfunct1"])
        act = inst_def.cfunct1
        if act != exp:
            details["cfunct1"] = f"{exp} vs {act}"
            details["err"] = True
    if "cfunct2high" in ident:
        exp = parse_int(ident["cfunct2high"])
        act = inst_def.cfunct2high
        if act != exp:
            details["cfunct2high"] = f"{exp} vs {act}"
            details["err"] = True
    if "cfunct2low" in ident:
        exp = parse_int(ident["cfunct2low"])
        act = inst_def.cfunct2low
        if act != exp:
            details["cfunct2low"] = f"{exp} vs {act}"
            details["err"] = True
    if "cfunct3" in ident:
        exp = parse_int(ident["cfunct3"])
        act = inst_def.cfunct3
        if act != exp:
            details["cfunct3"] = f"{exp} vs {act}"
            details["err"] = True
    if "cfunct5high" in ident:
        exp = parse_int(ident["cfunct5high"])
        act = inst_def.cfunct5high
        if act != exp:
            details["cfunct5high"] = f"{exp} vs {act}"
            details["err"] = True
    if "cfunct5low" in ident:
        exp = parse_int(ident["cfunct5low"])
        act = inst_def.cfunct5low
        if act != exp:
            details["cfunct5low"] = f"{exp} vs {act}"
            details["err"] = True

    if "rd" in expect:
        exp = parse_int(expect["rd"])
        act = inst.rd
        if act != exp:
            details["rd"] = f"{exp} vs {act}"
            details["err"] = True
    if "rs1" in expect:
        exp = parse_int(expect["rs1"])
        act = inst.rs1
        if act != exp:
            details["rs1"] = f"{exp} vs {act}"
            details["err"] = True
    if "rs2" in expect:
        exp = parse_int(expect["rs2"])
        act = inst.rs2
        if act != exp:
            details["rs2"] = f"{exp} vs {act}"
            details["err"] = True
    if "imm" in expect:
        exp = parse_int(expect["imm"])
        act = inst.imm
        if act != exp:
            details["imm"] = f"{exp} vs {act}"
            details["err"] = True

    return details

def run_test(filepath):
    print(f"{os.path.basename(filepath)}: ", end="", flush=True)

    with open(filepath, "r") as f:
        lines = [l.strip() for l in f.readlines()]

    # Generate pure machine codes
    prefix = "/home/aris/opt/riscv/bin/riscv64-unknown-elf-"
    os.system(f"{prefix}gcc -c -o tmp.o {filepath}")
    os.system(f"{prefix}objcopy -O binary tmp.o tmp.bin")
    data = open("tmp.bin", "rb").read()
    os.system("rm -f tmp.o tmp.bin")

    annos = parse_test_annos(lines)
    if os.path.basename(filepath).startswith("c."):
        assert len(data) == len(annos["expect"]) * 2, f"mismatched {len(data)} vs {len(annos["expect"]) * 2}"
    else:
        assert len(data) == len(annos["expect"]) * 4, f"mismatched {len(data)} vs {len(annos["expect"]) * 4}"
    insts = [int.from_bytes(data[i:i+4], "little") for i in range(0, len(data), 4)]
    failures = []

    for i in range(0, len(insts)):
        # decode
        inst = Inst()
        lib.api_decode(insts[i], ctypes.byref(inst))
        inst_def = InstDef()
        lib.api_lookup(insts[i], ctypes.byref(inst_def))

        # compare
        details = compare(i, annos, inst, inst_def)
        if details["err"]:
            failures.append(details)

    if len(failures) == 0:
        print("PASS")
    else:
        print("[")
        for detail in failures:
            print("  {")
            for k, v in detail.items():
                print(f"    {k}: {v},")
            print("  },")
        print("]")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--all", action="store_true", help="Run all test files")
    parser.add_argument("-f", "--file", type=str, help="Run single test file")

    args = parser.parse_args()

    if args.all:
        for filename in os.listdir("playground/asm"):
            if filename.endswith(".s"):
                run_test(f"playground/asm/{filename}")
    elif args.file:
        filepath = args.file
        if not os.path.exists(filepath):
            print(f"file not exists: {filepath}")
            exit(1)
        run_test(filepath)
