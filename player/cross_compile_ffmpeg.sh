#!/usr/bin/env bash
################################################################################
# ffmpeg windows cross compile helper/downloader script
################################################################################
# Copyright (C) 2012 Roger Pack
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#
# The GNU General Public License can be found in the LICENSE file.

yes_no_sel () {
unset user_input
local question="$1"
shift
while [[ "$user_input" != [YyNn] ]]; do
  echo -n "$question"
  read user_input
  if [[ "$user_input" != [YyNn] ]]; then
    clear; echo 'Your selection was not vaild, please try again.'; echo
  fi
done
# downcase it
user_input=$(echo $user_input | tr '[A-Z]' '[a-z]')
}

check_missing_packages () {
local check_packages=('curl' 'pkg-config' 'make' 'git' 'svn' 'cmake' 'gcc' 'autoconf' 'libtool' 'automake' 'yasm' 'cvs' 'flex' 'bison' 'makeinfo')
for package in "${check_packages[@]}"; do
  type -P "$package" >/dev/null || missing_packages=("$package" "${missing_packages[@]}")
done

if [[ -n "${missing_packages[@]}" ]]; then
  clear
  echo "Could not find the following execs: ${missing_packages[@]}"
  echo 'Install the missing packages before running this script.'
  exit 1
fi

local out=`cmake --version` # like cmake version 2.8.7
local version_have=`echo "$out" | cut -d " " -f 3`

function version { echo "$@" | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }'; }

if [[ $(version $version_have)  < $(version '2.8.10') ]]; then
  echo "your cmake version is too old $version_have wanted 2.8.10"
  exit 1
fi

}


cur_dir="$(pwd)/sandbox"
cpu_count="$(grep -c processor /proc/cpuinfo)" # linux only <sigh>
if [ -z "$cpu_count" ]; then
  cpu_count=`sysctl -n hw.ncpu | tr -d '\n'` # OS X
  if [ -z "$cpu_count" ]; then
    echo "warning, unable to determine cpu count, defaulting to 1"
    cpu_count=1 # boxes where we don't know how to determine cpu count [OS X for instance], default to just 1, instead of blank, which means infinite 
  fi
fi
original_cpu_count=$cpu_count # save it away for some that revert it temporarily

intro() {
  cat <<EOL
     ##################### Welcome ######################
  Welcome to the ffmpeg cross-compile builder-helper script.
  Downloads and builds will be installed to directories within $cur_dir
  If this is not ok, then exit now, and cd to the directory where you'd
  like them installed, then run this script again from there.  
  NB that once you build your compilers, you can no longer rename/move
  the sandbox directory, since it will have some hard coded paths in there.
  You can, of course, rebuild ffmpeg from within it, etc.
EOL
  if [[ $sandbox_ok != 'y' ]]; then
    yes_no_sel "Is ./sandbox ok (requires ~ 5GB space) [y/n]?"
    if [[ "$user_input" = "n" ]]; then
      exit 1
    fi
  fi
  mkdir -p "$cur_dir"
  cd "$cur_dir"
}

pick_compiler_flavors() {
  build_choice=win32
}

install_cross_compiler() {
  if [[ -f "mingw-w64-i686/compiler.done" || -f "mingw-w64-x86_64/compiler.done" ]]; then
   echo "MinGW-w64 compiler of some type or other already installed, not re-installing..."
   if [[ $rebuild_compilers != "y" ]]; then
     return # early exit
   fi
  fi

  pick_compiler_flavors 
  curl https://raw.github.com/rdp/ffmpeg-windows-build-helpers/master/patches/mingw-w64-build-3.2.0 -O  || exit 1
  chmod u+x mingw-w64-build-3.2.0
  # requires mingw-w64 svn: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=55706
  # cpu count 1 because otherwise with 945 MB RAM it can segfault ?
  nice ./mingw-w64-build-3.2.0 --mingw-w64-ver=svn --disable-shared --default-configure --clean-build --cpu-count=1 --threads=pthreads-w32 --pthreads-w32-ver=2-9-1 --build-type=$build_choice || exit 1 # --disable-shared allows c++ to be distributed at all...which seemed necessary for some random dependency...

  if [ -d mingw-w64-x86_64 ]; then
    touch mingw-w64-x86_64/compiler.done
  fi
  if [ -d mingw-w64-i686 ]; then
    touch mingw-w64-i686/compiler.done
  fi
  clear
  echo "Ok, done building MinGW-w64 cross-compiler..."
}

setup_env() {
  export PKG_CONFIG_LIBDIR= # disable pkg-config from reverting back to and finding system installed packages [yikes]
}

