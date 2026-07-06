import os
import lit.formats
from lit.llvm import llvm_config
from lit.llvm.subst import ToolSubst

config.name = "ADL"
config.test_format = lit.formats.ShTest(not llvm_config.use_lit_shell)
config.suffixes = [".mlir", ".adl", ".sail"]
config.excludes = ["Inputs"]

config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.adl_obj_root, "tests")

llvm_config.use_default_substitutions()

tool_dirs = [
    config.adl_tools_dir,
    config.llvm_tools_dir,
]

tools = [
    ToolSubst("adl-opt", unresolved="fatal"),
    ToolSubst("FileCheck", unresolved="fatal"),
    ToolSubst("not", unresolved="fatal"),
    ToolSubst("count", unresolved="fatal"),
]

llvm_config.add_tool_substitutions(tools, tool_dirs)
