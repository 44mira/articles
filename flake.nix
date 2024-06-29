{
  description = "Haskell Development Flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          ghc
        ];

        NAME = "haskell-devshell";

        shellHook = /* bash */ ''
          printf "\n\nEntering %s:\n\n" $NAME
        '';
      };
    };
}
