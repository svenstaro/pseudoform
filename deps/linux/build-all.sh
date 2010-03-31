#!/bin/bash

DISTRO="unknown"
WRKDIR=`pwd`

# makes declaring messages easier
message() {
	case "${1}" in
		info)
			echo -e "\033[1;34mINFO:\033[0m ${2} "
			;;
		status)
			echo -e "\033[1;32mSTATUS:\033[0m ${2} "
			;;
		warning)
			echo -e "\033[1;33mWARNING:\033[0m ${2} "
			;;
		error)
			echo -e "\033[1;31mERROR:\033[0m ${2} "
			;;
	esac
}

#
#----- Finds your distro and installs every dependancy it can ----
#

get_distro() {
	if [[ $(grep -ir "arch" /etc/issue) ]]; then
		DISTRO="archlinux"
	elif [[ -e /etc/SuSE-release ]]; then
		DISTRO="suse"
	elif [[ -e /etc/fedora-release ]]; then
		DISTRO="fedora"
	elif [[ -e /etc/debian-version ]]; then
		DISTRO="debian"
	elif [[ $(grep -ir "ubuntu" /etc/lsb-release) ]]; then
		DISTRO="ubuntu"
	elif [[ $(grep -ir "LinuxMint" /etc/lsb-release) ]]; then
		DISTRO="linuxmint"
	else 
		message "error" "Your distribution could not be detected."
		message "error" "Please report this to the developers."
		return 1
	fi
	message "info" "Your distribution has been detected as ${DISTRO}."
}

install_deps() {
	case "${1}" in
		archlinux)
			if [[ ! -f $(which yaourt) ]]; then
				message "error" "You will need to have yaourt installed."
				message "error" "Please have a look here: http://archlinux.fr/yaourt-en"
				return 1
			fi
			sudo yaourt -Sy --noconfirm base-devel subversion git cmake ogre-svn bullet-svn codeblocks-svn fmodex
			;;
		suse)
			message "error" "SuSE distributions are currently not supported because I have no clue at all about them."
			message "error" "If you want to be awesome, submit a patch to fix this :)."
			return 1
			;;
		fedora)
			message "info" "You will manually need to install the NVIDIA Cg toolkit for your architecture."
			message "info" "Get it here: http://developer.nvidia.com/object/cg_download.html"
			read -p "Press return to continue "
			sudo yum install make automake autoconf gcc gcc-c++ pkgconfig subversion git cmake ois freeimage freetype zlib zziplib gdb codeblocks boost libXaw libXrandr checkinstall
			;;
		debian)
			message "info" "You will need to enable all repositories before attempting the installation."
			read -p "Press return to continue "
			sudo apt-get install build-essential automake pkg-config subversion git cmake libois-dev gdb codeblocks-contrib nvidia-cg-toolkit libfreeimage-dev libboost-dev libfreetype6-dev libxaw7-dev libxrandr-dev libzzip-dev zlib1g-dev checkinstall
			;;
		ubuntu)
			message "info" "You will need to enable all repositories before attempting the installation."
			read -p "Press return to continue "
			sudo apt-get install build-essential automake pkg-config subversion git cmake libois-dev gdb codeblocks-contrib nvidia-cg-toolkit libfreeimage-dev libboost-dev libfreetype6-dev libxaw7-dev libxrandr-dev libzzip-dev zlib1g-dev checkinstall
			;;
		linuxmint)
			message "info" "You will need to enable all repositories before attempting the installation."
			read -p "Press return to continue "
			sudo apt-get install build-essential automake pkg-config subversion git cmake libois-dev gdb codeblocks-contrib nvidia-cg-toolkit libfreeimage-dev libboost1.37-dev libfreetype6-dev libxaw7-dev libxrandr-dev libzzip-dev zlib1g-dev checkinstall
			;;
	esac
}

#
#----- Defining all build and download functions -----
#

