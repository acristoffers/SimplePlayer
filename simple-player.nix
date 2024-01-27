{ stdenv, lib, qmake, qt5, wrapQtAppsHook }:

stdenv.mkDerivation {
  pname = "simple-player";
  version = "1.0.0";
  src = ./.;
  buildInputs = with qt5; [ qtbase qtmultimedia ];
  enableParallelBuilding = true;
  nativeBuildInputs = [ qmake wrapQtAppsHook ];
}
