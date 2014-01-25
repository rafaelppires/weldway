set xlabel "Y" offset 1,1,1
set ylabel "Z" offset 1,1,1
plot 'pos.dat' u (-$2/40):($3/40) w lp