make_ogre() {
	if [ ! -f ogre/build/ogre-svn*.??? ]; then
		if [ -d ogre ]; then
			echo "Existing ogre directory found. Won't get it again."
		else
			wget -c https://sourceforge.net/projects/ogre/files/ogre/1.7/ogre-v1-7-0p1.tar.bz2/download
			tar xjvf ogre-v1-7-0p1.tar.bz2
		fi
		cd ogre
		if [ ! -d build ]; then
			mkdir build
		fi
		cd build
		cmake .. || return 2
		make -j3 || return 2
		sudo checkinstall --pkgname ogre-svn --install --pkgversion 1.7 -y
		cd ../..
	else
		message "status" "Ogre already built and installed, so there's no need to do it again."
	fi
}

get_newton() {
	if [ -d newtonSDK ]; then
		message "info" "Existing newton directory found. Won't get it again."
	else
		if [ $(uname -m) == "x86_64" ]; then
		message "info" "Your machine has a 64bit architecture"		
		wget -c http://www.newtondynamics.com/downloads/NewtonLinux-64-2.17.tar.gz
		tar xf NewtonLinux-64-2.17.tar.gz
		else
		message "info" "Your machine has an 32bit architecture"		
		wget -c http://www.newtondynamics.com/downloads/NewtonLinux-32-2.17.tar.gz
		tar xf NewtonLinux-32-2.17.tar.gz newtonSDK
		fi
	fi
}

build_libnewton_utils() {
	if [ ! -f newton/sdk/libnewton_utils.a ]; then
		cd newton
		make || return 4
		cd ..
	else
		message "info" "You already have libnewton_utils.a"
	fi
}

make_bullet() {
	if [ ! -f bullet-2.75/build/bullet-physics*.??? ]; then
		if [ -d bullet-2.75 ]; then
			message "info" "Bullet already downloaded, won't get"
		else
			wget -c http://bullet.googlecode.com/files/bullet-2.75.tgz
			tar xf bullet-2.75.tgz bullet-2.75
			message "status" "Downloaded bulletphysics archive"
		fi
		cd bullet-2.75
		mkdir build
		cd build
		cmake ../src || return 5
		make -j3
		sudo checkinstall --pkgname bullet-physics --install --pkgversion 2.75 -y
		message "status" "made bulletphysics"
		cd ../../
	else
	message "status" "Bullet is already built and installed, so no need to do that again."
	fi
}

remake_protobuf() {
	cd ../../src/protobuf
	protoc level_format.proto --cpp_out=.
	cd ../../deps/linux
	message "status" "Remade protobuf source files to fit your computer."
}

make_chaiscript() {
	if [ ! -d /usr/include/chaiscript ]; then
		if [ -d  chaiscript-2.2-linux ]; then
			message "info" "Chaiscript already downloaded, won't get"
		else
			wget -c http://chaiscript.googlecode.com/files/chaiscript-2.2-linux.tar.bz2
			tar xjvf chaiscript-2.2-linux.tar.bz2 chaiscript-2.2-linux
			message "status" "Downloaded and unpacked chaiscript archive"
		fi
		cd chaiscript-2.2-linux
		perl -p -i.bak -e "s|/home/jason/Programming/chaiscript-2.2-linux|`pwd`|g" CMakeCache.txt
		. build_packages.sh || return 7
		sudo cp -r include/chaiscript /usr/include
		message "status" "Made and installed chaiscript!"
		cd ..
	else
	message "status" "Chaiscript is already here, no need to do that again."
	fi
}

#
#----- This is where the script starts -----
#

message "warning" "This script is really hacky and probably won't work for all systems."
message "warning" "Please help us improve it! Send error reports and patches if there are any problems."
sleep 3
get_distro || return 1 
install_deps ${DISTRO} || return 1

if [[ ${DISTRO} == "archlinux" ]]; then
	get_newton || echo "Couldn't get Newton"; return 3
	# If archlinux got this far, everything is already built
	message "info" "Everything is done :)."
	exit 0
fi

message "status" "Everything looks good, starting build process."

make_ogre || return 2
get_newton || return 3
build_libnewton_utils || return 4
# cmake in bullet isn't working right now, and it's not necessary so it's commented out
#make_bullet || return 5
remake_protobuf || return 6
make_chaiscript || return 7

# pseudoform/deps/linux/build-all.sh
# vim: ts=2 sw=2
