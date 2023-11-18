{
  description = "Simple Media Player";
  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixpkgs-unstable;
    flake-utils.url = github:numtide/flake-utils;
  };
  outputs = { self, flake-utils, nixpkgs }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = (import nixpkgs) { inherit system; };
        buildInputs = with pkgs; [ ];
      in
      rec {
        formatter = nixpkgs.legacyPackages.${system}.nixpkgs-fmt;
        packages = {
          default = packages.simple-player;
          simple-player = pkgs.libsForQt5.callPackage ./simple-player.nix { };
        };
        apps = rec {
          default = simple-player;
          simple-player = { type = "app"; program = "${packages.default}/bin/simple-player"; };
        };
        devShell = pkgs.mkShell {
          inherit buildInputs;
          nativeBuildInputs = with pkgs; [ cmake pkg-config ];
        };
      }
    );
}
