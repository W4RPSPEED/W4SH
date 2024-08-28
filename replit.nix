{ pkgs }: {
	deps = [
   pkgs.openssh
		pkgs.clang
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}