do_svn_checkout() {
  repo_url="$1"
  to_dir="$2"
  if [ ! -d $to_dir ]; then
    echo "svn checking out to $to_dir"
    svn checkout $repo_url $to_dir.tmp || exit 1
    mv $to_dir.tmp $to_dir
  else
    cd $to_dir
    echo "not svn Updating $to_dir since usually svn repo's aren't frequently updated..."
    # svn up
    cd ..
  fi
}

update_to_desired_branch_or_revision() {
  local to_dir="$1"
  local desired_branch="$2"
  if [ -n "$desired_branch" ]; then
   pushd $to_dir
   cd $to_dir
      echo "git co $desired_branch"
      git checkout  "$desired_branch" || exit 1
      git merge "$desired_branch" || exit 1 # depending on which type it is :)
   popd # in case it's a cd to ., don't want to cd to .. here...
  fi
}

do_git_checkout() {
  local repo_url="$1"
  local to_dir="$2"
  local desired_branch="$3"
  if [ ! -d $to_dir ]; then
    echo "Downloading (via git clone) $to_dir"
    # prevent partial checkouts by renaming it only after success
    git clone $repo_url $to_dir.tmp || exit 1
    mv $to_dir.tmp $to_dir
    echo "done downloading $to_dir"
    update_to_desired_branch_or_revision $to_dir $desired_branch
  else
    cd $to_dir
    echo "Updating to latest $to_dir version..."
    old_git_version=`git rev-parse HEAD`
    git pull # if you comment out, add an echo :)
    update_to_desired_branch_or_revision "." $desired_branch
    new_git_version=`git rev-parse HEAD`
    if [[ "$old_git_version" != "$new_git_version" ]]; then
     echo "got upstream changes, forcing re-configure."
     rm already*
    else
     echo "this pull got no new upstream changes, possibly not forcing re-configure..."
    fi 
    cd ..
  fi
}

get_small_touchfile_name() { # have to call with assignment like a=$(get_small...)
  local beginning="$1"
  local extra_stuff="$2"
  local touch_name="${beginning}_$(echo -- $extra_stuff | /usr/bin/env md5sum)" # make it smaller
  touch_name=$(echo $touch_name | sed "s/ //g") # md5sum introduces spaces, remove them
  echo $touch_name # bash cruddy return system LOL
} 

do_configure() {
  local configure_options="$1"
  local configure_name="$2"
  if [[ "$configure_name" = "" ]]; then
    configure_name="./configure"
  fi
  local cur_dir2=$(pwd)
  local english_name=$(basename $cur_dir2)
  local touch_name=$(get_small_touchfile_name already_configured "$configure_options")
  if [ ! -f "$touch_name" ]; then
    make clean # just in case
    #make uninstall # does weird things when run under ffmpeg src
    if [ -f bootstrap.sh ]; then
      ./bootstrap.sh
    fi
    rm -f already_* # reset
    echo "configuring $english_name as $ PATH=$PATH $configure_name $configure_options"
    nice "$configure_name" $configure_options || exit 1
    touch -- "$touch_name"
    make clean # just in case
  else
    echo "already configured $(basename $cur_dir2)" 
  fi
}

do_make() {
  local extra_make_options="$1 -j $cpu_count"
  local cur_dir2=$(pwd)
  local touch_name=$(get_small_touchfile_name already_ran_make "$extra_make_options")

  if [ ! -f $touch_name ]; then
    echo "making $cur_dir2 as $ PATH=$PATH make $extra_make_options"
    nice make $extra_make_options || exit 1
    touch $touch_name
  else
    echo "already did make $(basename "$cur_dir2")"
  fi
}

do_make_install() {
  local extra_make_options="$1"
  do_make "$extra_make_options"
  local touch_name=$(get_small_touchfile_name already_ran_make_install "$extra_make_options")
  if [ ! -f $touch_name ]; then
    echo "make installing $cur_dir2 as $ PATH=$PATH make install $extra_make_options"
    nice make install $extra_make_options || exit 1
    touch $touch_name
  fi
}

apply_patch() {
 local url=$1
 local patch_name=$(basename $url)
 local patch_done_name="$patch_name.done"
 if [[ ! -e $patch_done_name ]]; then
   curl $url -O || exit 1
   patch -p0 < "$patch_name" || exit 1
   touch $patch_done_name
 else
   echo "patch $patch_name already applied"
 fi
}

download_and_unpack_file() {
  url="$1"
  output_name=$(basename $url)
  output_dir="$2"
  if [ ! -f "$output_dir/unpacked.successfully" ]; then
    curl "$url" -O -L || exit 1
    tar -xf "$output_name" || unzip $output_name || exit 1
    touch "$output_dir/unpacked.successfully" || exit 1
    rm "$output_name"
  fi
}

