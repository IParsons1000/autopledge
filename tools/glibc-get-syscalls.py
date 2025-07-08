#!/usr/bin/env python3
#
# (c)2025 Ira Parsons
# glibc-get-syscalls.py - enumerate syscalls used per glibc function by
#                           recovering and parsing control flow graph
#

import angr
import networkx

# load glibc and linked libs
p = angr.Project('/usr/lib64/libc.so.6', load_options={ 'auto_load_libs': True });

# create a control flow graph for glibc
#  (this is static so that all possible paths are considered, dynamic would
#   freak out)
cfg = p.analyses.CFGFast();

# recover function call graph from control flow graph
cg = cfg.functions.callgraph;

# create subgraph from function
sg = cg.subgraph(networkx.single_source_shortest_path(cg, cfg.functions.function(name="_exit").addr).keys());

# locate nodes at the end of the line
for n, d in sg.out_degree():
	if d == 0:
		print(cfg.functions.function(n).name);
