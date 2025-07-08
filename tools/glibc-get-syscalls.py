#!/usr/bin/env python3
#
# (c)2025 Ira Parsons
# glibc-get-syscalls.py - enumerate syscalls used per glibc function by
#                           recovering and parsing control flow graph
#

import angr

# load glibc and linked libs
p = angr.Project('/usr/lib64/libc.so.6', load_options={ 'auto_load_libs': True });

# create a control flow graph for glibc
#  (this is static so that all possible paths are considered, dynamic would
#   freak out)
cfg = p.analyses.CFGFast();

# recover function call graph from control flow graph
cg = cfg.functions.callgraph;

for addr in cg.nodes:
	print(cfg.functions.function(addr).name);