generic_configure() {
  local extra_configure_options="$1"
  do_configure "--host=$host_target --prefix=$mingw_w64_x86_64_prefix --disable-shared --enable-static $extra_configure_options"
}

# needs 2 parameters currently
generic_download_and_install() {
  local url="$1"
  local english_name="$2" 
  local extra_configure_options="$3"
  download_and_unpack_file $url $english_name
  cd $english_name || exit "needs 2 parameters"
  generic_configure_make_install $extra_configure_options
  cd ..
}

generic_configure_make_install() {
  generic_configure $1
  do_make_install
}

build_ffmpeg() {
  #build only shared version
  do_git_checkout https://github.com/FFmpeg/FFmpeg.git ffmpeg_git_shared release/1.2
  local extra_configure_opts="--enable-shared --disable-static"
  cd ffmpeg_git_shared

  if [ "$bits_target" = "32" ]; then
   local arch=x86
  else
   local arch=x86_64
  fi

config_options="--arch=$arch --target-os=mingw32 --cross-prefix=$cross_prefix --pkg-config=pkg-config --disable-programs --disable-doc --disable-w32threads --disable-debug --extra-cflags=-DPTW32_STATIC_LIB  $extra_configure_opts "

  if [[ "$native_build" = "y" ]]; then
    config_options="$config_options --disable-runtime-cpudetect"
    # TODO --cpu=host ...
  else
    config_options="$config_options --enable-runtime-cpudetect"
  fi
  
  do_configure "$config_options"
  echo $config_options > config_options.txt
  rm -f */*.a */*.dll *.exe # just in case some dependency library has changed, force it to re-link even if the ffmpeg source hasn't changed...
  rm already_ran_make*
  echo "doing ffmpeg make $(pwd)"
  do_make
  echo "Done! You will find $bits_target bit $shared binaries in $(pwd)/ff{mpeg,probe,play}*.exe"
  cd ..
  echo "Copying to folder"
  rm -rf BUILD
  mkdir BUILD
  mkdir BUILD/dll
  mkdir BUILD/lib
  find . -type f -name *.dll -exec cp {} BUILD/dll \;
  find . -type f -name *.lib -exec cp {} BUILD/lib \;
}

while true; do
  case $1 in
    -h | --help ) echo "options: --disable-nonfree=y --sandbox-ok=y --rebuild-compilers=y"; exit 0 ;;
    --sandbox-ok=* ) sandbox_ok="${1#*=}"; shift ;;
    --disable-nonfree=* ) disable_nonfree="${1#*=}"; shift ;;
    --rebuild-compilers=* ) rebuild_compilers="${1#*=}"; shift ;;
    -- ) shift; break ;;
    -* ) echo "Error, unknown option: '$1'."; exit 1 ;;
    * ) break ;;
  esac
done

intro # remember to always run the intro, since it adjust pwd
check_missing_packages
install_cross_compiler # always run this, too, since it adjust the PATH
setup_env

original_path="$PATH"
if [ -d "mingw-w64-i686" ]; then # they installed a 32-bit compiler
  echo "Building 32-bit ffmpeg..."
  host_target='i686-w64-mingw32'
  mingw_w64_x86_64_prefix="$cur_dir/mingw-w64-i686/$host_target"
  export PATH="$cur_dir/mingw-w64-i686/bin:$original_path"
  export PKG_CONFIG_PATH="$cur_dir/mingw-w64-i686/i686-w64-mingw32/lib/pkgconfig"
  bits_target=32
  cross_prefix="$cur_dir/mingw-w64-i686/bin/i686-w64-mingw32-"
  mkdir -p win32
  cd win32
  build_ffmpeg
  cd ..
fi

if [ -d "mingw-w64-x86_64" ]; then # they installed a 64-bit compiler
  echo "Building 64-bit ffmpeg..."
  host_target='x86_64-w64-mingw32'
  mingw_w64_x86_64_prefix="$cur_dir/mingw-w64-x86_64/$host_target"
  export PATH="$cur_dir/mingw-w64-x86_64/bin:$original_path"
  export PKG_CONFIG_PATH="$cur_dir/mingw-w64-x86_64/x86_64-w64-mingw32/lib/pkgconfig"
  mkdir -p x86_64
  bits_target=64
  cross_prefix="$cur_dir/mingw-w64-x86_64/bin/x86_64-w64-mingw32-"
  cd x86_64
  build_ffmpeg
  cd ..
fi

echo "done with ffmpeg cross compiler script, it may have built the following binaries: 
$(find . -name ffmpeg.exe)"
