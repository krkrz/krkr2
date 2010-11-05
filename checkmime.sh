#! /bin/sh

svn list -R | while read n; do
	if [ "x`svn propget svn:mime-type $n`" = "x" ]; then
		MIMETYPE=""
		case $n in
			*.h)
				MIMETYPE="text/x-chdr"
				;;
			*.cpp)
				MIMETYPE="text/x-c++src"
				;;
			*.c)
				MIMETYPE="text/x-csrc"
				;;
			*.txt)
				MIMETYPE="text/plain"
				;;
			*.tjs)
				MIMETYPE="text/x-tjs"
				;;
			*.ks)
				MIMETYPE="text/x-kag-scenario"
				;;
			*.htm|*.html)
				MIMETYPE="text/html"
				;;
		esac
		if [ "x$MIMETYPE" != "x" ]; then
			CHARSET=`nkf --guess $n`
			if [ "x$CHARSET" != "xASCII" ]; then
				MIMETYPE="$MIMETYPE; charset=$CHARSET"
				echo "svn propset \"svn:mime-type\" \"$MIMETYPE\" \"$n\""
			fi
		fi
	fi
done

