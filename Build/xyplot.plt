set xlabel "X" offset 1,1,1
set ylabel "Y" offset 1,1,1
plot 'pos.dat' u ($1/40):(-$2/40) w lp t "Projeção XY"