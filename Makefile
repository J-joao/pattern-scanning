.PHONY: dirwalk

pattern_scan: pattern_scan.c
	gcc -o $@ $< -Wextra