rm $(dirname "$2")/*stats
rm $(dirname "$4")/*stats
rm $(dirname "$6")/*stats
rm $(dirname "$8")/*stats
./stat_getter $(dirname "$2")
./stat_getter $(dirname "$4")
./stat_getter $(dirname "$6")
./stat_getter $(dirname "$8")
gnuplot -persist << EOFMarke
        set key outside bmargin
	set terminal jpeg  
        set xlabel 'Iterations' 
        set ylabel 'Cycles' 
	set output "$1"
	plot "$2" using 1 title '$3', "$4" using 1 title '$5', "$6" using 1 title '$7', "$8" using 1 title '$9'
EOFMarke
