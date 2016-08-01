#!/bin/bash -e
#####################################
#
# This program source code file is part of KiCad, a free EDA CAD application.
#
# Copyright (C) 2015 Marco Ciampa <ciampix@libero.it>
# Copyright (C) 2015-2016 KiCAd Developers
#
# License GNU GPL Version 3 or any later version.
#
#####################################

LANG=C

if [ "$1" = "--help" ] || [ "$1" = "-h" ] ; then
  echo "Usage: $0 [-k] [locale]"
  echo
  echo "Where -k means keep pot template and not delete it"
  exit
fi

if [ "$1" = "-k" ] ; then
  KEEP=1
  shift
fi

if [ -z ${SOURCEDIR} ]; then
  SOURCEDIR=../kicad-source-mirror
  echo "Using default SOURCEDIR=${SOURCEDIR}"
fi
CSVFILE=${PWD}/i18n_status.csv

#Autovars
LOCALDIR=$PWD
LINGUAS=`cat LINGUAS|grep -v '^#'|grep -v '^\s*$'` #Read file without comment and empty lines
POTDIRS=`cat POTDIRS|grep -v '^#'|grep -v '^\s*$'` #Read file without comment and empty lines

cd $SOURCEDIR

#Generate source file list
for f in $POTDIRS
do
  find $f -name "*.cpp" >>$LOCALDIR/POTFILES #List files
  find $f -name "*.h"   >>$LOCALDIR/POTFILES #List files
done

#Generate/update template pot file
xgettext -f $LOCALDIR/POTFILES -k_ -k_HKI --force-po --from-code utf-8 -o $LOCALDIR/kicad.pot

rm $LOCALDIR/POTFILES

#check if present in locale list
validate() { echo $LINGUAS | grep -F -q -w "$1"; }

#If supplied, update only the specified locale
if [ ! "$1" = "" ] ; then
  if ! validate "$1"; then
    echo "Error!"
    echo "Locale argument \"$1\" not present in current locale list:"
    for i in $LINGUAS; do echo -n "$i " ; done
    exit 1
  else
    LINGUAS="$1"
  fi
fi

echo "Writing summary to ${CSVFILE}"
echo "LANG;TRANSLATED;FUZZY;UNTRANSLATED" > "${CSVFILE}"

for i in $LINGUAS
do
  echo "## $i"
  msgmerge --force-po $LOCALDIR/$i/kicad.po $LOCALDIR/kicad.pot -o $LOCALDIR/$i/kicad.po 2> /dev/null
  msgfmt --statistics $LOCALDIR/$i/kicad.po 2>&1 >>/dev/null |
    while IFS=",." read A B C D ; do
      echo $A
      echo $B
      echo $C
      echo $D

      for STRING in "$A" "$B" "$C" "$D" ; do
        STRING=${STRING# }
        case "$STRING" in
        *" translated message"* )
          TRANSLATED=${STRING% translated message*}
          ;;
        *" fuzzy translation"* )
          FUZZY=${STRING% fuzzy translation*}
          ;;
        *" untranslated message"* )
          UNTRANSLATED=${STRING% untranslated message*}
          ;;
        "" )
          ;;
        * )
          echo >&2 "$0: Unknown format of \"msgfmt --statistics $LOCALDIR/$i/kicad.po \": \"$STRING\""
          exit 1
          ;;
        esac
      done
      echo "$i;${TRANSLATED};${FUZZY};${UNTRANSLATED}">>"${CSVFILE}"
    done
done

if [ ! "$KEEP" = "1" ]; then
  rm $LOCALDIR/kicad.pot
fi
