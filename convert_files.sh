#!/bin/bash

shopt -s nullglob

DOS_UNIX="dos2unix"
ICONV=(iconv -f CP1251 -t UTF-8)
#ELF (exported from bashrc)
MAIN="main"

YELLOW='\e[33m'
RESET='\e[0m'
echo -e "---Hi! You want to process the directory: ${YELLOW}$(pwd)${RESET}"
read -r -n 1 -p "---Is that correct?(y/n)"   choice
echo ""
case "$choice" in
	[yY])
		echo "---Sir,yes sir!"
		;;
	*)
		echo "---Fine,then we exit!"
		exit 1
		;;
esac

files=$(find . -maxdepth 1 -type f -name "*.txt")
if ! [[ -n "$files" ]]; then
	echo -e "---Seems like there is no ${YELLOW}'.txt'${RESET} file in this directory.Terminating process."
	exit 1
fi

for file in *; do
	if [ -f "$file" ]; then
		"$DOS_UNIX" "$file" 2>/dev/null
		if [ $? -ne 0 ]; then
			echo -e "---Error while converting to Unix-like newlines: ${YELLOW}"$file"${RESET}"
			exit 1
		fi
		"${ICONV[@]}" "$file" -o "$file"
		if [ $? -ne 0 ]; then
			echo -e "---Error while converting charset to UTF-8: ${YELLOW}"$file"${RESET}"
			exit 1
		fi
	fi
done
echo "-------------------------------------------------------------------"
echo -e "---${YELLOW}All converting is done.${RESET}"
echo "-------------------------------------------------------------------"

if [[ ! -e "$ELF" ]]; then
	echo -e "---Error: ELF file ${YELLOW}'main'${RESET} was not found.Terminating process."
	exit 1
fi

cp "$ELF" .
if [ $? -ne 0 ]; then
	echo -e "---Error while copying ELF file ${YELLOW}'main'${RESET}.Terminating process."
	exit 1
fi

./"$MAIN"
if [ $? -ne 0 ]; then
	echo "---Error while executing ELF file ${YELLOW}'main'${RESET}.Terminating process."
	exit 1
fi
echo "-------------------------------------------------------------------"
echo -e "---${YELLOW}Script and elf worked successfully,enjoy results.${RESET}"
echo "-------------------------------------------------------------------"

trash-put "$MAIN"
if [ $? -ne 0 ]; then
	echo "---Error while deleting ELF file ${YELLOW}'main'${RESET}."
fi
