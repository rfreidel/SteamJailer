#!/bin/sh

USAGE="Usage:\n${0##*/} -c|--create||--d|destroy --jail|-j=JAIL_NAME --sequence|-s=PAIR_SEQUENCE [--public-bridge|-b] [--private-bridge|-p]"

# Parse command-line arguments
for i in "$@"
do
  case $i in
    --help)
      printf "${USAGE}\n"
      exit
      ;;
    -j=*|--jail=*)
      JAIL="${i#*=}"
      shift
      ;;
    -s=*|--sequence=*)
      SEQUENCE="${i#*=}"
      shift
      ;;
    -b|--public-bridge)
      PUBLIC=1
      shift
      ;;
    -p|--private-bridge)
      PRIVATE=1
      shift
      ;;
    -c|--create)
      CREATE=1
      [ ! -z ${DESTROY} ] && printf "ERROR: --create or --destroy is required, please select only one.\n\n${USAGE}\n" && exit 1 >&2
      shift
      ;;
    -d|--destroy)
      DESTROY=1
      [ ! -z ${CREATE} ] && printf "ERROR: --create or --destroy is required, please select only one.\n\n${USAGE}\n" && exit 1 >&2
      shift
      ;;
    --debug)
      DEBUG=1
      shift
      ;;
    *)
      # unknown option
      ;;
  esac
done

if [ ! -z ${DEBUG} ]; then
  printf "JAIL: ${JAIL}\nSEQUENCE: ${SEQUENCE}\nCREATE: ${CREATE}\nDESTROY: ${DESTROY}\nPUBLIC: ${PUBLIC}\nPRIVATE: ${PRIVATE}\n"
fi

if [ -z ${JAIL} ] && [ ! -z ${CREATE} ]; then
  printf "ERROR: Missing jail name.\n\n${USAGE}\n" >&2
  exit 1
fi

if [ -z ${SEQUENCE} ] && [ ! -z ${CREATE} ]; then
  printf "ERROR: Missing interface sequence number.\n\n${USAGE}\n" >&2
  exit 1
fi

if [ -z ${CREATE} ] && [ -z ${DESTROY} ]; then
  printf "ERROR: --create or --destroy is required, please select only one.\n\n${USAGE}\n" >&2
  exit 1
fi

if [ ! -z ${CREATE} ]; then
  ep=$(ifconfig epair create)
  seq=$( echo "$ep" | sed 's/^epair\([0-9]\{1,\}\)a$/\1/g')
  if [ -z "${seq##*[!0-9]*}" ]; then
    printf "Error: creating epair, check the created epair\n" >&2
    exit 1
  fi
  
  ifconfig "epair${seq}a" name "e${SEQUENCE}a_${JAIL}"
  ifconfig "epair${seq}b" name "e${SEQUENCE}b_${JAIL}"
  
  if [ ! -z ${PUBLIC} ]; then
    ifconfig bridge0 create
    ifconfig bridge0 name jailetherbridge
    ifconfig jailetherbridge up
    ifconfig jailetherbridge addm "e${SEQUENCE}a_${JAIL}"
  fi
  
  if [ ! -z ${PRIVATE} ]; then
    ifconfig bridge1 create
    ifconfig bridge1 name jailprivbridge
    ifconfig jailprivbridge up
    ifconfig jailprivbridge addm "e${SEQUENCE}a_${JAIL}"
  fi
fi

if [ ! -z ${DESTROY} ]; then
  if [ ! -z ${JAIL} ] && [ ! -z ${SEQUENCE} ]; then
    ifconfig "e${SEQUENCE}a_${JAIL}" destroy
  fi
  
  if [ ! -z ${PUBLIC} ]; then
    ifconfig jailetherbridge destroy
  fi
  
  if [ ! -z ${PRIVATE} ]; then
    ifconfig jailprivbridge destroy
  fi
fi