set terminal png
set output 'stackexchange_fn_fp.png'
# set yrange [-0.3:1.2]
# set xrange [-0.3:1.2]
# set logscale y 10

plot './data/stackexchange.txt' every ::0::0 using 2:3:1 with labels offset char 3,0 notitle, '' every ::0::0 using 2:3 with points pt 7 lc rgb "purple" title 'K = 128', \
     '' every ::1::1 using 2:3:1 with labels offset char -3,1  point pt 7 notitle , '' every ::1::1 using 2:3 with points pt 7 lc rgb "purple"  notitle, \
     '' every ::2::2 using 2:3:1 with labels offset char -3,1  point pt 7 notitle , '' every ::2::2 using 2:3 with points pt 7 lc rgb "purple"  notitle, \
     '' every ::3::3 using 2:3:1 with labels offset char 3,0  point pt 7 notitle , '' every ::3::3 using 2:3 with points pt 7 lc rgb "red" title 'K = 256', \
     '' every ::4::4 using 2:3:1 with labels offset char 3,0  point pt 7 notitle , '' every ::4::4 using 2:3 with points pt 7 lc rgb "red"  notitle, \
     '' every ::5::5 using 2:3:1 with labels offset char -3,1  point pt 7 notitle , '' every ::5::5 using 2:3 with points pt 7 lc rgb "red"  notitle, \
     '' every ::6::6 using 2:3:1 with labels offset char 3,0  point pt 7 notitle , '' every ::6::6 using 2:3 with points pt 7 lc rgb "blue" title 'K = 512', \
     '' every ::7::7 using 2:3:1 with labels offset char 3,0  point pt 7 notitle , '' every ::7::7 using 2:3 with points pt 7 lc rgb "blue" notitle, \
     '' every ::8::8 using 2:3:1 with labels offset char -3,1  point pt 7 notitle , '' every ::8::8 using 2:3 with points pt 7 lc rgb "blue" notitle, \
     '' every ::9::9 using 2:3:1 with labels offset char 3,0  point pt 7 notitle , '' every ::9::9 using 2:3 with points pt 7 lc rgb "forest-green" title 'K = 90000'
