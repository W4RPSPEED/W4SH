{ pkgs }: {
	deps = [
   pkgs.cmake
   pkgs.openssh
		pkgs.clang
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}