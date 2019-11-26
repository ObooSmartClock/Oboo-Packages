#!/bin/sh

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 path_to_img_dir/" >&2
  exit 1
fi


CARD_DIR=$1

echo $CARD_DIR

for file in $CARD_DIR/png/*.png
do
	# echo "$file"
	baseName=${file##*/}
	baseName=${baseName%.png}
	echo $baseName
	php img_conv_core.php "name=img_$baseName&img=$file&format=bin_rgb565&transp=alpha"
	mv "img_$baseName.bin" "$CARD_DIR/bin/"
done

# rm ./*.c
# rm $CARD_DIR/bin/*

# mkdir bin
# mv ./*.bin $CARD_DIR/bin
