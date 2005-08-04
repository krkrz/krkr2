
opendir DH, "ps/reduced";
@files = readdir(DH);
closedir DH;

@mode_list = (
	"ltPsNormal",
	"ltPsAdditive",
	"ltPsSubtractive",
	"ltPsMultiplicative",
	"ltPsScreen",
	"ltPsOverlay",
	"ltPsHardLight",
	"ltPsSoftLight",
	"ltPsColorDodge",
	"ltPsColorDodge5",
	"ltPsColorBurn",
	"ltPsLighten",
	"ltPsDarken",
	"ltPsDifference",
	"ltPsDifference5",
	"ltPsExclusion"
);


open FH , ">index.html" or die;

print FH <<EOF;
<body><html>
<p><img src="grad_base.png"> の上に <img src="grad_fore.png"> を様々なモードで重ね合わせた物です。</p>
<p>Aは全ピクセルの不透明度を 100% にしてレイヤの不透明度 100% で重ね合わせた物、Bは全ピクセルの不透明度を 100% にしてレイヤの不透明度を 50% にして重ね合わせた物、Cは全ピクセルの不透明度を 50% にしてレイヤの不透明度を 100% にして重ね合わせたものです。</p>
<p>画像は、演算結果が分かりやすいように16色に減色してあります。</p>

<table>
	<tr>
		<td>
		</td>
		<td>
			A
		</td>
		<td>
			B
		</td>
		<td>
			C
		</td>
	</tr>
EOF

foreach $mode (@mode_list)
{
	print FH "<tr><td style=\"background:#ffd0ff; color:black;\">PhotoShop<br>$mode</td>";
	print FH "<td style=\"background:#ffd0ff; color:black;\">";
	print FH "<img src=\"ps/reduced/$mode.png\">";
	print FH "</td>";
	print FH "<td style=\"background:#ffd0ff; color:black;\">";
	print FH "<img src=\"ps/reduced/${mode}_o.png\">";
	print FH "</td>";
	print FH "<td style=\"background:#ffd0ff; color:black;\">";
	print FH "<img src=\"ps/reduced/${mode}_h.png\">";
	print FH "</td>";
	print FH "</tr>\n";

	print FH "<tr><td style=\"background:#d0ffff; color:black;\">吉里吉里<br>$mode</td>";
	print FH "<td style=\"background:#d0ffff; color:black;\">";
	print FH "<img src=\"kirikiri/reduced/$mode.png\">";
	print FH "</td>";
	print FH "<td style=\"background:#d0ffff; color:black;\">";
	print FH "<img src=\"kirikiri/reduced/${mode}_o.png\">";
	print FH "</td>";
	print FH "<td style=\"background:#d0ffff; color:black;\">";
	print FH "<img src=\"kirikiri/reduced/${mode}_h.png\">";
	print FH "</td>";
	print FH "</tr>\n";


}

print FH <<EOF;
</table>
</html></body>
EOF



open FH , ">index2.html" or die;

print FH <<EOF;
<body><html>
<p>A は <img src="grad_base.png"> の上に <img src="grad_fore.png"> を不透明度 50% で重ね合わせた画像です。</p>
<p>B は <img src="grad_fore.png"> の上に <img src="grad_base.png"> を不透明度 50% で重ね合わせた画像です。</p>

<table>
	<tr>
		<td>
		</td>
		<td>
			A
		</td>
		<td>
			B
		</td>
	</tr>
EOF

foreach $mode (@mode_list)
{
	print FH "<tr><td style=\"background:#d0ffff; color:black;\">吉里吉里<br>$mode</td>";
	print FH "<td style=\"background:#d0ffff; color:black;\">";
	print FH "<img src=\"kirikiri/reduced/${mode}_o.png\">";
	print FH "</td>";
	print FH "<td style=\"background:#d0ffff; color:black;\">";
	print FH "<img src=\"kirikiri/reduced/${mode}_c.png\">";
	print FH "</td>";
	print FH "</tr>\n";


}

print FH <<EOF;
</table>
</html></body>
EOF

