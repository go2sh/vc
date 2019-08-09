import gdb.printing
import sys
import os.path
sys.path.insert(0, os.path.dirname(__file__))

import register

gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    register.build_pretty_printer())