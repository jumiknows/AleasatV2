#!/usr/bin/env python

import argparse

from  alea.obcfw.util.codegen import CodeGenerator
from alea.obcfw.cmd_sys.cmd_sys_codegen import CmdSysCodeGen
from alea.obcfw.log.log_codegen import LogCodeGen
from alea.obcfw.task.task_codegen import TaskCodeGen

CODE_GENERATORS: dict[str, type[CodeGenerator]] = {
    "cmd_sys" : CmdSysCodeGen,
    "log"     : LogCodeGen,
    "task"    : TaskCodeGen,
}

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="OBC Firmware Code Generator")
    parser.add_argument("target", choices=list(CODE_GENERATORS.keys()), help="Which target you want to generate")
    parser.add_argument("-i", dest="specs_paths", action="store", nargs="+", help="Input specification (JSON) files")
    parser.add_argument("-o", dest="output_dir", action="store", required=True, help="Output folder")
    args = parser.parse_args()

    cls = CODE_GENERATORS[args.target]
    codegen = cls(args.output_dir)
    codegen.generate(args.specs_paths)
