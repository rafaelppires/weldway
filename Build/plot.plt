set xlabel "X" offset 1,1,1
set ylabel "Y" offset 1,1,1
set zlabel "Z" offset 1,1,1
splot 'pos.dat' u ($1/40):(-$2/40):($3/40) w lp t "Tocha"