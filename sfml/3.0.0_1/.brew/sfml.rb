class Sfml < Formula
  # Don't update SFML until there's a corresponding CSFML release
  desc "Multi-media library with bindings for multiple languages"
  homepage "https://www.sfml-dev.org/"
  url "https://www.sfml-dev.org/files/SFML-3.0.0-sources.zip"
  sha256 "8cc41db46b59f07c44ecf21c74a0f956d37735dec9d90ff4522856cb162ba642"
  license "Zlib"
  revision 1
  head "https://github.com/SFML/SFML.git", branch: "master"

  depends_on "cmake" => :build
  depends_on "doxygen" => :build
  depends_on "pkgconf" => :build
  depends_on "flac"
  depends_on "freetype"
  depends_on "libogg"
  depends_on "libvorbis"

  on_linux do
    depends_on "libx11"
    depends_on "libxcursor"
    depends_on "libxi"
    depends_on "libxrandr"
    depends_on "mesa"
    depends_on "mesa-glu"
    depends_on "openal-soft"
    depends_on "systemd"
  end

  def install
    # Fix "fatal error: 'os/availability.h' file not found" on 10.11 and
    # "error: expected function body after function declarator" on 10.12
    # Requires the CLT to be the active developer directory if Xcode is installed
    ENV["SDKROOT"] = MacOS.sdk_path if OS.mac? && MacOS.version <= :high_sierra

    # Always remove the "extlibs" to avoid install_name_tool failure
    # (https://github.com/Homebrew/homebrew/pull/35279) but leave the
    # headers that were moved there in https://github.com/SFML/SFML/pull/795
    rm_r(Dir["extlibs/*"] - ["extlibs/headers"])

    args = [
      "-DBUILD_SHARED_LIBS=ON",
      "-DCMAKE_INSTALL_RPATH=#{rpath}",
      "-DSFML_INSTALL_PKGCONFIG_FILES=TRUE",
      "-DSFML_BUILD_DOC=TRUE",
      "-DSFML_USE_SYSTEM_DEPS=ON",
    ]

    system "cmake", "-S", ".", "-B", "build", *args, *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--build", "build", "--target=doc"
    system "cmake", "--install", "build"
  end

  test do
    (testpath/"test.cpp").write <<~CPP
      #include "Time.hpp"
      int main() {
        sf::Time t1 = sf::milliseconds(10);
        return 0;
      }
    CPP
    system ENV.cxx, "-I#{include}/SFML/System", "-std=c++17", testpath/"test.cpp",
                    "-L#{lib}", "-lsfml-system", "-o", "test"
    system "./test"
  end
end
