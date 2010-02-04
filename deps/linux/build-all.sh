#!/bin/bash

DISTRO="unknown"

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
			sudo apt-get install build-essential automake pkg-config subversion git cmake libois-dev gdb codeblocks-contrib nvidia-cg-toolkit libfreeimage-dev libboost-dev libfreetype6-dev libxaw7-dev libxrandr-dev libzzip-dev zlib1g-dev checkinstall
			;;
	esac
}

make_ogre() {
	if [ -d ogre ]; then
		echo "Existing ogre directory found. Won't get it again."
	else
		wget -c http://downloads.sourceforge.net/project/ogre/ogre/1.7/ogre-v1-7-0RC1.tar.bz2
		tar xjvf ogre-v1-7-0RC1.tar.bz2
	fi
	cd ogre
	mkdir build
	cd build
	cmake .. || return 1
	make -j3 || return 1
	sudo checkinstall --pkgname ogre-svn --install --pkgversion 1.7 -y
}

get_newton() {
	if [ -d newtonSDK ]; then
		echo "Existing newton directory found. Won't get it again."
	else
		wget -c http://www.newtondynamics.com/downloads/NewtonLinux-32-2.17.tar.gz
		tar xf NewtonLinux-32-2.17.tar.gz newtonSDK
	fi
}

make_bullet() {
	if [ -d bullet-2.75 ]; then
		echo "Bullet already downloaded, won't get"
	else
		wget -c http://bullet.googlecode.com/files/bullet-2.75.tgz
		tar xf bullet-2.75.tgz bulletphysics
		message "status" "downloaded bulletphysics archive"
	fi
	cd bullet-2.75/src
	cmake . || echo "to remake first delete the bullet-2.75 directory"; return 1
	sudo checkinstall --pkgname bullet-physics --install --pkgversion 2.75 -y
	message "status" "made bulletphysics"
}

message "warning" "This script is really hacky and probably won't work for all systems."
message "warning" "Please help us improve it! Send error reports and patches if there are any problems."
sleep 3
get_distro || exit 1 
install_deps ${DISTRO} || exit 1

if [[ ${DISTRO} == "archlinux" ]]; then
	get_newton || echo "Couldn't get Newton"; exit 1
	# If archlinux got this far, everything is already built
	message "info" "Everything is done :)."
	exit 0
fi

message "status" "Everything looks good, starting build process."

make_ogre || exit 1
get_newton || exit 1
make_bullet || exit 1

# vim: ts=2 sw=2
