#! /bin/sh

function check()
{
	svn list -R | while read n; do
		if [ "x`svn propget svn:mime-type $n`" = "x" ]; then
			MIMETYPE=""
			case $n in
				*.h)
					MIMETYPE="text/x-chdr; charset=Shift_JIS"
					;;
				*.cpp)
					MIMETYPE="text/x-c++src; charset=Shift_JIS"
					;;
				*.c)
					MIMETYPE="text/x-csrc; charset=Shift_JIS"
					;;
				*.txt)
					MIMETYPE="text/plain; charset=Shift_JIS"
					;;
				*.tjs)
					MIMETYPE="text/x-tjs; charset=Shift_JIS"
					;;
				*.ks)
					MIMETYPE="text/x-kag-scenario; charset=Shift_JIS"
					;;
			esac
			if [ "x$MIMETYPE" != "x" ]; then
					echo "svn propset \"svn:mime-type\" \"$MIMETYPE\" \"$n\""
			fi
		fi
	done
}

check
