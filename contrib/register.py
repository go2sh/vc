import gdb.printing
from printers import StringRefPrinter, FormatTokenPrinter

def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter(
        "vc")
    pp.add_printer('StringRef', '^vc::StringRef$', StringRefPrinter)
    pp.add_printer('FormatToken', '^vc::format::FormatToken$', FormatTokenPrinter)
    return pp