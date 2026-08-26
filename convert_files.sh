#!/bin/bash

shopt -s nullglob

DOS_UNIX="dos2unix"
ICONV=(iconv -f CP1251 -t UTF-8)
#ELF (exported from bashrc)
MAIN="main"
NOT_FOUND="NOT_FOUND_CODES.txt"

GREEN='\e[32m'
PURPLE='\e[35m'
RESET='\e[0m'
echo -e "---Hi! You want to process the directory: ${GREEN}$(pwd)${RESET}"
read -r -n 1 -p "---Is that correct?(y/n)	" choice
echo ""
case "$choice" in
	[yY])
#		echo "---Sir,yes sir!"
		;;
	*)
		echo "---Fine,then we exit!"
		exit 1
		;;
esac

files=$(find . -maxdepth 1 -type f -name "*.txt")
if ! [[ -n "$files" ]]; then
	echo -e "---Seems like there is no ${GREEN}'.txt'${RESET} file in this directory.${PURPLE}Terminating process${RESET}"
	exit 1
fi

fin_files=$(find . -maxdepth 1 -type f -name "*_FIN*")
if [[ -n "$fin_files" ]]; then
	echo  "---Double running script. You already have the '_FINAL.txt' files.${PURPLE}Terminating process${RESET}"
	exit 1
fi

for file in *; do
	if [[ "$file" != *.txt ]]; then
		echo "---Skipping file: "$file""
		continue;
	fi
	if [[ -f "$file" ]]; then
		"$DOS_UNIX" "$file" 2>/dev/null
		if [[ $? -ne 0 ]]; then
			echo -e "---${PURPLE}Error${RESET} while converting to Unix-like newlines: ${GREEN}"$file"${RESET}"
			exit 1
		fi
		"${ICONV[@]}" "$file" -o "$file" 2>/dev/null
		if [[ $? -ne 0 ]]; then
			echo -e "---${PURPLE}Error${RESET} while converting charset to UTF-8: ${GREEN}"$file"${RESET}"
#			exit 1
		fi
	fi
done
echo "-------------------------------------------------------------------"
echo -e "---${GREEN}All converting is done.${RESET}"
echo "-------------------------------------------------------------------"

if [[ ! -e "$ELF" ]]; then
	echo -e "---${PURPLE}Error${RESET}: ELF file ${GREEN}'main'${RESET} was not found.${PURPLE}Terminating process${RESET}"
	exit 1
fi

cp "$ELF" .
if [[ $? -ne 0 ]]; then
	echo -e "---${PURPLE}Error${RESET} while copying ELF file ${GREEN}'main'${RESET}.${PURPLE}Terminating process${RESET}"
	exit 1
fi

./"$MAIN"
if [[ $? -ne 0 ]]; then
	echo -e "---${PURPLE}Error${RESET} while executing ELF file ${GREEN}'main'${RESET}.${PURPLE}Terminating process${RESET}"
	exit 1
fi
echo "-------------------------------------------------------------------"
echo -e "---${GREEN}Script worked successfully,enjoy results.${RESET}"
echo "-------------------------------------------------------------------"

trash-put "$MAIN"
if [[ $? -ne 0 ]]; then
	echo -e "---${PURPLE}Error${RESET} while deleting ELF file ${GREEN}'main'${RESET}."
fi

if [[ -e "$NOT_FOUND" ]]; then
	if [[ ! -s "$NOT_FOUND" ]]; then
		trash-put "$NOT_FOUND"
	else
		codes=$(wc -l < "$NOT_FOUND")
		echo -e "---${PURPLE}Some codes not found so they placed in \""$NOT_FOUND"\" file: $codes${RESET}"
	fi
fi
